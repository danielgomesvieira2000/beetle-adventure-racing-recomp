// src/game/config.cpp — host-owned settings persistence. See config.hpp for the contract.

#define _CRT_SECURE_NO_WARNINGS   // std::getenv on MSVC (project also builds with -Wno-everything)

#include "config.hpp"

#include <cstdio>
#include <cstdlib>
#include <exception>
#include <fstream>
#include <string>

#include "json/json.hpp"   // nlohmann::json (vendored by ultramodern; same header config.hpp uses)

#if defined(_WIN32)
#   ifndef WIN32_LEAN_AND_MEAN
#       define WIN32_LEAN_AND_MEAN
#   endif
#   include <Windows.h>
#endif

namespace fs = std::filesystem;
using ultramodern::renderer::GraphicsConfig;

namespace {

// Directory containing the running executable (for portable.txt detection). Falls back to cwd.
fs::path executable_dir() {
#if defined(_WIN32)
    wchar_t buf[MAX_PATH];
    DWORD n = GetModuleFileNameW(nullptr, buf, MAX_PATH);
    if (n == 0 || n >= MAX_PATH) {
        return fs::current_path();
    }
    return fs::path(buf, buf + n).parent_path();
#else
    std::error_code ec;
    fs::path p = fs::read_symlink("/proc/self/exe", ec);
    if (ec) {
        return fs::current_path();
    }
    return p.parent_path();
#endif
}

// One-time migration of the pre-rename settings directory.
//
// This app used to be called "BeetleRecomp" and stored its settings under that name. Renaming it
// moved the directory, which would have silently presented every existing player with a fresh
// first run: no key bindings, no graphics settings, no cached ROM (so the launcher would sit on
// "Load ROM" again), and no Controller Pak saves. So if the new directory does not exist yet but
// the old one does, rename the old one into place.
//
// This is the only place the former name survives in code, and it is deliberate: it is a legacy
// path to be found, not a name this program answers to.
//
// fs::rename is atomic on the same volume and both paths are siblings under the same config root,
// so there is no partially-migrated state to reason about. Every failure is non-fatal -- the app
// carries on with an empty new directory, exactly as it would have without this function.
void migrate_legacy_config_directory(const fs::path& new_dir) {
    static constexpr const char* kLegacyName = "BeetleRecomp";

    std::error_code ec;
    if (fs::exists(new_dir, ec)) {
        return;                                  // already migrated, or a genuinely fresh install
    }

    const fs::path legacy = new_dir.parent_path() / kLegacyName;
    if (!fs::is_directory(legacy, ec)) {
        return;                                  // nothing to migrate
    }

    fs::rename(legacy, new_dir, ec);
    if (ec) {
        std::fprintf(stderr, "[beetle-adventure-racing-recomp] could not migrate settings from \"%s\" (%s); "
                             "starting with fresh settings\n", legacy.string().c_str(), ec.message().c_str());
        return;
    }
    std::fprintf(stderr, "[beetle-adventure-racing-recomp] migrated settings from \"%s\"\n",
                 legacy.string().c_str());
}

fs::path compute_app_config_directory() {
    std::error_code ec;
    const fs::path exe_dir = executable_dir();
    if (fs::exists(exe_dir / "portable.txt", ec)) {
        return exe_dir;
    }
#if defined(_WIN32)
    if (const char* local = std::getenv("LOCALAPPDATA")) {
        return fs::path(local) / "beetle-adventure-racing-recomp";
    }
#else
    if (const char* xdg = std::getenv("XDG_CONFIG_HOME")) {
        return fs::path(xdg) / "beetle-adventure-racing-recomp";
    }
    if (const char* home = std::getenv("HOME")) {
        return fs::path(home) / ".config" / "beetle-adventure-racing-recomp";
    }
#endif
    return fs::current_path();
}

// graphics.json has two writers, and for one field they disagree about the vocabulary.
//
// This file is one writer. The other is RecompFrontend's settings menu, which persists through
// librecomp's own Config (`<tab id>.json` under the same directory, so also `graphics.json`) using
// the display names in `ui_config_tab_graphics.cpp`. Every enum's names match across the two except
// HUDRatioMode::Full, which is "Full" to ultramodern's NLOHMANN_JSON_SERIALIZE_ENUM table and
// "Expand" to the menu -- the menu's label for it, matching the Aspect Ratio option beside it.
//
// Neither side throws on a name it does not know; both quietly fall back to a default. So writing
// "Full" here made the menu reset HUD Placement to its own default (Clamp16x9), and writing "Expand"
// there made this loader read HUD Ratio as Original -- which silently disables HUD anchoring, since
// Original collapses every extended-GBI origin back to the frame's centre. Both were observed.
//
// So: accept either spelling on the way in, and write the menu's spelling on the way out, because
// the menu is the writer that wins whenever the frontend build is running.
static const char *hud_ratio_to_string(ultramodern::renderer::HUDRatioMode mode) {
    switch (mode) {
        case ultramodern::renderer::HUDRatioMode::Clamp16x9: return "Clamp16x9";
        case ultramodern::renderer::HUDRatioMode::Full:      return "Expand";
        case ultramodern::renderer::HUDRatioMode::Original:
        default:                                             return "Original";
    }
}

static ultramodern::renderer::HUDRatioMode hud_ratio_from_string(const std::string& s,
                                                                 ultramodern::renderer::HUDRatioMode fallback) {
    using ultramodern::renderer::HUDRatioMode;
    if ((s == "Expand") || (s == "Full"))    return HUDRatioMode::Full;
    if ((s == "Clamp16x9") || (s == "16:9")) return HUDRatioMode::Clamp16x9;
    if (s == "Original")                     return HUDRatioMode::Original;
    return fallback;
}

nlohmann::json graphics_to_json(const GraphicsConfig& c) {
    // Enum fields serialize as strings via the NLOHMANN_JSON_SERIALIZE_ENUM tables in config.hpp,
    // except hr_option -- see the note above.
    return nlohmann::json{
        {"developer_mode",  c.developer_mode},
        {"res_option",      c.res_option},
        {"wm_option",       c.wm_option},
        {"hr_option",       hud_ratio_to_string(c.hr_option)},
        {"api_option",      c.api_option},
        {"ar_option",       c.ar_option},
        {"msaa_option",     c.msaa_option},
        {"rr_option",       c.rr_option},
        {"hpfb_option",     c.hpfb_option},
        {"rr_manual_value", c.rr_manual_value},
        {"ds_option",       c.ds_option},
        {"divot_option",    c.divot_option},
        {"present_fill_mode", c.pfm_option},
    };
}

// Missing keys keep their default; unknown enum strings fall back to the enum's first entry
// (nlohmann's enum-serializer behavior) rather than throwing, so partial/old files still load.
GraphicsConfig graphics_from_json(const nlohmann::json& j) {
    GraphicsConfig c = bar::config::default_graphics_config();
    c.developer_mode  = j.value("developer_mode",  c.developer_mode);
    c.res_option      = j.value("res_option",      c.res_option);
    c.wm_option       = j.value("wm_option",       c.wm_option);
    c.hr_option       = hud_ratio_from_string(j.value("hr_option", std::string{}), c.hr_option);
    c.api_option      = j.value("api_option",      c.api_option);
    c.ar_option       = j.value("ar_option",       c.ar_option);
    c.msaa_option     = j.value("msaa_option",     c.msaa_option);
    c.rr_option       = j.value("rr_option",       c.rr_option);
    c.hpfb_option     = j.value("hpfb_option",     c.hpfb_option);
    c.rr_manual_value = j.value("rr_manual_value", c.rr_manual_value);
    c.ds_option       = j.value("ds_option",       c.ds_option);
    c.divot_option    = j.value("divot_option",    c.divot_option);
    c.pfm_option      = j.value("present_fill_mode", c.pfm_option);
    return c;
}

} // namespace

namespace bar::config {

const fs::path& get_app_config_directory() {
    static const fs::path dir = [] {
        fs::path d = compute_app_config_directory();
        // Before create_directories() below makes the new path exist, and so before anything can
        // read a setting out of it.
        migrate_legacy_config_directory(d);
        std::error_code ec;
        fs::create_directories(d, ec);
        if (ec) {
            std::fprintf(stderr, "[beetle-adventure-racing-recomp] could not create config dir \"%s\" (%s); using cwd\n",
                         d.string().c_str(), ec.message().c_str());
            return fs::current_path();
        }
        return d;
    }();
    return dir;
}

GraphicsConfig default_graphics_config() {
    using namespace ultramodern::renderer;
    GraphicsConfig c{};
    c.developer_mode  = false;
    c.res_option      = Resolution::Auto;                     // display-native: render at the window/monitor resolution (integer scale)
    c.wm_option       = WindowMode::Windowed;
    c.hr_option       = HUDRatioMode::Full;                   // widescreen HUD: anchor the racing HUD to the widened frame's edges
    c.api_option      = GraphicsApi::Auto;                    // RT64 picks D3D12 on Windows / Vulkan on Linux
    c.ar_option       = AspectRatio::Original;                // BASELINE: plain 4:3 first, widescreen after
    c.msaa_option     = Antialiasing::MSAA4X;                  // seam fix 2A: 4x MSAA adds edge coverage, softening coplanar seams
    c.rr_option       = RefreshRate::Display;                 // HIGH-FPS: interpolate up to the monitor rate
    c.hpfb_option     = HighPrecisionFramebuffer::Auto;
    c.rr_manual_value = 144;                                  // only consulted when rr_option == Manual
    c.ds_option       = 1;
    c.divot_option    = DivotFilter::Auto;                    // seam fix 2B: match the game's VI divot bit (on for BAR)
    c.pfm_option      = PresentFillMode::Pillarbox;           // BASELINE: whole picture, true proportions
    return c;
}

GraphicsConfig load_and_apply_graphics() {
    GraphicsConfig config = default_graphics_config();
    const fs::path path = get_app_config_directory() / "graphics.json";
    std::error_code ec;
    if (fs::exists(path, ec)) {
        try {
            std::ifstream in(path);
            nlohmann::json j;
            in >> j;
            config = graphics_from_json(j);
            std::fprintf(stderr, "[beetle-adventure-racing-recomp] loaded graphics config from \"%s\"\n", path.string().c_str());
        } catch (const std::exception& e) {
            std::fprintf(stderr, "[beetle-adventure-racing-recomp] graphics.json parse error (%s); using defaults\n", e.what());
            config = default_graphics_config();
        }
    } else {
        // First run: write defaults so the file is discoverable and hand-editable until the menu exists.
        save_graphics(config);
        std::fprintf(stderr, "[beetle-adventure-racing-recomp] no graphics.json; wrote defaults to \"%s\"\n", path.string().c_str());
    }
    ultramodern::renderer::set_graphics_config(config);
    return config;
}

bool save_graphics(const GraphicsConfig& config) {
    const fs::path path = get_app_config_directory() / "graphics.json";
    try {
        std::ofstream out(path, std::ios::trunc);
        out << graphics_to_json(config).dump(4) << '\n';
        return out.good();
    } catch (const std::exception& e) {
        std::fprintf(stderr, "[beetle-adventure-racing-recomp] failed to write \"%s\": %s\n", path.string().c_str(), e.what());
        return false;
    }
}

bool save_current_graphics() {
    return save_graphics(ultramodern::renderer::get_graphics_config());
}

} // namespace bar::config
