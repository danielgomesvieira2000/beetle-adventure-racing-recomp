#include "frontend/bar_frontend.h"

#include <algorithm>
#include <cmath>
#include <cstdio>

#include "recompui/recompui.h"
#include "recompui/program_config.h"
#include "recompui/renderer.h"
#include "recompui/config.h"
#include "recompinput/input_events.h"
#include "recompinput/input_mapping.h"
#include "recompinput/input_state.h"
#include "recompinput/players.h"
#include "recompinput/profiles.h"
#include "librecomp/game.hpp"
#include "ultramodern/config.hpp"
#include <filesystem>
#include <string>
#include <variant>
#include <vector>

namespace {

// Must match the recomp::GameEntry registered in main.cpp — recompui drives the launcher's
// "Load ROM" / "Start Game" state from librecomp's validation of this id, which is in turn backed by
// the entry's XXH3-64 rom_hash. A mismatch here silently yields a launcher that can never start.
constexpr const char8_t* kGameId      = u8"bar.n64.us";
constexpr const char*    kModGameId   = "bar";
constexpr const char*    kDisplayName = "Beetle Adventure Racing!";

// How many controller ports this port serves. Beetle Adventure Racing does four-player split-screen,
// so it is the console's four -- which is also recompinput's own maximum.
//
// It is the single place that decides it: the Controls tab's player slots, which pads
// refresh_players will assign, and the ports poll_port and port_assigned answer for all come from
// here.
constexpr int kMaxPlayers = 4;

// Assets are resolved as <exe dir>/assets/<name> (recompui::file::get_asset_path). The primary font
// is REQUIRED: recompui throws "No primary font was registered" without it. The family name is what
// the stylesheet must reference, and is not necessarily the filename.
constexpr const char* kPrimaryFontFile   = "LatoLatin-Regular.ttf";
constexpr const char* kPrimaryFontFamily = "LatoLatin";   // the font file's real family name

// TEMPORARY bring-up tracing. The frontend build currently exits during renderer/UI setup with no
// diagnostic, so these mark how far the sequence gets. Remove once it starts cleanly.
void trace(const char* msg) {
    std::fprintf(stderr, "[frontend] %s\n", msg);
    std::fflush(stderr);
}

} // namespace

// Implemented in src/main/bar_frustum.cpp, which owns the frustum the game draws and culls with.
extern "C" void bar_set_draw_distance(float scale);

// Implemented in src/main/main.cpp, which owns the SDL audio device the finished samples go to.
// The Sound tab's Main Volume used to be inert: recompui defines the slider and reads it back, and
// nothing upstream ever applies it -- the port is expected to, and this one did not.
extern "C" void bar_set_audio_volume(double percent);
extern "C" void bar_set_mute_when_unfocused(bool mute);

// Draw Distance — a BAR-specific option appended to recompui's prefab Graphics tab.
//
// It is added from here rather than from RecompFrontend because that submodule is upstream
// (N64Recomp/RecompFrontend) and this port cannot push to it; the tab's Config is returned by
// create_graphics_tab(), so an extra option can simply be added to it. It has to happen BEFORE
// recompui::config::finalize(), which is what loads the JSON — an option added afterwards would
// never see its saved value.
//
// The value reaches the renderer through bar_set_draw_distance (src/main/bar_frustum.cpp), which
// multiplies the far plane of BAR's own view frustum — and, because the game derives its culling
// planes from the same numbers, the distance it is willing to submit geometry for.
namespace {

// Keys are what land in graphics.json, so they must stay stable; the third field is the label.
const std::vector<recomp::config::ConfigOptionEnumOption> kDrawDistanceOptions = {
    { 0u, "1x", "1x" },
    { 1u, "2x", "2x" },
    { 2u, "4x", "4x" },
};

const std::string kDrawDistanceOption = "draw_distance";

float draw_distance_scale_for(uint32_t index) {
    switch (index) {
        case 0:  return 1.0f;
        case 1:  return 2.0f;
        default: return 4.0f;
    }
}

void add_draw_distance_option() {
    recomp::config::Config &config = recompui::config::get_config(recompui::config::graphics::id);
    config.add_enum_option(
        kDrawDistanceOption,
        "Draw Distance",
        "How far into the distance the game draws. The original hardware drew 300 units ahead; "
        "<recomp-color primary>2x</recomp-color> and <recomp-color primary>4x</recomp-color> extend "
        "that, and extend the distance the game submits geometry for to match, so scenery stops "
        "appearing out of the fog.",
        kDrawDistanceOptions,
        2u /* 4x, matching the port's default */
    );
}

// Read the applied (not pending) value and hand it to the renderer. Called after finalize() and once
// per frame from pump_events, rather than through set_save_callback, because the Graphics tab has
// already installed its own save callback and replacing it would stop every other graphics setting
// applying. A map lookup per frame is far cheaper than that bug.
void push_draw_distance() {

    const recomp::config::ConfigValueVariant value =
        recompui::config::get_config(recompui::config::graphics::id).get_option_value(kDrawDistanceOption);
    if (const uint32_t *index = std::get_if<uint32_t>(&value)) {
        bar_set_draw_distance(draw_distance_scale_for(*index));
    }
}

} // namespace

void bar::frontend::install() {
    // BAR_DBG_UI=1 sends the frontend's diagnostics to bar_ui_trace.log in the working directory.
    // Worth keeping: this build links as /SUBSYSTEM:WINDOWS, so it has no console and shell
    // redirection of stderr captures nothing — without this, frontend problems are silent.
    if (std::getenv("BAR_DBG_UI") != nullptr) {
        static std::FILE* log = std::freopen("bar_ui_trace.log", "w", stderr);
        (void)log;
    }

    trace("install: begin");

    recompui::programconfig::set_program_name(kDisplayName);
    recompui::programconfig::set_program_id(kGameId);

    recompui::register_primary_font(kPrimaryFontFile, kPrimaryFontFamily);

    // Built when recompui creates its menus. The library owns the ROM flow entirely:
    // add_start_game_or_load_rom_option() shows "Load ROM" until librecomp reports a valid ROM for
    // kGameId and "Start Game" afterwards, with a native file picker behind it — so ROM loading and
    // hash verification need no code on this side.
    recompui::register_launcher_init_callback([](recompui::LauncherMenu* menu) {
        trace("launcher init callback: begin");

        recompui::GameOptionsMenu* options = menu->init_game_options_menu(
            kGameId, kModGameId, kDisplayName,
            // No thumbnail: any artwork for the game itself would be taken from the cartridge, and
            // this project ships nothing derived from a dump. The launcher lays out fine without one.
            {},
            recompui::GameOptionsMenuLayout::Center);

        // The launcher's background, which is a different thing from the game thumbnail above: this
        // is the project's OWN icon (icons/Icon.png, wrapped into an SVG by
        // scripts/make-launcher-logo.py), so it carries none of the cartridge's restrictions.
        //
        // The default title is deliberately kept, unlike wave-race-64-recomp, which removes it
        // because its artwork spells the game's name out. This one is only the icon, so the
        // library's title label is what still names the game. The two do not collide: recompui puts
        // the title at 25% of the window height and the menu at the bottom quarter, and the SVG's
        // 680x120 viewBox makes the icon land in the band between them (see the generator script).
        menu->set_launcher_background_svg("icons/Logo.svg");

        options->add_start_game_or_load_rom_option();
        options->add_setup_controls_option();
        options->add_settings_option();
        // Mods. The runtime half has worked all along -- recomp::start() calls initialize_mods() and
        // scan_mods() itself, and main.cpp gives librecomp this game's mod id -- but with no entry
        // to the tab, a mod could be installed and never appear, never be enabled, and never be
        // reported broken. The tab lists what was found; this opens it without starting the game.
        options->add_mods_option();
        // "Quit" rather than the library's "Exit", matching this project's reference port. It calls
        // ultramodern::quit(), which unwinds the game thread and the renderer in order rather than
        // tearing the process down, so a race in progress still writes its Controller Pak records.
        options->add_exit_option("Quit");

        trace("launcher init callback: menu built");
    });

    // The keyboard scheme, declared as the frontend's DEFAULTS rather than left to RecompFrontend,
    // whose own defaults are a different scheme again (WASD and space).
    //
    // These are wave-race-64-recomp's bindings, deliberately, so that the two ports play the same
    // way from the same keyboard. They are NOT the scheme this port's older documentation described
    // (which had B on Z and the Z trigger on Left Shift, with L and R on Q and W); that scheme was
    // inherited from the bespoke bar::input tables, which nothing reads any more in this build.
    //
    // Defaults apply to a profile the first time it is created; a keyboard profile already saved in
    // controls.json keeps whatever it holds until it is reset in the Controls tab.
    {
        using recompinput::GameInput;
        using recompinput::InputField;
        const struct { GameInput input; SDL_Scancode key; } keys[] = {
            { GameInput::X_AXIS_NEG,  SDL_SCANCODE_LEFT },
            { GameInput::X_AXIS_POS,  SDL_SCANCODE_RIGHT },
            { GameInput::Y_AXIS_POS,  SDL_SCANCODE_UP },
            { GameInput::Y_AXIS_NEG,  SDL_SCANCODE_DOWN },
            { GameInput::A,           SDL_SCANCODE_X },
            { GameInput::B,           SDL_SCANCODE_C },
            { GameInput::Z,           SDL_SCANCODE_Z },
            { GameInput::START,       SDL_SCANCODE_RETURN },
            { GameInput::L,           SDL_SCANCODE_A },
            { GameInput::R,           SDL_SCANCODE_S },
            // The C buttons work the camera, which BAR uses constantly, so they stay under the
            // right hand while the left drives.
            { GameInput::C_UP,        SDL_SCANCODE_I },
            { GameInput::C_DOWN,      SDL_SCANCODE_K },
            { GameInput::C_LEFT,      SDL_SCANCODE_J },
            { GameInput::C_RIGHT,     SDL_SCANCODE_L },
            { GameInput::DPAD_UP,     SDL_SCANCODE_T },
            { GameInput::DPAD_DOWN,   SDL_SCANCODE_G },
            { GameInput::DPAD_LEFT,   SDL_SCANCODE_F },
            { GameInput::DPAD_RIGHT,  SDL_SCANCODE_H },
        };
        for (const auto& binding : keys) {
            recompinput::set_default_mapping_for_keyboard(
                binding.input, { InputField::keyboard(binding.key) });
        }
    }

    // Four player slots in the Controls tab, because the game has four. This is not a narrowing --
    // it is what stops the tab assuming a single player.
    //
    // Which pad is which is then not a choice anyone should have to make before their first race:
    // pads are assigned in the order they are connected, so the first is player one and the second
    // is player two (see refresh_players in pump_events below), and the tab's "Assign players" modal
    // is left for anyone who wants to override that.
    recompinput::players::set_player_count_range(1, kMaxPlayers);

    // Config tabs. recompui's create_menus() calls config::init_modal(), which throws
    // "Configurations have not been loaded. Call recompui::config::finalize() first." unless the
    // tabs exist and finalize() has run — so this must happen here, before recomp::start() brings
    // the renderer up. Each tab persists to its own <id>.json in the app config directory.
    //
    // Rumble strength is enabled because this port serves the rumble motor register alongside the
    // Controller Pak, so a pad really can vibrate; gyro and mouse are off, since BAR has no use for
    // either.
    recompui::config::create_general_tab(recompui::config::GeneralTabOptions{
        .has_rumble_strength  = true,
        .has_gyro_sensitivity = false,
        .has_mouse_sensitivity = false,
    });
    recompui::config::create_graphics_tab();
    add_draw_distance_option();

    // Sound. Main Volume did nothing until this. recompui defines the slider and reads it back, and
    // nothing upstream ever applies it -- the port is expected to, and this one was not, so the
    // control was inert in every build so far. The callback covers all three ways the value
    // changes: Load, when the saved setting is read at startup; Temporary, while the slider is being
    // dragged, which is what makes it audible as you move it; and Permanent, on Apply.
    recomp::config::Config& sound = recompui::config::create_sound_tab();
    sound.add_option_change_callback(
        recompui::config::sound::options::main_volume,
        [](recomp::config::ConfigValueVariant value, recomp::config::ConfigValueVariant,
           recomp::config::OptionChangeContext) {
            if (const double* percent = std::get_if<double>(&value)) {
                bar_set_audio_volume(*percent);
            }
        });
    sound.add_bool_option(
        "mute_unfocused", "Mute When Not In Focus",
        "Silences the game while another window has focus.",
        true);
    sound.add_option_change_callback(
        "mute_unfocused",
        [](recomp::config::ConfigValueVariant value, recomp::config::ConfigValueVariant,
           recomp::config::OptionChangeContext) {
            if (const bool* mute = std::get_if<bool>(&value)) {
                bar_set_mute_when_unfocused(*mute);
            }
        });
    // There is no Music Volume beside those two, and the reason belongs here rather than in a
    // ticket: Main Volume is applied to the finished buffer, which is the only place this port can
    // reach. Music and effects are already mixed together by the time the samples arrive, so
    // separating them means reaching into the game's own sequence players -- work that has not been
    // done for BAR. wave-race-64-recomp has the option because that work was done there.

    recompui::config::create_controls_tab();
    recompui::config::create_mods_tab();
    recompui::config::finalize();

    // finalize() is what reads the JSON, so the saved values only exist after it. Draw Distance is
    // pushed every frame from pump_events (see push_draw_distance); the audio settings have real
    // change callbacks, which finalize() fires with OptionChangeContext::Load, so they need no
    // per-frame poll.
    push_draw_distance();

    // Fullscreen at the display's own resolution and aspect ratio on a first run. Two thirds of
    // that are already the library's defaults -- resolution is Auto, which renders at the window's
    // true pixel size rather than upscaling 320x240, and aspect ratio is Expand -- and only the
    // window mode defaults to Windowed.
    //
    // This runs after finalize() and only when there is no saved graphics.json, which is what makes
    // it a first-run default rather than an override: choose Windowed in the menu and that choice is
    // written and respected from then on. Setting it before finalize() does not work, because the
    // option map does not exist until the JSON has loaded.
    //
    // Both copies are set. recompui owns the value the menu shows, ultramodern owns the one the
    // renderer reads, and the graphics tab syncs the two on change; setting only one leaves the menu
    // and the window disagreeing.
    //
    // The scripted-run environment variables suppress it. A headless capture run
    // (docs/technical/08-diagnostics-and-methodology.md) is watched in a window while it plays, and
    // a fresh config directory is exactly the state those runs start from -- so without this check,
    // every scripted run on a clean profile would take over the display.
    const bool scripted_run = std::getenv("BAR_AUTOPLAY") != nullptr ||
                              std::getenv("BAR_SHOTS") != nullptr ||
                              std::getenv("BAR_SHOT_BURST") != nullptr;
    if (!scripted_run && !std::filesystem::exists(recomp::get_config_path() / "graphics.json")) {
        recompui::config::get_graphics_config().set_option_value(
            recompui::config::graphics::options::wm_option,
            static_cast<uint32_t>(ultramodern::renderer::WindowMode::Fullscreen));

        ultramodern::renderer::GraphicsConfig gfx = ultramodern::renderer::get_graphics_config();
        gfx.wm_option = ultramodern::renderer::WindowMode::Fullscreen;
        ultramodern::renderer::set_graphics_config(gfx);

        trace("no saved graphics settings; defaulting to fullscreen at the display's size");
    }

    // Input bindings. This is what makes the menus DRIVEABLE, and nothing else calls it:
    // load_controls_config() is the only public entry point that reaches
    // profiles::initialize_input_bindings(), which builds the key/button -> menu-action mapping
    // recompui navigates with. Without it the UI still renders and RmlUi still highlights on hover
    // (its own hit-testing), but no key or button maps to Accept/Back/navigate, so nothing can be
    // activated — the menu looks alive and is completely inert.
    //
    // The call also seeds sensible keyboard and controller defaults when the file does not exist
    // yet, and writes controls.json alongside the other config files.
    const std::filesystem::path controls_path = recomp::get_config_path() / "controls.json";
    const bool loaded = recompinput::profiles::load_controls_config(controls_path);
    trace(loaded ? "controls config loaded" : "controls config created from defaults");

    trace("install: done");
}


namespace {

// Who is player one, and who is player two, three and four.
//
// The frontend's own answer is a modal in the Controls tab: it opens, each player presses a button
// on the pad they want, and the assignment is committed. That suits a game where which pad is which
// matters and cannot be guessed. Until someone has been through it, though, NOTHING is assigned --
// so a pad that is plugged in and working drives nothing, with no hint anywhere that a modal in a
// settings tab is what stands between it and the game.
//
// So the pads are assigned here instead, in the order SDL reports them, whenever that set changes:
// plug one in and it is player one, plug a second in and it is player two, up to the game's four.
// With no pad at all the keyboard becomes player one, so the game is still playable -- and player one
// keeps the keyboard whatever else it has, because get_n64_input merges a player's controller and
// keyboard profiles.
// scripts/patch-recompinput.py adds auto_assign_controllers; the modal still wins while it is open,
// for anyone who wants to choose.
void refresh_players() {
    std::vector<SDL_GameController*> connected;
    for (int i = 0; i < SDL_NumJoysticks(); ++i) {
        if (!SDL_IsGameController(i)) continue;
        // Opening an already-open device returns the existing handle rather than opening it twice,
        // so rescanning every frame is cheap and correct rather than a workaround. It has to be a
        // rescan: recompinput::handle_events() is the only thing draining SDL's queue, and
        // SDL_PollEvent removes what it returns, so a hotplug loop of our own would never see a
        // CONTROLLERDEVICEADDED at all.
        if (SDL_GameController* pad = SDL_GameControllerOpen(i)) {
            connected.push_back(pad);
        }
    }

    // The first call always assigns, even with nothing connected. Comparing against the last set
    // alone is what breaks input entirely on a machine with no pad attached: an empty set matches an
    // empty set, so nothing is ever assigned, player one does not exist, and the game is told it has
    // no controller at all -- keyboard included.
    static bool assigned_once = false;
    static std::vector<SDL_GameController*> assigned;
    if (assigned_once && connected == assigned) {
        return;
    }
    assigned_once = true;
    assigned = connected;

    recompinput::players::auto_assign_controllers(connected.data(), connected.size());

    std::fprintf(stderr, "[beetle-adventure-racing-recomp] %zu controller%s connected; assigned to "
                         "%zu player%s\n",
                 connected.size(), connected.size() == 1 ? "" : "s",
                 recompinput::players::get_number_of_assigned_players(),
                 recompinput::players::get_number_of_assigned_players() == 1 ? "" : "s");
    std::fflush(stderr);
}

} // namespace

void bar::frontend::pump_events() {
    // recompinput owns the SDL pump: it polls, filters, forwards to recompui, and applies cursor
    // visibility / relative-mouse mode from recompui's flag (recompui itself never calls SDL).
    recompinput::handle_events();

    // poll_inputs() is what fills recompinput's keyboard snapshot from SDL, and like update_rumble
    // it is exposed for the port to call rather than called by the library itself. Without it,
    // InputState.keys stays null and every keyboard binding reads as unpressed -- while a pad keeps
    // working, because the controller path asks the player's own SDL handle directly. The result is
    // a dead keyboard beside a perfect gamepad, with nothing in the settings to suggest why.
    recompinput::poll_inputs();

    refresh_players();

    // recompinput ramps the motor towards the level the General tab's Rumble Strength asks for, so
    // this has to run every frame rather than only when a port changes its request. bar::input still
    // decides WHICH ports want to rumble (main.cpp's input_set_rumble hands it over).
    recompinput::update_rumble();

    // Pick up Draw Distance as soon as the player applies it. See push_draw_distance().
    push_draw_distance();
}

bool bar::frontend::port_assigned(int port) {
    if (port < 0 || port >= kMaxPlayers) {
        return false;
    }
    // Player one always exists, because a keyboard is always attached: refresh_players gives player
    // one the single-player keyboard profile as well as whatever pad it has, and get_n64_input
    // merges the two, so the keys and the pad both play at any moment without either having to be
    // chosen. Ports two to four exist once their pad has been plugged in.
    return port == 0 || recompinput::players::get_player_is_assigned(port);
}

void bar::frontend::set_port_rumble(int port, bool on) {
    if (port < 0 || port >= kMaxPlayers) {
        return;
    }
    recompinput::set_rumble(port, on);
}

uint16_t bar::frontend::poll_port(int port, int8_t* stick_x, int8_t* stick_y) {
    if (stick_x != nullptr) *stick_x = 0;
    if (stick_y != nullptr) *stick_y = 0;
    if (!port_assigned(port)) {
        return 0;
    }

    uint16_t pressed = 0;
    float x = 0.0f;
    float y = 0.0f;
    recompinput::profiles::get_n64_input(port, &pressed, &x, &y);

    // get_n64_input returns the stick already normalised to [-1, 1] and deadzoned. BAR reads the
    // raw joybus bytes, which are the N64's own signed range; 80 is the value the hardware stick
    // reaches at full deflection and what the rest of this port has always scaled to.
    constexpr float kN64Range = 80.0f;
    auto to_n64 = [](float v) {
        return static_cast<int8_t>(std::lround(std::clamp(v, -1.0f, 1.0f) * kN64Range));
    };
    if (stick_x != nullptr) *stick_x = to_n64(x);
    if (stick_y != nullptr) *stick_y = to_n64(y);
    return pressed;
}

bool bar::frontend::menu_capturing_input() {
    return recompui::is_context_capturing_input();
}

std::unique_ptr<ultramodern::renderer::RendererContext>
bar::frontend::create_render_context(uint8_t* rdram,
                                     ultramodern::renderer::WindowHandle window_handle,
                                     bool developer_mode) {
    // Console presentation: present strictly from the VI origin, at VI time, exactly as hardware
    // does. BAR's main-menu film-roll transition steps the VI origin across a framebuffer without
    // ever redrawing, so any mode that presents only freshly-rendered content collapses the pan to
    // an instant swap. Do not "optimise" this to PresentEarly/SkipBuffering.
    trace("create_render_context: begin");

    // RT64's developer mode, on unconditionally.
    //
    // Every path to RT64's debug UI is gated on it: the F1 key handler, the event filter RT64
    // installs for itself, and State::inspect() at the other end -- which is also where the port's
    // HUD inspector is drawn. A debug menu that only exists in a build made for it is a debug menu
    // nobody has when they need it; the person looking at a misplaced HUD element is running the
    // game they downloaded.
    //
    // Nothing is drawn until F1 is pressed: RT64 creates its inspector on the keystroke and
    // State::inspect() returns immediately while there is none, so the cost of leaving this on is a
    // null check per frame. F2 is unbound in the fork for the same reason -- see
    // lib/rt64/src/hle/rt64_application.cpp.
    (void)developer_mode;
    auto ctx = recompui::renderer::create_render_context(
        rdram, window_handle, ultramodern::renderer::PresentationMode::Console, true);

    trace(ctx ? "create_render_context: ok" : "create_render_context: returned null");
    return ctx;
}
