#pragma once

// The HUD inspector: point at an element, see what the port thinks it is, and change its class while
// the game is in front of you.
//
// BAR's widescreen 2D layer is decided by a classifier in the RT64 fork
// (lib/rt64/src/hle/rt64_bar_hud.cpp), and until this existed the only way to see what it decided was
// to run with BAR_HUD_TRACE=1 and match a log to a screenshot by eye. That is a bad instrument for
// the thing it measures: a menu transition lasts a fraction of a second, and an element's identity is
// not something a picture shows. Several confident conclusions drawn that way were about the wrong
// frame -- docs/KNOWN_ISSUES.md carries two of them.
//
// So the classifier publishes what it decided, every frame, and RT64's developer UI draws it: a list
// of the frame's elements with their identity, extent and class, each with a dropdown that overrides
// the class from the next frame. RT64 supplies the rest and already did before this existed -- it can
// pause the game and keep the paused frame interactive, and right-clicking a pixel lists the draw
// calls under it.
//
// **F1 opens it**, in any build. See docs/HUD-INSPECTOR.md.
//
// Two threads meet here. The classifier runs on the thread that submits display lists and fills a
// frame under construction; the panel runs on the thread that owns the renderer's UI and reads the
// last frame that was completed. They swap under a lock, once per display list.

#include <cstdint>

namespace bar::inspector {

// The classes the classifier can give an element, in the order the panel offers them. Must match
// RT64::BarHud::Class in lib/rt64/src/hle/rt64_bar_hud.h -- the value is the wire format shared by
// the dropdown, the hook and hud.json.
enum Class : int { kCenter = 0, kLeft = 1, kRight = 2, kStretch = 3, kSpill = 4 };

// Whether the panel is running. hud.json tags are applied either way; this only governs the window
// and the per-frame element list.
bool enabled();

// Called once at startup, before the renderer is created. Reads BAR_INSPECTOR and loads hud.json.
void init();

// Installs the hooks into the RT64 fork. Safe to call when disabled -- the tag lookup is installed
// regardless, so a saved hud.json still applies with the panel off.
void install();

} // namespace bar::inspector
