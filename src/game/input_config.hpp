// src/game/input_config.hpp — host-owned controller/input settings for beetle-adventure-racing-recomp.
//
// The runtime input path (single source of truth: bar_input in src/main) is driven by a 4-port
// InputConfig: for each N64 controller port, whether it's plugged in, which physical device drives
// it (keyboard or a specific SDL game controller), which pak is inserted (None / Controller Pak /
// Rumble Pak), and a full set of rebindable button/axis bindings. This module owns the DATA MODEL
// and its persistence to input.json — mirroring bar::config (graphics.json) exactly:
//   * NLOHMANN_JSON_SERIALIZE_ENUM string tables for every enum (unknown string -> first entry,
//     never throws; partial/old files keep defaults via j.value(key, default)),
//   * load(): read input.json (or write defaults on first run),
//   * save(): persist,
//   * default_input_config(): reproduces the pre-Controls single-port behavior byte-for-byte.
//
// It is deliberately runtime-free: it does NOT push anything to the live input path (that is
// bar::input::set_config(), Layer C) and does NOT open any SDL device. The Controls UI (src/ui) and
// startup (src/main) read this model, mutate it, persist via set()/save(), and separately push it to
// the runtime. Kept SDL-free in the header; the .cpp includes only SDL's enum headers (SDL_scancode /
// SDL_gamecontroller) to name the default-binding constants — no SDL runtime dependency.

#ifndef BAR_GAME_INPUT_CONFIG_HPP
#define BAR_GAME_INPUT_CONFIG_HPP

#include <array>
#include <cstdint>
#include <string>
#include <vector>

#include "json/json.hpp"   // nlohmann::json + NLOHMANN_JSON_SERIALIZE_ENUM (same header config.hpp uses)

namespace bar::input_config {

// ----------------------------------------------------------------------------------------
// The canonical list of assignable N64 inputs. Order drives the Controls dialog's row order and is
// the index space of Bindings::map. The 14 buttons carry their N64 button-mask bit (nbits); the 4
// analog half-axes carry nbits=0 (a sentinel — the runtime treats them as stick deflection, not a
// button bit). Keep in sync with kInputInfo[] in input_config.cpp.
// ----------------------------------------------------------------------------------------
enum class N64Input : int {
    A, B, Z, Start, L, R,
    CUp, CDown, CLeft, CRight,
    DUp, DDown, DLeft, DRight,
    AnalogUp, AnalogDown, AnalogLeft, AnalogRight,
    Count
};

// UI-facing metadata: JSON key (stable, for input.json), human label (for the bind row), and the
// N64 button bit this input contributes to a read (0 for the analog half-axes).
struct InputInfo { const char* key; const char* label; uint16_t nbits; };
const InputInfo& input_info(N64Input id);

// ----------------------------------------------------------------------------------------
// A single physical binding. `code` is interpreted per `source`:
//   Keyboard      -> SDL_Scancode           (layout-independent; exactly what SDL_KEYDOWN delivers)
//   GamepadButton -> SDL_GameControllerButton
//   GamepadAxis   -> SDL_GameControllerAxis, with `dir` selecting the half (+1 / -1); e.g. a trigger
//                    is (axis=TRIGGERLEFT, dir=+1), left-stick-left is (axis=LEFTX, dir=-1).
// SDL2's scancode/button/axis enum values are ABI-stable, so storing the int is safe across runs.
// ----------------------------------------------------------------------------------------
enum class BindSource : int { None, Keyboard, GamepadButton, GamepadAxis };

struct Bind {
    BindSource source = BindSource::None;
    int code = 0;
    int dir  = 0;
};

// One physical bind per N64 input, indexed by N64Input.
struct Bindings {
    std::array<Bind, (size_t)N64Input::Count> map{};
};

// Which physical device drives a port. A gamepad is identified primarily by `uid` — a per-physical-
// device stable id built by bar_input as "<guid>!<serial>" when SDL exposes a serial number, else
// "<guid>!#<n>" (n = 1-based ordinal among connected same-GUID pads). The plain SDL joystick GUID
// alone is NOT unique: it encodes bus/vendor/product only, so two identical controllers (or any two
// XInput pads) collide. `guid` is kept for matching configs written before uid existed and as the
// fallback when the exact uid isn't currently connected; display_name is a snapshot for the dropdown.
enum class DeviceType : int { None, Keyboard, Gamepad };
struct DeviceRef {
    DeviceType  type = DeviceType::None;
    std::string guid;           // SDL_JoystickGetGUIDString (empty for None/Keyboard)
    std::string display_name;   // SDL_GameControllerName snapshot (for the UI label)
    std::string uid;            // stable unique device id from bar_input ("" in pre-uid configs)
};

// The accessory inserted into a port. Controller Pak = mempak save store (emulated in the SI stub);
// Rumble Pak = force feedback (SDL rumble). Mutually exclusive on real hardware, so a dropdown.
enum class PakType : int { None, ControllerPak, RumblePak };

struct PortConfig {
    bool        connected = false;   // is a controller plugged into this N64 port
    DeviceRef   device;              // which host device drives it
    // BAR saves ONLY to a Controller Pak (osPfs*; no EEPROM/SRAM/Flash), so a port with no pak makes
    // the game open with "No Controller Pak present. … Start Game Without Saving" and never save.
    // The 32 KiB mempak store below is fully emulated and persisted per port, so default it inserted.
    PakType     pak = PakType::ControllerPak;
    Bindings    bindings;
    std::string profile_name;        // last-applied named profile ("" = custom / unsaved edits)
};

// A named, reusable binding preset in the shared library.
struct BindingProfile {
    std::string name;
    Bindings    bindings;
};

struct InputConfig {
    std::array<PortConfig, 4>   ports;
    std::vector<BindingProfile> profiles;   // shared library; ports reference by name
};

// ----------------------------------------------------------------------------------------
// JSON enum tables (ADL-found by nlohmann). Placed in the header so the Controls UI can convert
// enum<->string the same way bar_ui's enum_to_str/str_to_enum does for graphics enums.
// ----------------------------------------------------------------------------------------
NLOHMANN_JSON_SERIALIZE_ENUM(BindSource, {
    {BindSource::None,          "None"},
    {BindSource::Keyboard,      "Keyboard"},
    {BindSource::GamepadButton, "GamepadButton"},
    {BindSource::GamepadAxis,   "GamepadAxis"},
})
NLOHMANN_JSON_SERIALIZE_ENUM(DeviceType, {
    {DeviceType::None,     "None"},
    {DeviceType::Keyboard, "Keyboard"},
    {DeviceType::Gamepad,  "Gamepad"},
})
NLOHMANN_JSON_SERIALIZE_ENUM(PakType, {
    {PakType::None,          "None"},
    {PakType::ControllerPak, "ControllerPak"},
    {PakType::RumblePak,     "RumblePak"},
})

// ----------------------------------------------------------------------------------------
// Public API (mirrors bar::config).
// ----------------------------------------------------------------------------------------

// The built-in defaults: reproduces the pre-Controls single-port behavior. Port 0 is a plugged-in
// keyboard-driven controller with the historical keymap; ports 1-3 are unplugged. The profile
// library is seeded with "Default Keyboard" and "Default Gamepad".
InputConfig default_input_config();

// True if input.json already exists (i.e. NOT a first run). Call before load() to decide whether to
// apply first-run defaults (e.g. defaulting port 0 to a detected controller).
bool config_file_exists();

// Read input.json from bar::config::get_app_config_directory() (falling back to defaults on any
// error; writing defaults on first run) and make it the canonical config. Returns the loaded config.
// Call once at startup; the caller then pushes it to the runtime (bar::input::set_config).
InputConfig load();

// True if `b` has at least one binding whose source matches physical device `type` (Keyboard vs a
// Gamepad button/axis). Used to decide whether assigning a device to a port should load that device's
// default profile (so a freshly-assigned controller/keyboard works without manual rebinding).
bool bindings_match_device(const Bindings& b, DeviceType type);

// Persist an InputConfig to input.json. Returns false on I/O error.
bool save(const InputConfig& config);

// The current canonical config (a copy the UI edits). Reflects the last load()/set().
InputConfig get();

// Replace the canonical config and persist it. Does NOT push to the runtime — the caller
// (Controls UI / startup) separately calls bar::input::set_config().
void set(const InputConfig& config);

// Named-profile library helpers (operate on and persist the canonical config).
void save_profile(const std::string& name, const Bindings& bindings);   // add or overwrite
const BindingProfile* find_profile(const InputConfig& cfg, const std::string& name);
void delete_profile(const std::string& name);

} // namespace bar::input_config

#endif // BAR_GAME_INPUT_CONFIG_HPP
