#pragma once

// beetle-adventure-racing-recomp <-> RecompFrontend glue.
//
// RecompFrontend (recompui + recompinput) is the launcher/menu and input layer extracted from
// Zelda 64: Recompiled. It replaces the bespoke src/ui menu; see docs/PINNED_REVISIONS.md for the
// build constraints. Everything here is compiled only when -DBEETLE_ENABLE_FRONTEND=ON.

#include <memory>

#include "ultramodern/renderer_context.hpp"

// Forward-declared so main.cpp can hand events over without pulling in recompui.h, which drags in
// SDL and the whole of RmlUi.
union SDL_Event;

namespace bar::frontend {

// Pump SDL events through the frontend. This REPLACES the host's own SDL_PollEvent loop rather
// than supplementing it: recompinput::handle_events() polls SDL itself, and two pumps would race
// for the same queue and silently drop each other's events.
//
// Doing it this way is what makes the mouse work. Besides routing events to recompui, that function
// applies SDL_ShowCursor and SDL_SetRelativeMouseMode from recompui's cursor state — recompui only
// records a flag and never touches SDL — and also handles Alt+Enter/F11 fullscreen, controller
// binding capture, controller hotplug, file drops, player assignment and quit.
void pump_events();

// True while a menu actually owns input. Game input must be frozen for exactly this window and no
// longer: the legacy UI froze input whenever its overlay existed, which is why the keyboard did
// nothing at all in normal (non-headless) launches.
bool menu_capturing_input();

// Register program identity, fonts and the launcher layout with recompui. Must be called BEFORE
// recomp::start(), because recompui builds its menus during renderer bring-up.
void install();

// ---- The game's controller ports, resolved by recompinput -------------------------------------
//
// With the frontend on, recompinput owns input: it holds the per-device profiles the Controls tab
// edits, and profiles::get_n64_input is where a player's remapping is actually applied. A port that
// reads SDL itself -- which this one did, through bar::input -- silently ignores every rebinding the
// player has made, and never sees a pad at all, because recompinput::handle_events() is the only
// thing draining the SDL queue and the hotplug events never reach anyone else.
//
// These two are the whole bridge. main.cpp calls them from bar_poll_keyboard and from the
// connected-device callback; nothing else in the port includes a recompinput header.

// True when this N64 port has something driving it. Player one always does -- a keyboard is always
// attached -- and ports two to four exist once a second, third or fourth pad has been plugged in.
bool port_assigned(int port);

// This port's live N64 button mask, plus its analog stick in the N64's own +/-80 range. Returns 0
// and a centred stick for an unassigned port. Applies the player's own bindings.
uint16_t poll_port(int port, int8_t* stick_x, int8_t* stick_y);

// Ask this port's pad to rumble, or stop. recompinput owns the motor when the frontend is on: it
// ramps towards the level the General tab's Rumble Strength asks for, and pump_events() is what
// drives that ramp. Requesting it anywhere else means a slider at 30% still rumbles at full.
void set_port_rumble(int port, bool on);

// Render-context factory for ultramodern's renderer_callbacks.create_render_context.
//
// recompui ships its own RT64Context (it has to, since it draws the UI over the game), so the port
// uses that instead of src/main/rt64_render_context.cpp when the frontend is enabled. The runtime's
// callback is (rdram, window_handle, developer_mode) while recompui's factory also takes a
// presentation mode, so this adapts between them — and that extra parameter is exactly where BAR's
// requirement is expressed: the main-menu film-roll is a VI-origin pan that never redraws, so it
// only animates in RT64's Console presentation mode.
std::unique_ptr<ultramodern::renderer::RendererContext>
create_render_context(uint8_t* rdram, ultramodern::renderer::WindowHandle window_handle, bool developer_mode);

} // namespace bar::frontend
