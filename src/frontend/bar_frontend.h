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
