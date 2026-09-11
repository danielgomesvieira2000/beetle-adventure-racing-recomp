# Recomp Frontend

RecompFrontend is a library for use with N64: Recompiled projects that provides input handling (recompinput) and a menu API (recompui).

## recompinput

recompinput is a module that uses [SDL2](https://github.com/libsdl-org/SDL/tree/SDL2) to implement controller, keyboard, and mouse input handling for N64: Recompiled projects. It integrates with recompui, which uses it to allow the previously mentioned input devices to navigate any menus built using the library.

The recompinput module also handles controller mapping, multiplayer device binding, and multiplayer controller profiles. Controller profiles and player device selection for multiplayer use device GUIDs to restore settings for controllers between play sessions.

## recompui

recompui is a comprehensive UI API built on [RmlUi](https://github.com/mikke89/RmlUi). It provides basic UI elements, such as buttons and labels, as well as high-level components like the config menus, mod menu, multiplayer menu, and more.

Rendering is performed by interfacing with [RT64](https://github.com/rt64/rt64) to build [plume](https://github.com/renderbag/plume) command lists for the UI elements.

The recompui library also provides an API for N64: Recompiled mods to build their own UI for in-game use. The API definition can be found in the `recompui.h` header in a given project's mod template.
