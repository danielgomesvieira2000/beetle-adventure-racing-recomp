// src/main/bar_input.cpp — live 4-port runtime input path. See bar_input.hpp.

#ifdef _WIN32
#  define _CRT_SECURE_NO_WARNINGS
#endif

#include "main/bar_input.hpp"

#include <array>
#include <atomic>
#include <cstdio>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <memory>
#include <mutex>
#include <vector>

#include <SDL.h>

#include "game/config.hpp"   // bar::config::get_app_config_directory (mempak file location)

namespace fs = std::filesystem;

using bar::input_config::InputConfig;
using bar::input_config::PortConfig;
using bar::input_config::Bindings;
using bar::input_config::Bind;
using bar::input_config::BindSource;
using bar::input_config::DeviceType;
using bar::input_config::PakType;
using bar::input_config::N64Input;
using bar::input_config::input_info;

namespace {

// --- thresholds (match the pre-Controls hardcoded values in bar_sample_gamepad) ---
constexpr int kAxisButtonThreshold = 12000;  // an axis bound to a button (e.g. trigger->Z) fires past this
constexpr int kAxisDeadzone        = 8000;   // an axis bound to the analog stick starts contributing past this
constexpr int kAnalogMax           = 80;     // N64 stick range

// --- live config snapshot (immutable, swapped on set_config) ---
std::mutex g_cfg_mutex;
std::shared_ptr<const InputConfig> g_cfg = std::make_shared<const InputConfig>(bar::input_config::default_input_config());

std::shared_ptr<const InputConfig> cfg_snapshot() {
    std::lock_guard<std::mutex> lk(g_cfg_mutex);
    return g_cfg;
}

// --- per-port gamepad snapshots (written on the main thread, read on the SI thread) ---
// Packed like the pre-Controls g_pad_state: buttons(0-15) | sx(16-23) | sy(24-31).
std::array<std::atomic<uint32_t>, 4> g_port_state{};
// The first connected controller not explicitly assigned to any port, sampled with the Default
// Gamepad map and folded into a Keyboard-driven port 0 — preserves the old "keyboard + a plugged pad
// both drive player 1" convenience without an explicit assignment.
std::atomic<uint32_t> g_auto_pad_state{0};

// --- rumble intent (set on any thread, consumed on the main thread) ---
std::array<std::atomic<bool>, 4> g_rumble_want{};

// --- open SDL controllers (MAIN THREAD ONLY) ---
struct OpenPad {
    SDL_GameController* gc = nullptr;
    SDL_JoystickID     id  = -1;
    std::string        guid;
    std::string        uid;    // stable unique id: guid + "!" + serial, or guid + "!#<n>" ordinal
    std::string        name;
};
std::vector<OpenPad> g_pads;        // main thread only
std::mutex           g_pads_mutex;  // guards enumerate_devices() vs. add/remove (both main thread, but be safe)

std::string guid_string(SDL_JoystickGUID g) {
    char buf[33] = {};
    SDL_JoystickGetGUIDString(g, buf, sizeof(buf));
    return std::string(buf);
}

uint32_t pack(uint16_t btn, int sx, int sy) {
    if (sx >  kAnalogMax) sx =  kAnalogMax; else if (sx < -kAnalogMax) sx = -kAnalogMax;
    if (sy >  kAnalogMax) sy =  kAnalogMax; else if (sy < -kAnalogMax) sy = -kAnalogMax;
    return (uint32_t)btn
         | ((uint32_t)(uint8_t)(int8_t)sx << 16)
         | ((uint32_t)(uint8_t)(int8_t)sy << 24);
}

// Scale a raw axis deflection (already flipped so positive == the bound direction) to 0..kAnalogMax.
int axis_to_analog(int deflection) {
    if (deflection < kAxisDeadzone) return 0;
    int v = deflection * kAnalogMax / 32767;
    return v > kAnalogMax ? kAnalogMax : v;
}

// Sample one open controller through `binds` into a packed word. MAIN THREAD (reads SDL directly).
uint32_t sample_pad(SDL_GameController* gc, const Bindings& binds) {
    if (gc == nullptr || !SDL_GameControllerGetAttached(gc)) return 0;
    uint16_t btn = 0;
    int nx = 0, ny = 0;
    for (int i = 0; i < (int)N64Input::Count; i++) {
        const Bind& bd = binds.map[i];
        bool active = false;
        int  deflection = 0;   // for axis binds: magnitude in the bound direction (>=0 when active)
        if (bd.source == BindSource::GamepadButton) {
            active = SDL_GameControllerGetButton(gc, (SDL_GameControllerButton)bd.code) != 0;
        } else if (bd.source == BindSource::GamepadAxis) {
            int v = SDL_GameControllerGetAxis(gc, (SDL_GameControllerAxis)bd.code);
            deflection = (bd.dir < 0) ? -v : v;
            active = deflection > kAxisButtonThreshold;
        } else {
            continue;   // keyboard (or None) binds don't participate in a gamepad sample
        }
        const uint16_t nbits = input_info((N64Input)i).nbits;
        if (nbits != 0) {
            if (active) btn |= nbits;
        } else {
            int contrib = (bd.source == BindSource::GamepadAxis) ? axis_to_analog(deflection)
                                                                 : (active ? kAnalogMax : 0);
            switch ((N64Input)i) {
                case N64Input::AnalogRight: nx += contrib; break;
                case N64Input::AnalogLeft:  nx -= contrib; break;
                case N64Input::AnalogUp:    ny += contrib; break;   // N64 stick up is positive
                case N64Input::AnalogDown:  ny -= contrib; break;
                default: break;
            }
        }
    }
    return pack(btn, nx, ny);
}

// Resolve each configured gamepad port to an open pad index (into g_pads), or -1. Caller holds
// g_pads_mutex. Two passes so an exact uid assignment always wins over a legacy/replug fallback:
//   1. exact uid match (the specific physical device the user picked in the Controls dialog),
//   2. GUID fallback for the rest (pre-uid configs, or a serial-less pad whose ordinal changed),
//      greedy by port order so two identical pads still land on distinct ports deterministically.
// `claimed` marks which g_pads indices were consumed (for the port-0 auto-fold of unassigned pads).
struct PadResolution { std::array<int, 4> port_pad; std::array<bool, 16> claimed; };
PadResolution resolve_pads(const InputConfig& cfg) {
    PadResolution r;
    r.port_pad.fill(-1);
    r.claimed.fill(false);
    auto wants_pad = [&](int port) {
        const PortConfig& pc = cfg.ports[port];
        return pc.connected && pc.device.type == DeviceType::Gamepad;
    };
    for (int port = 0; port < 4; port++) {   // pass 1: exact uid
        if (!wants_pad(port) || cfg.ports[port].device.uid.empty()) continue;
        for (int i = 0; i < (int)g_pads.size() && i < 16; i++) {
            if (!r.claimed[i] && g_pads[i].uid == cfg.ports[port].device.uid) {
                r.port_pad[port] = i;
                r.claimed[i] = true;
                break;
            }
        }
    }
    for (int port = 0; port < 4; port++) {   // pass 2: GUID fallback
        if (!wants_pad(port) || r.port_pad[port] >= 0 || cfg.ports[port].device.guid.empty()) continue;
        for (int i = 0; i < (int)g_pads.size() && i < 16; i++) {
            if (!r.claimed[i] && g_pads[i].guid == cfg.ports[port].device.guid) {
                r.port_pad[port] = i;
                r.claimed[i] = true;
                break;
            }
        }
    }
    return r;
}

} // namespace

namespace bar::input {

void set_config(const InputConfig& cfg) {
    auto next = std::make_shared<const InputConfig>(cfg);
    std::lock_guard<std::mutex> lk(g_cfg_mutex);
    g_cfg = std::move(next);
}

void on_controller_added(int sdl_device_index) {
    if (!SDL_IsGameController(sdl_device_index)) return;
    SDL_GameController* gc = SDL_GameControllerOpen(sdl_device_index);
    if (gc == nullptr) {
        std::fprintf(stderr, "[beetle-adventure-racing-recomp] SDL_GameControllerOpen(%d) failed: %s\n", sdl_device_index, SDL_GetError());
        return;
    }
    SDL_Joystick* js = SDL_GameControllerGetJoystick(gc);
    OpenPad pad;
    pad.gc   = gc;
    pad.id   = SDL_JoystickInstanceID(js);
    pad.guid = guid_string(SDL_JoystickGetGUID(js));
    const char* nm = SDL_GameControllerName(gc);
    pad.name = (nm != nullptr && *nm != '\0') ? nm : "Controller";
    // The GUID alone is not unique (it's bus/vendor/product — identical for two same-model pads and
    // for ALL XInput pads), so derive a per-physical-device uid: prefer the hardware serial number
    // (stable across sessions/replug), else the smallest free ordinal among connected same-GUID pads
    // (stable within a session; across sessions it follows connection order).
    const char* serial = SDL_JoystickGetSerial(js);
    {
        std::lock_guard<std::mutex> lk(g_pads_mutex);
        // Guard against a duplicate open for an instance we already track.
        for (const auto& p : g_pads) {
            if (p.id == pad.id) { SDL_GameControllerClose(gc); return; }
        }
        auto uid_taken = [&](const std::string& u) {
            for (const auto& p : g_pads) { if (p.uid == u) return true; }
            return false;
        };
        if (serial != nullptr && *serial != '\0' && !uid_taken(pad.guid + "!" + serial)) {
            pad.uid = pad.guid + "!" + serial;   // taken = clone pads reporting one shared serial -> ordinal
        } else {
            int n = 1;
            while (uid_taken(pad.guid + "!#" + std::to_string(n))) n++;
            pad.uid = pad.guid + "!#" + std::to_string(n);
        }
        g_pads.push_back(pad);
    }
    std::fprintf(stderr, "[beetle-adventure-racing-recomp] gamepad connected: %s [%s]\n", pad.name.c_str(), pad.uid.c_str());
}

void on_controller_removed(int sdl_instance_id) {
    std::lock_guard<std::mutex> lk(g_pads_mutex);
    for (auto it = g_pads.begin(); it != g_pads.end(); ++it) {
        if (it->id == sdl_instance_id) {
            if (it->gc != nullptr) SDL_GameControllerClose(it->gc);
            std::fprintf(stderr, "[beetle-adventure-racing-recomp] gamepad disconnected: %s\n", it->name.c_str());
            g_pads.erase(it);
            return;
        }
    }
}

std::vector<DeviceEntry> enumerate_devices() {
    std::lock_guard<std::mutex> lk(g_pads_mutex);
    std::vector<DeviceEntry> out;
    out.reserve(g_pads.size());
    for (const auto& p : g_pads) out.push_back(DeviceEntry{ p.guid, p.uid, p.name, p.name });
    // Disambiguate identically-named pads in the dropdown: "Xbox One Controller (1)" / "(2)", numbered
    // in connection order. Singletons keep their bare name.
    for (auto& d : out) {
        int total = 0;
        for (const auto& o : out) { if (o.name == d.name) total++; }
        if (total < 2) continue;
        int ordinal = 1;
        for (const auto& o : out) {
            if (&o == &d) break;
            if (o.name == d.name) ordinal++;
        }
        d.display = d.name + " (" + std::to_string(ordinal) + ")";
    }
    return out;
}

void sample_all_ports() {
    auto cfg = cfg_snapshot();
    std::lock_guard<std::mutex> lk(g_pads_mutex);

    // Explicit per-port gamepad assignments first (so they claim their pads before the auto-fold).
    const PadResolution res = resolve_pads(*cfg);
    for (int port = 0; port < 4; port++) {
        const int idx = res.port_pad[port];
        g_port_state[port].store(idx >= 0 ? sample_pad(g_pads[idx].gc, cfg->ports[port].bindings) : 0,
                                 std::memory_order_relaxed);
    }
    const std::array<bool, 16>& claimed = res.claimed;

    // Auto pad: the first unclaimed controller, folded into a keyboard-driven port 0 with the Default
    // Gamepad map (matches the old "plug in a pad, it drives player 1 alongside the keyboard").
    uint32_t auto_state = 0;
    if (cfg->ports[0].connected && cfg->ports[0].device.type == DeviceType::Keyboard) {
        const bar::input_config::BindingProfile* gp =
            bar::input_config::find_profile(*cfg, "Default Gamepad");
        for (int i = 0; i < (int)g_pads.size() && i < 16; i++) {
            if (!claimed[i]) {
                auto_state = gp != nullptr ? sample_pad(g_pads[i].gc, gp->bindings) : 0;
                break;
            }
        }
    }
    g_auto_pad_state.store(auto_state, std::memory_order_relaxed);
}

void flush_rumble() {
    auto cfg = cfg_snapshot();
    std::lock_guard<std::mutex> lk(g_pads_mutex);
    // Track last-sent state per pad index so we only (re)issue on change.
    static std::array<bool, 16> last_on{};
    std::array<bool, 16> want{};
    std::array<bool, 16> touched{};
    const PadResolution res = resolve_pads(*cfg);
    for (int port = 0; port < 4; port++) {
        const PortConfig& pc = cfg->ports[port];
        // Any connected gamepad may rumble, regardless of which pak the port is configured with.
        // The SI layer serves the Controller Pak data area and the rumble motor register from the
        // same transaction stream (see bar_handle_pak), so a port presents both accessories at once
        // and the player never has to swap. A pad that cannot rumble is simply a no-op below.
        if (!(pc.connected && pc.device.type == DeviceType::Gamepad && pc.pak != PakType::None)) continue;
        const int idx = res.port_pad[port];   // same routing the sampler uses (uid-exact, GUID fallback)
        if (idx < 0) continue;
        touched[idx] = true;
        if (g_rumble_want[port].load(std::memory_order_relaxed)) want[idx] = true;
    }

    // Auto pad: mirror the input auto-fold above. A keyboard-driven port 0 with an unclaimed
    // controller also receives port 0's rumble, so simply plugging a pad in gives both steering and
    // vibration with no assignment step — which matters now that a port presents a Controller Pak
    // and a Rumble Pak at once, and doubly so while there is no Controls UI to assign devices in.
    if (cfg->ports[0].connected && cfg->ports[0].device.type == DeviceType::Keyboard
        && cfg->ports[0].pak != PakType::None) {
        for (int i = 0; i < (int)g_pads.size() && i < 16; i++) {
            if (!res.claimed[i]) {
                touched[i] = true;
                if (g_rumble_want[0].load(std::memory_order_relaxed)) want[i] = true;
                break;
            }
        }
    }

    for (int i = 0; i < (int)g_pads.size() && i < 16; i++) {
        if (!touched[i]) { last_on[i] = false; continue; }
        if (want[i] != last_on[i]) {
            SDL_GameControllerRumble(g_pads[i].gc, want[i] ? 0xFFFF : 0, want[i] ? 0xFFFF : 0,
                                     want[i] ? 0xFFFFFFFFu : 0);
            last_on[i] = want[i];
        }
    }
}

uint16_t resolve_port(int port, int8_t* sx, int8_t* sy) {
    if (sx) *sx = 0;
    if (sy) *sy = 0;
    if (port < 0 || port >= 4) return 0;
    auto cfg = cfg_snapshot();
    const PortConfig& pc = cfg->ports[port];
    if (!pc.connected || pc.device.type == DeviceType::None) return 0;

    uint16_t btn = 0;
    int nx = 0, ny = 0;

    if (pc.device.type == DeviceType::Keyboard) {
        const Uint8* ks = SDL_GetKeyboardState(nullptr);
        for (int i = 0; i < (int)N64Input::Count; i++) {
            const Bind& bd = pc.bindings.map[i];
            if (bd.source != BindSource::Keyboard) continue;
            if (bd.code <= 0 || bd.code >= SDL_NUM_SCANCODES) continue;
            const bool down = (ks != nullptr) && ks[bd.code] != 0;
            if (!down) continue;
            const uint16_t nbits = input_info((N64Input)i).nbits;
            if (nbits != 0) {
                btn |= nbits;
            } else {
                switch ((N64Input)i) {
                    case N64Input::AnalogRight: nx += kAnalogMax; break;
                    case N64Input::AnalogLeft:  nx -= kAnalogMax; break;
                    case N64Input::AnalogUp:    ny += kAnalogMax; break;
                    case N64Input::AnalogDown:  ny -= kAnalogMax; break;
                    default: break;
                }
            }
        }
        // Fold in the auto (unassigned) pad for the keyboard-driven port 0.
        const uint32_t s = g_auto_pad_state.load(std::memory_order_relaxed);
        btn |= (uint16_t)(s & 0xFFFF);
        if (nx == 0 && ny == 0) {
            nx = (int)(int8_t)((s >> 16) & 0xFF);
            ny = (int)(int8_t)((s >> 24) & 0xFF);
        }
    } else { // Gamepad — read the pre-sampled snapshot
        const uint32_t s = g_port_state[port].load(std::memory_order_relaxed);
        btn = (uint16_t)(s & 0xFFFF);
        nx  = (int)(int8_t)((s >> 16) & 0xFF);
        ny  = (int)(int8_t)((s >> 24) & 0xFF);
    }

    if (nx >  kAnalogMax) nx =  kAnalogMax; else if (nx < -kAnalogMax) nx = -kAnalogMax;
    if (ny >  kAnalogMax) ny =  kAnalogMax; else if (ny < -kAnalogMax) ny = -kAnalogMax;
    if (sx) *sx = (int8_t)nx;
    if (sy) *sy = (int8_t)ny;
    return btn;
}

bool port_connected(int port) {
    if (port < 0 || port >= 4) return false;
    auto cfg = cfg_snapshot();
    const PortConfig& pc = cfg->ports[port];
    return pc.connected && pc.device.type != DeviceType::None;
}

PakType port_pak(int port) {
    if (port < 0 || port >= 4) return PakType::None;
    auto cfg = cfg_snapshot();
    const PortConfig& pc = cfg->ports[port];
    if (!(pc.connected && pc.device.type != DeviceType::None)) return PakType::None;
    return pc.pak;
}

void set_rumble(int port, bool on) {
    if (port < 0 || port >= 4) return;
    g_rumble_want[port].store(on, std::memory_order_relaxed);
}

} // namespace bar::input

// ==========================================================================================
// Controller Pak (mempak) storage — a faithful 32 KiB-per-port backing store persisted to disk. The
// SI stub (os_unimpl_stubs.cpp) does the joybus wire format (address/data CRC, PIF layout); this owns
// the bytes + persistence. A fresh pak is written as a valid formatted-empty N64 Controller Pak so it
// presents a valid ID + free page table; whatever the game writes afterwards is stored faithfully.
// ==========================================================================================
namespace {

constexpr int kMempakSize = 0x8000;   // 32 KiB
std::mutex g_mempak_mutex;
std::array<std::vector<uint8_t>, 4> g_mempak;       // empty until first touched
std::array<bool, 4> g_mempak_loaded{};
std::array<bool, 4> g_mempak_dirty{};

// __osSumcalc / __osIdCheckSum ported from lib/bar-decomp tools/ultralib/src/io/{contpfs.c} (2.0I).
uint16_t pfs_sumcalc(const uint8_t* p, int len) {
    uint32_t sum = 0;
    for (int i = 0; i < len; i++) { sum += p[i]; sum &= 0xFFFF; }
    return (uint16_t)sum;
}

// Build a valid formatted-empty Controller Pak image (1 bank / 128 pages). ID block (4 copies) with
// valid checksums, an inode/FAT with all data pages free, empty backup + directory. Constants from
// os_pfs.h: ID areas at blocks 1/3/4/6, label at block 7, PFS_ONE_PAGE=8 blocks=256B, 128 FAT entries,
// free-page marker = 3, inode_start_page = 1 + DEF_DIR_PAGES(2) + 2*banks(1) = 5.
void format_empty_mempak(std::vector<uint8_t>& m) {
    m.assign(kMempakSize, 0);

    // ---- ID block (__OSPackId, 32 bytes) ----
    uint8_t id[32] = {};
    id[0x18] = 0x00; id[0x19] = 0x01;   // deviceid = 1 (bit0 set => valid pak)
    id[0x1A] = 0x01;                    // banks = 1 (32 KiB)
    id[0x1B] = 0x00;                    // version
    uint16_t csum = 0, icsum = 0;       // __osIdCheckSum: sum of big-endian u16s over the first 0x1C bytes
    for (int j = 0; j < 0x1C; j += 2) {
        uint16_t d = (uint16_t)((id[j] << 8) | id[j + 1]);
        csum  += d;
        icsum += (uint16_t)~d;
    }
    id[0x1C] = (uint8_t)(csum >> 8);  id[0x1D] = (uint8_t)csum;
    id[0x1E] = (uint8_t)(icsum >> 8); id[0x1F] = (uint8_t)icsum;
    for (int blk : {1, 3, 4, 6}) std::memcpy(&m[blk * 32], id, 32);
    // label (block 7) left zero.

    // ---- inode/FAT page (page 1 = byte 256) + backup (page 2 = byte 512) ----
    // 128 big-endian u16 entries; data pages (>= inode_start_page 5) = 3 (free). Entry 0 low byte holds
    // the page checksum over entries [5..127]. Then mirror to the backup page.
    uint8_t page[256] = {};
    for (int i = 5; i < 128; i++) { page[i * 2] = 0x00; page[i * 2 + 1] = 0x03; }   // free
    const uint16_t inode_sum = pfs_sumcalc(&page[5 * 2], (128 - 5) * 2);
    page[0] = 0x00;                       // entry 0 bank byte
    page[1] = (uint8_t)(inode_sum & 0xFF);// entry 0 page byte = checksum low byte
    std::memcpy(&m[1 * 256], page, 256);
    std::memcpy(&m[2 * 256], page, 256);
    // directory pages (3-4) left zero (all entries free/empty).
}

fs::path mempak_path(int port) {
    return bar::config::get_app_config_directory() / ("mempak_p" + std::to_string(port) + ".pak");
}

void mempak_ensure_loaded(int port) {   // caller holds g_mempak_mutex
    if (g_mempak_loaded[port]) return;
    g_mempak[port].assign(kMempakSize, 0);
    const fs::path p = mempak_path(port);
    std::error_code ec;
    bool ok = false;
    if (fs::exists(p, ec)) {
        std::ifstream in(p, std::ios::binary);
        if (in) {
            in.read(reinterpret_cast<char*>(g_mempak[port].data()), kMempakSize);
            if (in.gcount() == kMempakSize) ok = true;
        }
    }
    if (!ok) {
        format_empty_mempak(g_mempak[port]);
        std::ofstream out(p, std::ios::binary | std::ios::trunc);
        if (out) out.write(reinterpret_cast<const char*>(g_mempak[port].data()), kMempakSize);
        std::fprintf(stderr, "[beetle-adventure-racing-recomp] created Controller Pak \"%s\"\n", p.string().c_str());
    }
    g_mempak_loaded[port] = true;
    g_mempak_dirty[port] = false;
}

} // namespace

namespace bar::input {

bool mempak_read(int port, int block, uint8_t out[32]) {
    if (port < 0 || port >= 4 || block < 0) return false;
    const int off = block * 32;
    if (off + 32 > kMempakSize) return false;
    std::lock_guard<std::mutex> lk(g_mempak_mutex);
    mempak_ensure_loaded(port);
    std::memcpy(out, &g_mempak[port][off], 32);
    return true;
}

bool mempak_write(int port, int block, const uint8_t in[32]) {
    if (port < 0 || port >= 4 || block < 0) return false;
    const int off = block * 32;
    if (off + 32 > kMempakSize) return false;
    std::lock_guard<std::mutex> lk(g_mempak_mutex);
    mempak_ensure_loaded(port);
    std::memcpy(&g_mempak[port][off], in, 32);
    g_mempak_dirty[port] = true;
    return true;
}

void mempak_flush_all() {
    std::lock_guard<std::mutex> lk(g_mempak_mutex);
    for (int port = 0; port < 4; port++) {
        if (!g_mempak_loaded[port] || !g_mempak_dirty[port]) continue;
        const fs::path p = mempak_path(port);
        std::ofstream out(p, std::ios::binary | std::ios::trunc);
        if (out) {
            out.write(reinterpret_cast<const char*>(g_mempak[port].data()), kMempakSize);
            g_mempak_dirty[port] = false;
        }
    }
}

} // namespace bar::input
