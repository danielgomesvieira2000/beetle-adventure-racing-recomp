// src/game/config.hpp — host-owned settings persistence for beetle-adventure-racing-recomp.
//
// ultramodern owns the live GraphicsConfig (get/set_graphics_config) but never touches disk —
// loading and saving the user's settings is the port's job. This module:
//   * resolves a per-user config/data directory (so saves/settings don't litter the cwd),
//   * loads graphics settings from graphics.json (or sane defaults) and pushes them into the
//     runtime via set_graphics_config() — this is what turns on RT64 frame interpolation
//     (high refresh rate at identical game speed; see docs/SETTINGS_MENU_AND_HIGH_FPS.md),
//   * writes the current settings back out.
//
// The eventual settings menu (src/ui) edits the same GraphicsConfig and calls save_graphics().

#ifndef BAR_GAME_CONFIG_HPP
#define BAR_GAME_CONFIG_HPP

#include <filesystem>

#include "ultramodern/config.hpp"   // ultramodern::renderer::GraphicsConfig

namespace bar::config {

// Per-user config/data directory, created if missing. Resolution order:
//   1. If a file named "portable.txt" sits next to the executable -> the executable's directory
//      (portable install; settings + saves stay with the app).
//   2. Windows:  %LOCALAPPDATA%\beetle-adventure-racing-recomp\
//      Linux:    $XDG_CONFIG_HOME/beetle-adventure-racing-recomp  (else ~/.config/beetle-adventure-racing-recomp)
//   3. Fallback: the current working directory.
// The result is cached after the first call.
const std::filesystem::path& get_app_config_directory();

// Defaults used when graphics.json is absent or unreadable. High refresh-rate interpolation is
// ON by default (rr_option = Display = match the monitor); every other field mirrors the
// renderer's previous built-in behavior so enabling persistence changes nothing else.
ultramodern::renderer::GraphicsConfig default_graphics_config();

// Load graphics.json from get_app_config_directory() (falling back to defaults on any error)
// and apply it with ultramodern::renderer::set_graphics_config(). Call once at startup, before
// recomp::start(). Returns the config that was applied.
ultramodern::renderer::GraphicsConfig load_and_apply_graphics();

// Persist a GraphicsConfig to graphics.json in get_app_config_directory(). Returns false on I/O
// error. Pass the value you intend to persist (typically the one the menu just edited).
bool save_graphics(const ultramodern::renderer::GraphicsConfig& config);

// Convenience: persist whatever ultramodern currently holds (get_graphics_config()).
bool save_current_graphics();

} // namespace bar::config

#endif // BAR_GAME_CONFIG_HPP
