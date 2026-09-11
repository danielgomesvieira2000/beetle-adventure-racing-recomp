//
// RT64 -- Beetle Adventure Racing HUD anchoring and the port's HUD inspector
//

#include "rt64_bar_hud.h"

#include <atomic>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <set>

#include "../include/rt64_extended_gbi.h"

// The port installs these; see the header. Null in a standalone RT64, which is why every call site
// checks.
extern "C" {
    void (*RT64_PortInspectorHook)() = nullptr;
    void (*RT64_BarHudNoteElement)(const char *identity, const char *secondIdentity,
        float minX, float maxX, float minY, float maxY, int givenClass, const char *kind) = nullptr;
    void (*RT64_BarHudEndFrame)() = nullptr;
    int (*RT64_BarHudTagLookup)(const char *identity, const char *secondIdentity, int *outClass) = nullptr;
}

namespace RT64 {
    namespace BarHud {
        static std::atomic_bool sRacing = { false };
        static std::atomic_uint32_t sGameState = { 0 };
        static std::atomic_bool sPaused = { false };

        // Env switches, read once. BAR_HUD_ANCHOR=0 disables the positional heuristic entirely (the
        // HUD goes back to sitting in the middle of the frame); BAR_HUD_ORTHO=0 keeps the rectangles
        // anchored but leaves the orthographic layer -- the speedometer needle -- centred, which is
        // how the two halves are told apart on screen. BAR_HUD_TRACE=1 prints each anchored
        // rectangle once.
        //
        // BAR_HUD_ANCHOR=0 does NOT disable tags: an element someone has explicitly tagged in
        // hud.json is still placed where the tag says. That is the point of the switch -- it isolates
        // the heuristic, so the two can be told apart.
        static bool envFlag(const char *name, bool fallback) {
            const char *value = std::getenv(name);
            if (value == nullptr) {
                return fallback;
            }

            return (value[0] != '0');
        }

        static bool anchorEnabled() {
            static const bool enabled = envFlag("BAR_HUD_ANCHOR", true);
            return enabled;
        }

        static bool orthoEnabled() {
            static const bool enabled = envFlag("BAR_HUD_ORTHO", true);
            return enabled;
        }

        static bool traceEnabled() {
            static const bool enabled = envFlag("BAR_HUD_TRACE", false);
            return enabled;
        }

        void setRacing(bool racing) {
            if (traceEnabled()) {
                static int last = -1;
                if (int(racing) != last) {
                    last = int(racing);
                    fprintf(stdout, "[hud] racing=%d\n", last);
                    fflush(stdout);
                }
            }

            sRacing.store(racing, std::memory_order_relaxed);
        }

        bool racing() {
            return anchorEnabled() && sRacing.load(std::memory_order_relaxed);
        }

        void setGameState(uint32_t state) {
            sGameState.store(state, std::memory_order_relaxed);
        }

        void setPaused(bool paused) {
            sPaused.store(paused, std::memory_order_relaxed);
        }

        bool paused() {
            return sPaused.load(std::memory_order_relaxed);
        }

        uint32_t gameState() {
            return sGameState.load(std::memory_order_relaxed);
        }

        // Identities promoted out of somebody's hud.json and into the build.
        //
        // This is the intended end of a tag's life. hud.json is where an element is IDENTIFIED, in
        // the inspector, against a running game; once the answer is known it belongs here, so that
        // every build carries it and nobody has to have the right file in their settings folder. A
        // hud.json still repeating what this table says is not wrong, but it is one more thing that
        // can disagree.
        //
        // Consulted AFTER the port's tag lookup, so a live override in the panel and a hand-written
        // hud.json both still win -- which is what makes it possible to check a promoted answer
        // against a different one without editing this array.
        struct BuiltinTag {
            const char *identity;
            Class cls;
        };

        static const BuiltinTag sBuiltinTags[] = {
            // Promoted 11 Sep 2026, from hud.json, in the order they were found. All three are
            // pinned to Center against the positional heuristic, which is the only thing a Center tag
            // can override -- Center is already the default everywhere else, so each of these is an
            // element a race was throwing to an edge that should have stayed where the game put it.
            //
            // WHAT each one is has not been written down yet. That is a gap, not a style choice: an
            // address with no name cannot be checked by anyone who was not there, and if one of them
            // ever stops matching there is nothing to search the game for. Name them.
            { "tex:0x003C6E20", Class::Center },
            { "tex:0x003C66A8", Class::Center },
            { "tex:0x003CEA48", Class::Center },
        };

        static bool builtinTag(const char *identity, const char *secondIdentity, Class *outClass) {
            for (const BuiltinTag &tag : sBuiltinTags) {
                if (((identity != nullptr) && (std::strcmp(tag.identity, identity) == 0)) ||
                    ((secondIdentity != nullptr) && (std::strcmp(tag.identity, secondIdentity) == 0)))
                {
                    *outClass = tag.cls;
                    return true;
                }
            }

            return false;
        }

        // The positional heuristic, unchanged from when anchoring was the only thing this file did.
        // It answers Left, Right or Center, and it is only ever consulted during a race.
        static Class classifyByPosition(const FixedRect &rect, int32_t screenWidth, bool fillCycle) {
            if (rect.isNull() || (screenWidth <= 0)) {
                return Class::Center;
            }

            // Every HUD element is a textured rectangle. The rectangles drawn in fill cycle are the
            // frame and depth clears and BAR's overscan mask -- backdrop layers that must keep
            // spanning the frame, not elements with a place on it.
            if (fillCycle) {
                return Class::Center;
            }

            // BAR renders 320x240 and its HUD is authored against exactly that, so the bands are
            // measured against the game's screen rather than against the draw's scissor: the scissor
            // is sometimes RT64's unbounded 2048x2048 one, against which every band test is
            // meaningless. Requiring the rectangle to lie inside the screen is what rejects the
            // full-frame backdrops, whose height alone already puts them outside both bands.
            const int32_t screenH = (screenWidth * 3) / 4;
            if ((rect.ulx < 0) || (rect.uly < 0) || (rect.lrx > (screenWidth << 2)) || (rect.lry > (screenH << 2))) {
                return Class::Center;
            }

            // The top and bottom bands hold the HUD; the middle band holds the centred race messages
            // and is left as RT64 places it. Measured element extents, in game pixels: speedometer
            // 21..53, timer 21..51, lap 51..70, messages 79..154, map 149..205, position 202..222.
            const int32_t topBand = (screenH * 33) / 100;      // 79
            const int32_t bottomBand = (screenH * 60) / 100;   // 144
            const bool inTopBand = (rect.lry <= (topBand << 2));
            const bool inBottomBand = (rect.uly >= (bottomBand << 2));
            if (!inTopBand && !inBottomBand) {
                return Class::Center;
            }

            // Within a band, the side an element belongs to is simply the side it is drawn on. The
            // thresholds leave a wide neutral middle so that a centred element appearing in a band --
            // a lap time, a "wrong way" warning -- is not thrown to an edge.
            const int32_t centreX = (rect.ulx + rect.lrx) / 2;
            const int32_t leftLimit = ((screenWidth * 40) / 100) << 2;    // 128
            const int32_t rightLimit = ((screenWidth * 60) / 100) << 2;   // 192
            if (centreX <= leftLimit) {
                return Class::Left;
            }
            else if (centreX >= rightLimit) {
                return Class::Right;
            }

            return Class::Center;
        }

        Class classifyRect(const FixedRect &rect, int32_t screenWidth, bool fillCycle,
            uint32_t textureAddress, uint32_t displayListAddress, bool textureOn, uint32_t fillColor)
        {
            const bool listing = (RT64_BarHudNoteElement != nullptr);
            const bool tagging = (RT64_BarHudTagLookup != nullptr);
            constexpr bool anyBuiltinTags = (sizeof(sBuiltinTags) / sizeof(sBuiltinTags[0])) > 0;

            // Nothing to do at all: no inspector listening, nothing tagged either way, and not in a
            // race. This is the shipped path in a session where the debug menu is never opened, so
            // it costs one branch per rectangle rather than two string formats -- but only while the
            // built-in table is empty, since a promoted tag has to be applied everywhere.
            if (!listing && !tagging && !anyBuiltinTags && !racing()) {
                return Class::Center;
            }

            // The element's identity, which is what a tag names and what the inspector's filter
            // matches on. Which scheme applies depends on the draw, and getting this wrong is not a
            // cosmetic problem: an untextured rectangle carries whatever G_SETTIMG last set, so
            // identifying it by texture address gives it the identity of some unrelated element.
            // That really happened -- a pause screen's black overlay and the menu text under it came
            // out with the same "tex:" address, and tagging the overlay stretched the text.
            char identity[24];
            char secondIdentity[24];
            const char *kind = "untex";
            if (fillCycle) {
                // The fill colour is far more specific than the display list, and stable between
                // frames: a backdrop keeps its colour where its display list address need not.
                kind = "fill";
                std::snprintf(identity, sizeof(identity), "fill:0x%08X", fillColor);
                std::snprintf(secondIdentity, sizeof(secondIdentity), "dl:0x%08X", displayListAddress);
            }
            else if (textureOn) {
                kind = "tex";
                std::snprintf(identity, sizeof(identity), "tex:0x%08X", textureAddress);
                std::snprintf(secondIdentity, sizeof(secondIdentity), "dl:0x%08X", displayListAddress);
            }
            else {
                // Untextured and not a fill: a shaded or blended rectangle, which is what a
                // translucent overlay is. The display list is all there is to go on, so it is the
                // identity rather than the afterthought -- and the texture address is deliberately
                // NOT offered as the second one, because it belongs to somebody else.
                std::snprintf(identity, sizeof(identity), "dl:0x%08X", displayListAddress);
                std::snprintf(secondIdentity, sizeof(secondIdentity), "untex");
            }

            // Tags first, and they apply everywhere -- that is the whole difference between a tag and
            // the heuristic. The heuristic then runs only in a race.
            Class cls = Class::Center;
            bool tagged = false;
            if (tagging) {
                int tagClass = 0;
                if (RT64_BarHudTagLookup(identity, secondIdentity, &tagClass) != 0) {
                    cls = Class(tagClass);
                    tagged = true;
                }
            }

            if (!tagged) {
                Class builtin = Class::Center;
                if (builtinTag(identity, secondIdentity, &builtin)) {
                    cls = builtin;
                    tagged = true;
                }
            }

            if (!tagged && racing()) {
                cls = classifyByPosition(rect, screenWidth, fillCycle);
            }

            if (listing) {
                RT64_BarHudNoteElement(identity, secondIdentity,
                    rect.ulx / 4.0f, rect.lrx / 4.0f, rect.uly / 4.0f, rect.lry / 4.0f,
                    int(cls), kind);
            }

            if ((cls != Class::Center) && traceEnabled()) {
                static std::set<uint64_t> seen;
                const uint64_t key =
                    (uint64_t(uint16_t(rect.ulx)) << 48) | (uint64_t(uint16_t(rect.uly)) << 32) |
                    (uint64_t(uint16_t(rect.lrx)) << 16) | uint64_t(uint16_t(rect.lry));
                if (seen.insert(key).second && (seen.size() <= 2000)) {
                    static const char *names[] = { "center", "left", "right", "stretch", "spill" };
                    fprintf(stdout, "[hud] %s px=(%.1f,%.1f)-(%.1f,%.1f) -> %s%s\n", identity,
                        rect.ulx / 4.0f, rect.uly / 4.0f, rect.lrx / 4.0f, rect.lry / 4.0f,
                        names[int(cls)], tagged ? " (tagged)" : "");
                    fflush(stdout);
                }
            }

            return cls;
        }

        void endFrame() {
            if (RT64_BarHudEndFrame != nullptr) {
                RT64_BarHudEndFrame();
            }
        }

        uint16_t leftOriginFor(Class cls) {
            switch (cls) {
            case Class::Left:
                return G_EX_ORIGIN_LEFT;
            case Class::Right:
                return G_EX_ORIGIN_RIGHT;
            case Class::Center:
            case Class::Stretch:
            case Class::Spill:
            default:
                // Stretch and Spill both leave the element where it is. Stretch changes how wide it
                // may be drawn and Spill how far it may run; neither is a translation, and giving
                // either an origin would move it as well, which is not what was asked for.
                return G_EX_ORIGIN_NONE;
            }
        }

        uint16_t rightOriginFor(Class cls) {
            // Both edges of an element take the same origin, so the element translates towards its
            // edge instead of being stretched towards it.
            return leftOriginFor(cls);
        }

        Class classifyProjection(ProjKind kind, uint32_t projectionIndex, const FixedRect &scissor,
            bool publish)
        {
            // The identity carries the projection's kind and the number RT64's own Game editor puts
            // on it, so that what is seen there and what is tagged here are the same thing.
            static const char *const kPrefixes[] = { "persp", "ortho", "rect", "proj" };
            const char *prefix = kPrefixes[int(kind) & 3];

            char identity[24];
            std::snprintf(identity, sizeof(identity), "%s:%u", prefix, unsigned(projectionIndex));

            // Precedence, the same as for rectangles: the panel's live override and hud.json
            // first (both answered by the port), then an identity promoted into the build, and only
            // then the default rule.
            //
            // Only orthographic layers have a default, and it is the old whole-layer behaviour: pin
            // them to the left edge during a race, because the needle is what that rule was written
            // for and it is still right for a race nobody has paused. Everything else is Center
            // until somebody tags it, so listing the other projections changes nothing by itself.
            Class cls = Class::Center;
            if ((kind == ProjKind::Orthographic) && racing() && orthoEnabled()) {
                cls = Class::Left;
            }

            bool tagged = false;
            if (RT64_BarHudTagLookup != nullptr) {
                int tagClass = 0;
                if (RT64_BarHudTagLookup(identity, "", &tagClass) != 0) {
                    cls = Class(tagClass);
                    tagged = true;
                }
            }

            if (!tagged) {
                Class builtin = Class::Center;
                if (builtinTag(identity, "", &builtin)) {
                    cls = builtin;
                }
            }

            if (publish && (RT64_BarHudNoteElement != nullptr)) {
                RT64_BarHudNoteElement(identity, "", scissor.ulx / 4.0f, scissor.lrx / 4.0f,
                    scissor.uly / 4.0f, scissor.lry / 4.0f, int(cls), prefix);
            }

            return cls;
        }

        void coverScale(Class cls, float *outX, float *outY) {
            *outX = 1.0f;
            *outY = 1.0f;
            if (cls != Class::Cover) {
                return;
            }

            // The rectangle BAR authors its full-screen 2D into. Measured with BAR_DBG_RECT during a
            // race as the complement of the overscan mask, and confirmed from the pause screen: with
            // the backdrop's projection merely stretched, its top edge still sat at about 9% of the
            // frame height, which is 21 of 240 -- the inset, not a failure of the stretch.
            static const struct Inset {
                float left = 22.0f, top = 17.0f, right = 296.0f, bottom = 223.0f;
            } inset = [] {
                Inset parsed;
                const char *e = std::getenv("BAR_HUD_COVER_INSET");
                if (e != nullptr) {
                    float l = 0.0f, t = 0.0f, r = 0.0f, b = 0.0f;
                    if (sscanf(e, "%f,%f,%f,%f", &l, &t, &r, &b) == 4) {
                        parsed.left = l;
                        parsed.top = t;
                        parsed.right = r;
                        parsed.bottom = b;
                    }
                }
                return parsed;
            }();

            const float width = inset.right - inset.left;
            const float height = inset.bottom - inset.top;
            if ((width > 0.0f) && (height > 0.0f)) {
                *outX = 320.0f / width;
                *outY = 240.0f / height;
            }
        }

        uint16_t viewportOriginFor(Class cls) {
            switch (cls) {
            case Class::Left:
                return G_EX_ORIGIN_LEFT;
            case Class::Right:
                return G_EX_ORIGIN_RIGHT;
            default:
                // Center keeps the projection processor's own decision; Stretch and Cover are
                // expressed by the aspect-ratio compensation and the magnification instead of by an
                // origin, so both want none of one. Spill has no meaning
                // for a whole projection -- a projection is not scissored by the 4:3 box the way an
                // individual rectangle is -- and is treated as Center.
                return G_EX_ORIGIN_NONE;
            }
        }
    }
};

// Bridge for the app: BAR's racing state and game state live in the game's RDRAM, which only the app
// side can read, so it pushes the answers in once per controller poll rather than RT64 guessing.
extern "C" void bar_rt64_set_hud_anchor(int racing) {
    RT64::BarHud::setRacing(racing != 0);
}

extern "C" void bar_rt64_set_game_state(unsigned int state) {
    RT64::BarHud::setGameState(state);
}

extern "C" void bar_rt64_set_hud_paused(int paused) {
    RT64::BarHud::setPaused(paused != 0);
}

extern "C" unsigned int bar_rt64_get_game_state() {
    return RT64::BarHud::gameState();
}
