// src/game/input_config.cpp — controller/input settings persistence. See input_config.hpp.

#define _CRT_SECURE_NO_WARNINGS

#include "input_config.hpp"

#include <cstdio>
#include <exception>
#include <fstream>
#include <mutex>

#include "config.hpp"        // bar::config::get_app_config_directory()

// SDL enum headers only (no SDL runtime dependency) — to name the default-binding constants below.
#include <SDL_scancode.h>
#include <SDL_gamecontroller.h>

namespace fs = std::filesystem;

namespace bar::input_config {

// ----------------------------------------------------------------------------------------
// N64Input metadata — index-aligned with the N64Input enum. `nbits` is the N64 controller button
// mask each input ORs into a read (from BAR's controller reads, matching bar_poll_keyboard); the 4
// analog half-axes carry 0 (the runtime treats them as stick deflection, not a button bit).
// Labels are plain ASCII (the LatoLatin UI font has no arrow glyphs).
// ----------------------------------------------------------------------------------------
static const InputInfo kInputInfo[(size_t)N64Input::Count] = {
    { "a",            "A",            0x8000 },
    { "b",            "B",            0x4000 },
    { "z",            "Z",            0x2000 },
    { "start",        "Start",        0x1000 },
    { "l",            "L",            0x0020 },
    { "r",            "R",            0x0010 },
    { "c_up",         "C-Up",         0x0008 },
    { "c_down",       "C-Down",       0x0004 },
    { "c_left",       "C-Left",       0x0002 },
    { "c_right",      "C-Right",      0x0001 },
    { "d_up",         "D-Pad Up",     0x0800 },
    { "d_down",       "D-Pad Down",   0x0400 },
    { "d_left",       "D-Pad Left",   0x0200 },
    { "d_right",      "D-Pad Right",  0x0100 },
    { "analog_up",    "Analog Up",    0x0000 },
    { "analog_down",  "Analog Down",  0x0000 },
    { "analog_left",  "Analog Left",  0x0000 },
    { "analog_right", "Analog Right", 0x0000 },
};

const InputInfo& input_info(N64Input id) {
    int i = (int)id;
    if (i < 0 || i >= (int)N64Input::Count) i = 0;
    return kInputInfo[i];
}

// ----------------------------------------------------------------------------------------
// JSON (de)serialization. to_json for every struct (used by save); from_json for the leaf structs
// (Bind/Bindings/DeviceRef) so the explicit overlay loader can read them with j.value(key, default).
// Defined in dependency order so ADL resolves nested calls. Mirrors config.cpp's resilience: missing
// keys keep defaults, unknown enum strings fall back to the enum's first entry (never throws).
// ----------------------------------------------------------------------------------------
static void to_json(nlohmann::json& j, const Bind& b) {
    j = nlohmann::json{ {"source", b.source}, {"code", b.code}, {"dir", b.dir} };
}
static void from_json(const nlohmann::json& j, Bind& b) {
    b.source = j.value("source", BindSource::None);
    b.code   = j.value("code", 0);
    b.dir    = j.value("dir", 0);
}

static void to_json(nlohmann::json& j, const Bindings& bnd) {
    j = nlohmann::json::object();
    for (int i = 0; i < (int)N64Input::Count; i++) {
        j[kInputInfo[i].key] = bnd.map[i];
    }
}
static void from_json(const nlohmann::json& j, Bindings& bnd) {
    for (int i = 0; i < (int)N64Input::Count; i++) {
        bnd.map[i] = j.value(kInputInfo[i].key, Bind{});
    }
}

static void to_json(nlohmann::json& j, const DeviceRef& d) {
    j = nlohmann::json{ {"type", d.type}, {"guid", d.guid}, {"name", d.display_name}, {"uid", d.uid} };
}
static void from_json(const nlohmann::json& j, DeviceRef& d) {
    d.type         = j.value("type", DeviceType::None);
    d.guid         = j.value("guid", std::string{});
    d.display_name = j.value("name", std::string{});
    d.uid          = j.value("uid", std::string{});   // "" in pre-uid configs -> guid fallback matching
}

static void to_json(nlohmann::json& j, const PortConfig& p) {
    j = nlohmann::json{
        {"connected", p.connected},
        {"device",    p.device},
        {"pak",       p.pak},
        {"bindings",  p.bindings},
        {"profile",   p.profile_name},
    };
}

static void to_json(nlohmann::json& j, const BindingProfile& pr) {
    j = nlohmann::json{ {"name", pr.name}, {"bindings", pr.bindings} };
}

static nlohmann::json input_to_json(const InputConfig& c) {
    nlohmann::json j;
    j["ports"] = nlohmann::json::array();
    for (const auto& p : c.ports) j["ports"].push_back(p);
    j["profiles"] = nlohmann::json::array();
    for (const auto& pr : c.profiles) j["profiles"].push_back(pr);
    return j;
}

// Overlay a parsed input.json onto the defaults (so missing keys keep the default binding, exactly
// like graphics_from_json). Ports are read positionally; profiles fully replace the seeded library.
static InputConfig input_from_json(const nlohmann::json& j) {
    InputConfig c = default_input_config();
    if (j.contains("ports") && j["ports"].is_array()) {
        const auto& arr = j["ports"];
        for (int i = 0; i < 4 && i < (int)arr.size(); i++) {
            const nlohmann::json& pj = arr[i];
            PortConfig& p = c.ports[i];            // start from the default port
            p.connected    = pj.value("connected", p.connected);
            p.device       = pj.value("device", p.device);
            p.pak          = pj.value("pak", p.pak);
            p.bindings     = pj.value("bindings", p.bindings);
            p.profile_name = pj.value("profile", p.profile_name);
        }
    }
    if (j.contains("profiles") && j["profiles"].is_array()) {
        c.profiles.clear();
        for (const auto& pj : j["profiles"]) {
            BindingProfile bp;
            bp.name     = pj.value("name", std::string{});
            bp.bindings = pj.value("bindings", Bindings{});
            if (!bp.name.empty()) c.profiles.push_back(std::move(bp));
        }
    }
    return c;
}

// ----------------------------------------------------------------------------------------
// Default bindings.
// ----------------------------------------------------------------------------------------
static Bind kb(SDL_Scancode sc)                 { return Bind{ BindSource::Keyboard,      (int)sc, 0 }; }
static Bind gb(SDL_GameControllerButton btn)    { return Bind{ BindSource::GamepadButton, (int)btn, 0 }; }
static Bind ga(SDL_GameControllerAxis ax, int d){ return Bind{ BindSource::GamepadAxis,   (int)ax, d }; }

static void set_bind(Bindings& b, N64Input in, const Bind& bind) { b.map[(int)in] = bind; }

// The historical keyboard map from bar_poll_keyboard (main.cpp), converted VK -> SDL_Scancode. This
// is what runs by default (port 0's device is Keyboard), so it must match the old behavior exactly.
static Bindings default_keyboard_bindings() {
    Bindings b;
    set_bind(b, N64Input::A,     kb(SDL_SCANCODE_X));
    set_bind(b, N64Input::B,     kb(SDL_SCANCODE_Z));
    set_bind(b, N64Input::Z,     kb(SDL_SCANCODE_LSHIFT));
    set_bind(b, N64Input::Start, kb(SDL_SCANCODE_RETURN));
    set_bind(b, N64Input::L,     kb(SDL_SCANCODE_Q));
    set_bind(b, N64Input::R,     kb(SDL_SCANCODE_W));
    set_bind(b, N64Input::CUp,   kb(SDL_SCANCODE_I));
    set_bind(b, N64Input::CDown, kb(SDL_SCANCODE_K));
    set_bind(b, N64Input::CLeft, kb(SDL_SCANCODE_J));
    set_bind(b, N64Input::CRight,kb(SDL_SCANCODE_L));
    set_bind(b, N64Input::DUp,   kb(SDL_SCANCODE_T));
    set_bind(b, N64Input::DDown, kb(SDL_SCANCODE_G));
    set_bind(b, N64Input::DLeft, kb(SDL_SCANCODE_F));
    set_bind(b, N64Input::DRight,kb(SDL_SCANCODE_H));
    set_bind(b, N64Input::AnalogUp,    kb(SDL_SCANCODE_UP));
    set_bind(b, N64Input::AnalogDown,  kb(SDL_SCANCODE_DOWN));
    set_bind(b, N64Input::AnalogLeft,  kb(SDL_SCANCODE_LEFT));
    set_bind(b, N64Input::AnalogRight, kb(SDL_SCANCODE_RIGHT));
    return b;
}

// The SDL game-controller map from bar_sample_gamepad (main.cpp): face buttons, shoulders, LT->Z,
// D-pad, right-stick -> C-buttons, left-stick -> analog. Seeded as a named profile users can apply.
static Bindings default_gamepad_bindings() {
    Bindings b;
    set_bind(b, N64Input::A,     gb(SDL_CONTROLLER_BUTTON_A));
    set_bind(b, N64Input::B,     gb(SDL_CONTROLLER_BUTTON_B));
    set_bind(b, N64Input::Start, gb(SDL_CONTROLLER_BUTTON_START));
    set_bind(b, N64Input::L,     gb(SDL_CONTROLLER_BUTTON_LEFTSHOULDER));
    set_bind(b, N64Input::R,     gb(SDL_CONTROLLER_BUTTON_RIGHTSHOULDER));
    set_bind(b, N64Input::Z,     ga(SDL_CONTROLLER_AXIS_TRIGGERLEFT, +1));
    set_bind(b, N64Input::DUp,   gb(SDL_CONTROLLER_BUTTON_DPAD_UP));
    set_bind(b, N64Input::DDown, gb(SDL_CONTROLLER_BUTTON_DPAD_DOWN));
    set_bind(b, N64Input::DLeft, gb(SDL_CONTROLLER_BUTTON_DPAD_LEFT));
    set_bind(b, N64Input::DRight,gb(SDL_CONTROLLER_BUTTON_DPAD_RIGHT));
    set_bind(b, N64Input::CUp,   ga(SDL_CONTROLLER_AXIS_RIGHTY, -1));
    set_bind(b, N64Input::CDown, ga(SDL_CONTROLLER_AXIS_RIGHTY, +1));
    set_bind(b, N64Input::CLeft, ga(SDL_CONTROLLER_AXIS_RIGHTX, -1));
    set_bind(b, N64Input::CRight,ga(SDL_CONTROLLER_AXIS_RIGHTX, +1));
    set_bind(b, N64Input::AnalogUp,    ga(SDL_CONTROLLER_AXIS_LEFTY, -1));
    set_bind(b, N64Input::AnalogDown,  ga(SDL_CONTROLLER_AXIS_LEFTY, +1));
    set_bind(b, N64Input::AnalogLeft,  ga(SDL_CONTROLLER_AXIS_LEFTX, -1));
    set_bind(b, N64Input::AnalogRight, ga(SDL_CONTROLLER_AXIS_LEFTX, +1));
    return b;
}

InputConfig default_input_config() {
    InputConfig c;
    // Port 0: a plugged-in keyboard controller with the historical map (unchanged default behavior).
    c.ports[0].connected    = true;
    c.ports[0].device.type  = DeviceType::Keyboard;
    // BAR saves only via osPfs* (Controller Pak); with no pak the game opens on
    // "No Controller Pak present. … Start Game Without Saving" and can never save. The 32 KiB
    // per-port mempak store in bar_input.cpp is fully emulated and persisted, so ship it inserted.
    c.ports[0].pak          = PakType::ControllerPak;
    c.ports[0].bindings     = default_keyboard_bindings();
    c.ports[0].profile_name = "Default Keyboard";
    // Ports 1-3: unplugged, no device (as the SI stub reported before Controls).
    for (int i = 1; i < 4; i++) {
        c.ports[i] = PortConfig{};   // connected=false, device None, pak None, empty binds
    }
    // Seed the shared profile library.
    c.profiles.push_back(BindingProfile{ "Default Keyboard", default_keyboard_bindings() });
    c.profiles.push_back(BindingProfile{ "Default Gamepad",  default_gamepad_bindings() });
    return c;
}

// ----------------------------------------------------------------------------------------
// Canonical config + persistence.
// ----------------------------------------------------------------------------------------
static std::mutex   g_mutex;
static InputConfig  g_config = default_input_config();
static bool         g_loaded = false;

bool config_file_exists() {
    std::error_code ec;
    return fs::exists(bar::config::get_app_config_directory() / "input.json", ec);
}

bool bindings_match_device(const Bindings& b, DeviceType type) {
    for (const auto& bind : b.map) {
        if (type == DeviceType::Keyboard && bind.source == BindSource::Keyboard) return true;
        if (type == DeviceType::Gamepad &&
            (bind.source == BindSource::GamepadButton || bind.source == BindSource::GamepadAxis)) return true;
    }
    return false;
}

bool save(const InputConfig& config) {
    const fs::path path = bar::config::get_app_config_directory() / "input.json";
    try {
        std::ofstream out(path, std::ios::trunc);
        out << input_to_json(config).dump(4) << '\n';
        return out.good();
    } catch (const std::exception& e) {
        std::fprintf(stderr, "[beetle-adventure-racing-recomp] failed to write \"%s\": %s\n", path.string().c_str(), e.what());
        return false;
    }
}

InputConfig load() {
    InputConfig config = default_input_config();
    const fs::path path = bar::config::get_app_config_directory() / "input.json";
    std::error_code ec;
    if (fs::exists(path, ec)) {
        try {
            std::ifstream in(path);
            nlohmann::json j;
            in >> j;
            config = input_from_json(j);
            std::fprintf(stderr, "[beetle-adventure-racing-recomp] loaded input config from \"%s\"\n", path.string().c_str());
        } catch (const std::exception& e) {
            std::fprintf(stderr, "[beetle-adventure-racing-recomp] input.json parse error (%s); using defaults\n", e.what());
            config = default_input_config();
        }
    } else {
        save(config);   // first run: write defaults so the file is discoverable/hand-editable
        std::fprintf(stderr, "[beetle-adventure-racing-recomp] no input.json; wrote defaults to \"%s\"\n", path.string().c_str());
    }
    {
        std::lock_guard<std::mutex> lk(g_mutex);
        g_config = config;
        g_loaded = true;
    }
    return config;
}

InputConfig get() {
    std::lock_guard<std::mutex> lk(g_mutex);
    return g_config;
}

void set(const InputConfig& config) {
    {
        std::lock_guard<std::mutex> lk(g_mutex);
        g_config = config;
        g_loaded = true;
    }
    save(config);
}

void save_profile(const std::string& name, const Bindings& bindings) {
    if (name.empty()) return;
    InputConfig c = get();
    bool replaced = false;
    for (auto& pr : c.profiles) {
        if (pr.name == name) { pr.bindings = bindings; replaced = true; break; }
    }
    if (!replaced) c.profiles.push_back(BindingProfile{ name, bindings });
    set(c);
}

const BindingProfile* find_profile(const InputConfig& cfg, const std::string& name) {
    for (const auto& pr : cfg.profiles) {
        if (pr.name == name) return &pr;
    }
    return nullptr;
}

void delete_profile(const std::string& name) {
    InputConfig c = get();
    for (auto it = c.profiles.begin(); it != c.profiles.end(); ++it) {
        if (it->name == name) { c.profiles.erase(it); break; }
    }
    set(c);
}

} // namespace bar::input_config
