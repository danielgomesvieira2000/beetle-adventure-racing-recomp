# CMakeLists.txt — beetle-adventure-racing-recomp (static-recomp PC port of Beetle Adventure Racing)
#
# Adapted from Zelda64Recomp/Zelda64Recomp @ dev (CMakeLists.txt). Pinned deps in this repo:
#   lib/N64ModernRuntime @ main (recomp::start(Configuration) — see src/main/main.cpp)
#   lib/rt64 @ main, lib/RecompFrontend @ main, lib/N64Recomp @ mod-tool-release
#
# !!! UNTESTED — not yet configured/built (no local toolchain). Treat as a first draft.
#
# Prereqs before this will configure/build:
#   1) scripts/setup.*  -> `git submodule update --init --recursive` (populates the NESTED
#      submodules: N64ModernRuntime/N64Recomp, rt64/contrib, etc.) and builds N64Recomp.
#   2) ./N64Recomp beetle-adventure-racing-recomp.toml  -> populates RecompiledFuncs/*.c (this file globs them).
#   3) A C++20 toolchain: Windows = VS2022 + clang-cl + Ninja (RT64 -> D3D12);
#      Linux = clang + ninja + SDL2/GTK (RT64 -> Vulkan). See BUILDING.md.

cmake_minimum_required(VERSION 3.20)
project(beetle-adventure-racing-recomp LANGUAGES C CXX)

set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# The frontend is RecompFrontend (recompui + recompinput) — the launcher, settings, controls and
# input layer shared by the wiseguy-style recomps. It is the ONLY UI in this project: the fork's
# bespoke menu (src/ui) and its top-level RmlUi/lunasvg/Freetype submodules have been removed, so
# RmlUi now exists solely as RecompFrontend's own vendored dependency, nested inside lib/RecompFrontend.
#
# Off only for a deliberately UI-less build (headless testing, or bisecting a renderer problem).
option(BEETLE_ENABLE_FRONTEND "Build the RecompFrontend launcher/menus (recompui + recompinput)" ON)


if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND CMAKE_CXX_SIMULATE_ID STREQUAL "MSVC")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-everything /W4")
endif()
if (WIN32)
    # Avoid the constexpr mutex ctor so we don't require a fresh vcredist (matches upstream).
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_DISABLE_CONSTEXPR_MUTEX_CONSTRUCTOR")
endif()
if (CMAKE_VERSION VERSION_GREATER_EQUAL "3.24.0")
    cmake_policy(SET CMP0135 NEW)
endif()

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# ---- RT64 first: defines the `rt64` target + build_*_shader() helpers ----
set(RT64_STATIC TRUE)
set(RT64_SDL_WINDOW_VULKAN TRUE)
add_compile_definitions(HLSL_CPU)
add_subdirectory(${CMAKE_SOURCE_DIR}/lib/rt64 ${CMAKE_BINARY_DIR}/rt64)
# RT64 vendors SDL2 2.26.3, whose SDL_endian.h manually defines _m_prefetch (an old Clang-11
# workaround). clang 17+ provides it as a builtin -> "definition of builtin function" error.
# Defining SDL's own guard macro makes it skip that definition and use the compiler builtin.
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    target_compile_definitions(rt64 PRIVATE __PRFCHWINTRIN_H)
endif()

# NOTE: this project deliberately does NOT define Freetype::Freetype. RecompFrontend vendors its own
# freetype-windows-binaries and a FindFreetype.cmake that creates that imported target itself, and
# CMake refuses to create an imported target twice.

# ---- N64ModernRuntime: provides the `librecomp` and `ultramodern` targets ----
add_subdirectory(${CMAKE_SOURCE_DIR}/lib/N64ModernRuntime)

# ---- RecompFrontend: recompui (launcher/settings/mods, built on RmlUi) + recompinput (SDL2 input,
# controller profiles). It consumes N64ModernRuntime and RT64 by PATH rather than by target, so it
# reuses the very same runtime and renderer this port already builds. Must come after
# N64ModernRuntime, which provides the `miniz` target recompui links. ----
if (BEETLE_ENABLE_FRONTEND)
    # recompui/recompinput include SDL2 headers directly and expect either sdl2_SOURCE_DIR (the
    # Windows FetchContent layout) or SDL2_INCLUDE_DIRS (a system package) to be set ALREADY — their
    # own CMake notes this as a standalone-build limitation. Our SDL2 acquisition lives further down
    # beside the beetle-adventure-racing-recomp target, which is too late: the frontend would configure with an empty
    # include path and fail on `#include "SDL.h"`. Acquire it here first. Both calls are idempotent,
    # so the later block simply reuses what this populates.
    if (WIN32)
        include(FetchContent)
        set(SDL2_VERSION "2.30.3")
        FetchContent_Declare(sdl2
            URL https://github.com/libsdl-org/SDL/releases/download/release-${SDL2_VERSION}/SDL2-devel-${SDL2_VERSION}-VC.zip)
        FetchContent_MakeAvailable(sdl2)
    else()
        find_package(SDL2 REQUIRED)
    endif()

    # recompui compiles its RmlUi interface shaders with RT64's build_vertex_shader/build_pixel_shader
    # macros, which read these DXC_* variables from the enclosing scope — and RT64 does not export
    # them to the parent. Without them DXC emits nothing, and the follow-up bin2c step fails with
    # "Failed to open file ... InterfaceVS.hlsl.dxil! (Or it's empty)". Set them BEFORE the
    # add_subdirectory so the frontend's scope inherits them. Entry points match ours (VSMain/PSMain).
    set(DXC_COMMON_OPTS "-I${CMAKE_SOURCE_DIR}/lib/RecompFrontend/recompui/shaders")
    set(DXC_DXIL_OPTS "-Wno-ignored-attributes")
    set(DXC_SPV_OPTS "-spirv" "-fspv-target-env=vulkan1.0" "-fvk-use-dx-layout")
    set(DXC_PS_OPTS "${DXC_COMMON_OPTS}" "-E" "PSMain" "-T" "ps_6_0")
    set(DXC_VS_OPTS "${DXC_COMMON_OPTS}" "-E" "VSMain" "-T" "vs_6_0" "-fvk-invert-y")
    if (WIN32)
        set(DXC "${CMAKE_SOURCE_DIR}/lib/rt64/src/contrib/dxc/bin/x64/dxc.exe")
    elseif (CMAKE_SYSTEM_NAME MATCHES "Linux")
        set(DXC "LD_LIBRARY_PATH=${CMAKE_SOURCE_DIR}/lib/rt64/src/contrib/dxc/lib/x64" "${CMAKE_SOURCE_DIR}/lib/rt64/src/contrib/dxc/bin/x64/dxc-linux")
    endif()

    set(RECOMP_FRONTEND_N64MODERNRUNTIME_PATH ${CMAKE_SOURCE_DIR}/lib/N64ModernRuntime CACHE PATH "" FORCE)
    set(RECOMP_FRONTEND_RT64_PATH             ${CMAKE_SOURCE_DIR}/lib/rt64             CACHE PATH "" FORCE)
    add_subdirectory(${CMAKE_SOURCE_DIR}/lib/RecompFrontend)

    # Same SDL_endian.h / _m_prefetch clash the rt64 target is guarded against above, reached here
    # through the FetchContent SDL2 instead of RT64's vendored copy: SDL defines the builtin manually
    # as an old Clang-11 workaround, and clang 17+ rejects redefining it. Setting SDL's own guard
    # macro makes it defer to the compiler builtin.
    if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        target_compile_definitions(recompui    PRIVATE __PRFCHWINTRIN_H)
        target_compile_definitions(recompinput PRIVATE __PRFCHWINTRIN_H)
    endif()
endif()

# Shared include set for the generated C and the host.
set(RECOMP_INCLUDES
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/lib/N64ModernRuntime/ultramodern/include
    ${CMAKE_SOURCE_DIR}/lib/N64ModernRuntime/librecomp/include
    # Populated by recursive submodule init (N64ModernRuntime's nested N64Recomp):
    ${CMAKE_SOURCE_DIR}/lib/N64ModernRuntime/N64Recomp/include
)

# ---------------------------------------------------------------------------
# RecompiledFuncs — N64Recomp output. Run `./N64Recomp beetle-adventure-racing-recomp.toml` FIRST,
# or this target will have no sources and CMake will error.
# ---------------------------------------------------------------------------
add_library(RecompiledFuncs STATIC)
target_compile_options(RecompiledFuncs PRIVATE
    -fno-strict-aliasing                 # MANDATORY for recompiled C (it puns memory constantly)
    -Wno-unused-variable
    -Wno-implicit-function-declaration
)
target_include_directories(RecompiledFuncs PRIVATE ${RECOMP_INCLUDES})
file(GLOB FUNC_C_SOURCES   CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/RecompiledFuncs/*.c)
file(GLOB FUNC_CXX_SOURCES CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/RecompiledFuncs/*.cpp)
target_sources(RecompiledFuncs PRIVATE ${FUNC_C_SOURCES} ${FUNC_CXX_SOURCES})

# ---------------------------------------------------------------------------
# PatchesLib — the recompiled C-to-MIPS patches that override or hook game functions.
#
# Produced by:  scripts/build-patches.sh   (patches/*.c -> patches/patches.elf, built in WSL
#                                           because the Windows LLVM has no MIPS backend)
#               ./N64Recomp patches.toml   (patches.elf -> RecompiledPatches/patches.c)
#
# Optional by design: the tree builds and runs without any patches, so this is wired up only when
# the recompiled output is actually present. That keeps a fresh clone building before anyone has
# installed a MIPS toolchain.
# ---------------------------------------------------------------------------
#
# NOT a static library. Patch functions carry the same C names as the recompiled originals and the
# linker is what makes the patch win. Under clang-cl RECOMP_FUNC expands to a WEAK definition, for the
# originals and the patches alike, and with two weak definitions the linker keeps whichever it meets
# first. A static PatchesLib only ever contributes members for symbols that are still undefined when
# it is scanned; by then RecompiledFuncs' object holding the original has usually been pulled in for
# some other function, so the original won and the patch was silently ignored -- confirmed with the
# uvGfxClipRect viewport patch, which linked cleanly and changed nothing. Compiling patches.c
# directly into the executable puts its definitions in an object file, which the linker sees before
# it opens any archive, so the patch is the first definition and wins. The wiring is below the
# add_executable(), where the target exists.
if (EXISTS ${CMAKE_SOURCE_DIR}/RecompiledPatches/patches.c)
    set(BEETLE_PATCH_SOURCE ${CMAKE_SOURCE_DIR}/RecompiledPatches/patches.c)
    message(STATUS "beetle-adventure-racing-recomp: compiling recompiled patches into the executable (RecompiledPatches/patches.c)")
else()
    set(BEETLE_PATCH_SOURCE "")
    message(STATUS "beetle-adventure-racing-recomp: no RecompiledPatches/patches.c — building without patches")
endif()

# ---------------------------------------------------------------------------
# Main executable
# ---------------------------------------------------------------------------
add_executable(beetle-adventure-racing-recomp
    ${CMAKE_SOURCE_DIR}/src/main/main.cpp
    ${CMAKE_SOURCE_DIR}/src/main/os_unimpl_stubs.cpp     # no-op stubs for ignored OS funcs BAR calls
    ${CMAKE_SOURCE_DIR}/src/main/register_overlays.cpp   # wires recomp_overlays.inl into librecomp
    ${CMAKE_SOURCE_DIR}/src/main/overlay_bridge.cpp      # registers uv modules as overlays on load
    ${CMAKE_SOURCE_DIR}/src/main/hw_stubs.cpp            # native stubs for raw RCP-register libultra funcs
    ${CMAKE_SOURCE_DIR}/src/main/rt64_render_context.cpp # RT64-backed RendererContext subclass
    ${CMAKE_SOURCE_DIR}/src/main/bar_config.cpp          # feature/settings flags (fast-boot, etc.)
    ${CMAKE_SOURCE_DIR}/src/main/bar_frustum.cpp         # draw distance + the frustum the game culls against
    ${CMAKE_SOURCE_DIR}/src/main/bar_cheats.cpp          # BAR-specific cheats (host-side RDRAM pokes)
    ${CMAKE_SOURCE_DIR}/src/main/bar_preempt.cpp         # cooperative audio-thread preemption (no-underrun)
    ${CMAKE_SOURCE_DIR}/src/main/bar_crash.cpp           # symbolized crash backtrace (debug aid)
    ${CMAKE_SOURCE_DIR}/src/main/bar_input.cpp           # live 4-port runtime input path (device/keymap/rumble/pak)
    ${CMAKE_SOURCE_DIR}/src/game/config.cpp              # host-owned settings persistence + high-FPS enable
    ${CMAKE_SOURCE_DIR}/src/game/input_config.cpp        # 4-port controller settings model + input.json persistence
    ${CMAKE_SOURCE_DIR}/rsp/aspMain.cpp                  # RSPRecomp output: BAR audio ucode (M_AUDTASK)
    # TODO(BAR): add as authored (mirroring Zelda's src/main + src/game):
    #   src/main/register_patches.cpp
)
target_include_directories(beetle-adventure-racing-recomp PRIVATE
    ${RECOMP_INCLUDES}
    ${CMAKE_SOURCE_DIR}/src                         # "game/config.hpp", etc.
    ${CMAKE_SOURCE_DIR}/RecompiledFuncs            # recomp_overlays.inl + funcs.h
    ${CMAKE_SOURCE_DIR}/lib/rt64/src
    # rt64_render_context.cpp includes RT64's internal headers, which pull in vendored contrib
    # headers (hlslpp, plume, ...). Inherit RT64's full (private) include set so they resolve.
    $<TARGET_PROPERTY:rt64,INCLUDE_DIRECTORIES>
    ${CMAKE_CURRENT_BINARY_DIR}
)

if (CMAKE_SIZEOF_VOID_P EQUAL 8 AND CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64|amd64|AMD64")
    target_compile_options(beetle-adventure-racing-recomp PRIVATE -march=nehalem -fno-strict-aliasing -fms-extensions)
else()
    target_compile_options(beetle-adventure-racing-recomp PRIVATE -fno-strict-aliasing -fms-extensions)
endif()
if (MSVC)
    target_link_options(beetle-adventure-racing-recomp PRIVATE /OPT:NOICF)   # don't fold identical recompiled funcs
endif()

# ---- Windows: RT64 picks D3D12 by default; SDL2 via FetchContent ----
if (WIN32)
    include(FetchContent)
    set(SDL2_VERSION "2.30.3")
    FetchContent_Declare(sdl2
        URL https://github.com/libsdl-org/SDL/releases/download/release-${SDL2_VERSION}/SDL2-devel-${SDL2_VERSION}-VC.zip)
    FetchContent_MakeAvailable(sdl2)
    target_include_directories(beetle-adventure-racing-recomp PRIVATE ${sdl2_SOURCE_DIR}/include)
    target_link_directories(beetle-adventure-racing-recomp PRIVATE ${sdl2_SOURCE_DIR}/lib/x64)
    add_compile_definitions(NOMINMAX)
    add_custom_command(TARGET beetle-adventure-racing-recomp POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${sdl2_SOURCE_DIR}/lib/x64/SDL2.dll"
            "${CMAKE_SOURCE_DIR}/lib/rt64/src/contrib/dxc/bin/x64/dxil.dll"
            "${CMAKE_SOURCE_DIR}/lib/rt64/src/contrib/dxc/bin/x64/dxcompiler.dll"
            $<TARGET_FILE_DIR:beetle-adventure-racing-recomp>)
    set_target_properties(beetle-adventure-racing-recomp PROPERTIES
        LINK_FLAGS_DEBUG          "/SUBSYSTEM:CONSOLE"
        LINK_FLAGS_RELEASE        "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
    target_link_libraries(beetle-adventure-racing-recomp PRIVATE SDL2 Winmm.lib Dbghelp.lib)   # Winmm: timeBeginPeriod; Dbghelp: crash backtrace
    # TODO(BAR): app icon -> target_sources(beetle-adventure-racing-recomp PRIVATE icons/app.rc)
endif()

# ---- Linux: RT64 -> Vulkan ----
if (CMAKE_SYSTEM_NAME MATCHES "Linux")
    find_package(SDL2 REQUIRED)
    add_compile_definitions("RT64_SDL_WINDOW_VULKAN")
    target_include_directories(beetle-adventure-racing-recomp PRIVATE ${SDL2_INCLUDE_DIRS})
    set(THREADS_PREFER_PTHREAD_FLAG TRUE)
    find_package(Threads REQUIRED)
    target_link_libraries(beetle-adventure-racing-recomp PRIVATE
        "-latomic -static-libstdc++" ${CMAKE_DL_LIBS} Threads::Threads SDL2::SDL2)
endif()

# ---- Link the runtime + generated code ----
target_link_libraries(beetle-adventure-racing-recomp PRIVATE
    RecompiledFuncs
    librecomp
    ultramodern
    rt64
)

# Patches are compiled INTO the executable so their definitions precede the recompiled originals
# in RecompiledFuncs (see the PatchesLib note above for why a static library cannot do this).
if (BEETLE_PATCH_SOURCE)
    target_sources(beetle-adventure-racing-recomp PRIVATE ${BEETLE_PATCH_SOURCE})
    set_source_files_properties(${BEETLE_PATCH_SOURCE} PROPERTIES
        COMPILE_OPTIONS "-fno-strict-aliasing;-Wno-unused-variable;-Wno-implicit-function-declaration"
        INCLUDE_DIRECTORIES "${RECOMP_INCLUDES};${CMAKE_SOURCE_DIR}/RecompiledPatches")
endif()

# ---- RecompFrontend launcher/menus + input ----
if (BEETLE_ENABLE_FRONTEND)
    target_sources(beetle-adventure-racing-recomp PRIVATE ${CMAKE_SOURCE_DIR}/src/frontend/bar_frontend.cpp)
    target_link_libraries(beetle-adventure-racing-recomp PRIVATE recompui recompinput)
    target_compile_definitions(beetle-adventure-racing-recomp PRIVATE BEETLE_ENABLE_FRONTEND)
    # recompui.h pulls SDL.h and RmlUi headers; RmlUi arrives transitively via recompui's PUBLIC
    # link, but SDL2 comes from our own FetchContent above.
    if (WIN32)
        target_include_directories(beetle-adventure-racing-recomp PRIVATE ${sdl2_SOURCE_DIR}/include)
    endif()

    # Ship the UI assets recompui resolves as <exe dir>/assets/<name>: the REQUIRED primary font and
    # the project stylesheet. Without them recompui throws at menu-creation time.
    # Copy the whole tree so icons/ (and anything added later) comes along: recompui resolves both
    # "recomp.rcss" and "icons/X.svg" through the same <exe dir>/assets root.
    add_custom_command(TARGET beetle-adventure-racing-recomp POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                "${CMAKE_SOURCE_DIR}/assets/ui"
                "$<TARGET_FILE_DIR:beetle-adventure-racing-recomp>/assets"
        COMMENT "Staging RecompFrontend assets (stylesheet, primary font, icons)")
endif()
# ---------------------------------------------------------------------------
# Tier-2 settings/launcher menu (src/ui) — RmlUi rendered through RT64.
# ---------------------------------------------------------------------------

# ---- Bring-up crash diagnosis: embed debug info (/Z7) in the boot-path targets + emit a PDB.
# RT64 is deliberately left at Release (no option change -> not rebuilt) since the early-boot
# crash path is in librecomp / ultramodern / the recompiled code, not the renderer. Flip
# BEETLE_DEBUGINFO OFF once bring-up settles. ----
option(BEETLE_DEBUGINFO "Embed debug info in the boot-path targets for crash diagnosis" ON)
if (BEETLE_DEBUGINFO AND MSVC)
    foreach(dbgtgt RecompiledFuncs librecomp ultramodern beetle-adventure-racing-recomp)
        if (TARGET ${dbgtgt})
            target_compile_options(${dbgtgt} PRIVATE /Z7)
        endif()
    endforeach()
    target_link_options(beetle-adventure-racing-recomp PRIVATE /DEBUG)
endif()

set_property(TARGET beetle-adventure-racing-recomp PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")
