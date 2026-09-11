//
// RT64 -- Beetle Adventure Racing HUD anchoring and the port's HUD inspector
//

#pragma once

#include <cstdint>

#include "common/rt64_common.h"

namespace RT64 {
    // BAR authors its HUD against a fixed 320x240 screen, so in a widened frame RT64's default
    // placement (scale the element about the frame's centre by the inverse aspect scale) leaves the
    // whole HUD huddled in the middle 4:3 region with empty picture either side of it. Anchoring
    // moves each element to the edge it belongs to instead, which is what the extended GBI's
    // per-element origins exist for -- BAR emits none, so we classify its draws here and supply them.
    //
    // Two things decide an element's class, in this order:
    //
    //   1. A **tag**, looked up by the element's identity through the port (the inspector's live
    //      override first, then hud.json). Tags apply EVERYWHERE, in menus as well as races.
    //   2. The **positional heuristic** below, which runs only during a race. It is measured by
    //      position (see docs/RACING_DRAW_MAP.md): everything in the top band is the speedometer
    //      (left) and the timer and lap counter (right); everything in the bottom band is the course
    //      map and the position counter (right); the middle band is where the centred race messages
    //      ("GO!") are drawn and is deliberately left alone. It is racing-only because BAR's menu
    //      screens are authored as whole 4:3 compositions and look broken pulled apart -- so a menu
    //      element moves only when someone has explicitly tagged it.
    namespace BarHud {
        // What an element can be told to do. The values are the wire format: they are what the
        // port's dropdown indexes and what hud.json's list names map to, so do not renumber them.
        enum class Class : int {
            Center  = 0,  // Leave RT64's default centring alone -- stay in the 4:3 box.
            Left    = 1,  // Hold this element's distance from the left edge of the widened frame.
            Right   = 2,  // Hold its distance from the right edge.
            Stretch = 3,  // Do not squeeze it to 4:3 (G_EX_ASPECT_STRETCH). For backdrops and wipes.
            Spill   = 4,  // Placement untouched; only lift the 4:3 scissor so it may run past the
                          // old frame's edge. The one class that is about clipping, not placement.
            Cover   = 5,  // ORTHOGRAPHIC LAYERS ONLY. Stretch, and then magnify the layer's content
                          // about its centre by exactly enough to put BAR's overscan-safe rectangle
                          // on the frame's edges. For a layer whose geometry is authored inset for a
                          // CRT and so cannot reach the edges however the layer itself is scaled.
        };

        // Set by the app once per controller poll: true only while the game is in its racing state.
        void setRacing(bool racing);
        bool racing();

        // The app's game-state value, shown in the inspector's header so that a list of elements can
        // be tied to the screen it came from. BAR keeps it at gGameSettings + 0xA4.
        void setGameState(uint32_t state);
        uint32_t gameState();

        // Whether the game's own pause menu is up (gGameSettings + 0x86). Pushed in by the app from
        // the SI poll, like the racing flag. It matters because a paused race is still a race as far
        // as currentGameState is concerned, and the two screens want opposite things from the
        // orthographic layer -- see orthoViewportOrigin.
        void setPaused(bool paused);
        bool paused();

        // Where a 2D rectangle belongs on the widened frame, and -- as a side effect -- the point at
        // which the element is published to the port's inspector.
        //
        // `rect` is the draw's own rectangle in RDP quarter-pixel coordinates and `screenWidth` the
        // colour image's width in game pixels (320 for BAR); `fillCycle` says whether the draw is a
        // fill-cycle rectangle.
        //
        // The remaining four build the element's IDENTITY, which is what a tag names -- and which of
        // them is used depends on the draw, because a stale value is worse than no value:
        //
        //   textured        "tex:<G_SETTIMG address>", with the display list as the second identity.
        //   fill cycle      "fill:<fill colour>", which is far more specific than the display list
        //                   and stable between frames.
        //   untextured      "dl:<display list address>". NOT the texture address: an untextured draw
        //                   carries whatever G_SETTIMG last set, which belongs to some other element
        //                   entirely. Tagging by it moved a menu's text when the thing being tagged
        //                   was the black overlay drawn over it.
        Class classifyRect(const FixedRect &rect, int32_t screenWidth, bool fillCycle,
            uint32_t textureAddress, uint32_t displayListAddress, bool textureOn, uint32_t fillColor);

        // The extended-GBI origin that realises a class, for the left and right edge of an element.
        // Center, Stretch and Spill all want G_EX_ORIGIN_NONE: neither of the last two moves an
        // element, they change how wide it may be drawn and how far it may run.
        uint16_t leftOriginFor(Class cls);
        uint16_t rightOriginFor(Class cls);

        // What an ORTHOGRAPHIC PROJECTION should do, decided per projection.
        //
        // EVERY projection in the frame is offered, not only the orthographic ones -- a projection is
        // the only handle there is on anything drawn as geometry, since individual triangles are not
        // hooked. Only orthographic layers get a default; everything else is Center until tagged, so
        // listing them changes nothing on its own.
        //
        // BAR draws some of its 2D as geometry under an orthographic projection rather than as
        // texture rectangles, and those layers never appear in the rectangle list at all -- they go
        // through the projection processor instead. Two of them want opposite things:
        //
        //   * the speedometer's needle lives in the dial at the left of the screen, and has to
        //     follow it there or the two come apart. That is Class::Left.
        //   * the pause screen's darkened backdrop has to cover the whole widened frame. That is
        //     Class::Stretch, which means "do not compensate the projection's aspect ratio", so the
        //     layer's own 320-wide space is mapped across the full frame.
        //
        // They cannot be told apart by when they are drawn -- a paused race is still a race -- so
        // they are told apart by WHICH projection they are. `projectionIndex` is the same number
        // RT64's own debugger shows in its Game editor ("Orthographic #4"), and the identity a tag
        // names is "ortho:<n>", so what is seen there and what is tagged here are the same thing.
        //
        // Untagged, the answer is the old rule: Left while racing, Center otherwise. BAR_HUD_ORTHO=0
        // turns that default off on its own, to tell the rectangle and projection halves apart on
        // screen; it does not disable tags.
        //
        // `publish` says whether to hand this projection to the inspector. The same question is
        // asked from two places -- the projection processor, which transforms the layer, and the
        // framebuffer renderer, which places its viewport -- and they MUST get the same answer or
        // the layer is placed by one rule and transformed by the other. Only the first publishes.
        // The projection types the classifier distinguishes, matching RT64's Projection::Type and
        // the prefix each one's identity carries.
        enum class ProjKind : int {
            Perspective = 0,   // "persp:<n>" -- the 3D world
            Orthographic = 1,  // "ortho:<n>" -- 2D drawn as geometry
            Rectangle = 2,     // "rect:<n>"  -- the layer texture rectangles are drawn into
            Other = 3,         // "proj:<n>"
        };

        Class classifyProjection(ProjKind kind, uint32_t projectionIndex, const FixedRect &scissor,
            bool publish);

        // The magnification Class::Cover asks for, as a factor on the projection matrix's x and y.
        // Returns 1.0 for every other class. BAR draws into a rectangle inset for a television --
        // (22,17)-(296,223) of its 320x240 screen, the same numbers as the overscan mask -- so the
        // factors are 320/274 and 240/206, both about 1.167. BAR_HUD_COVER_INSET="l,t,r,b" overrides
        // the rectangle, because it is measured from one game and this class is not.
        void coverScale(Class cls, float *outX, float *outY);

        // Turns a class into the viewport origin the projection processor wants. Stretch and Center
        // both give G_EX_ORIGIN_NONE -- Stretch is expressed by suppressing the aspect-ratio
        // compensation instead, which is a separate lever at the same call site.
        uint16_t viewportOriginFor(Class cls);

        // End of a display list, which is where the list of elements the inspector shows is
        // published. Called from State::advanceWorkload.
        void endFrame();
    }
};

// ---- The port's side of the inspector ----------------------------------------------------------
//
// RT64 is a library and must keep building on its own, so none of this is a call into the port: the
// port sets these pointers at startup (src/main/bar_inspector.cpp) and they stay null otherwise.
// Every call site checks for null, so an unpatched or standalone RT64 behaves exactly as before.
extern "C" {
    // Draws the port's window inside RT64's inspector UI, with an ImGui frame already open. Called
    // once per frame from State::inspect(), which only runs in RT64's developer mode.
    extern void (*RT64_PortInspectorHook)();

    // One classified 2D element of the frame being built. Extents are in the game's own 320x240
    // pixels, before any widescreen arithmetic. `kind` is "tex", "fill", "untex" or "ortho" -- which
    // identity scheme this element got, shown in the panel so that two rows that look alike can be
    // told apart. An "ortho" row is a whole projection rather than a single draw, and arrives from
    // the projection processor rather than from the RDP.
    extern void (*RT64_BarHudNoteElement)(const char *identity, const char *secondIdentity,
        float minX, float maxX, float minY, float maxY, int givenClass, const char *kind);

    // End of the display list: the frame just noted is complete and may be shown.
    extern void (*RT64_BarHudEndFrame)();

    // The port's tag lookup: the inspector's live override first, then hud.json. Returns non-zero
    // and writes a BarHud::Class into *outClass when this identity is tagged. Installed even when
    // the inspector itself is off, so that a saved hud.json still applies.
    extern int (*RT64_BarHudTagLookup)(const char *identity, const char *secondIdentity, int *outClass);
}
