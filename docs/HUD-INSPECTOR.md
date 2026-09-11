# The HUD inspector

A window inside RT64's developer UI that lists every 2D element the port classified this frame,
outlines one on the screen when you hover it, and lets you change its widescreen class from a
dropdown while the game runs.

Why it exists: the previous instrument was `BAR_HUD_TRACE=1`, a log read afterwards and matched to a
screenshot by eye. That is a bad instrument for the thing it measures. A menu transition lasts a
fraction of a second, an element's identity is not something a picture shows, and two of the entries
in [`KNOWN_ISSUES.md`](KNOWN_ISSUES.md) were conclusions drawn that way about the wrong frame.

It is modelled directly on `wave-race-64-recomp`'s, which is this project's reference port; the
differences are noted under [Differences from the Wave Race version](#differences-from-the-wave-race-version).

---

## Using it

### Turning it on

**Press F1.** In any build, released or not, with nothing set beforehand. F1 again closes it.

F1 is RT64's own shortcut for its developer UI, and the port's window is drawn inside that UI, so
both appear together: RT64's **Game editor** on one side and **Beetle Adventure Racing HUD** on the
other. That pairing is the point — RT64's half answers "what did the renderer draw here" and the
port's half answers "which of the game's elements is that, and what class did we give it".

A debug menu that only exists in a build made for it is a debug menu nobody has when they need it:
the person looking at a misplaced HUD element is running the game they downloaded. Nothing is drawn
until F1 is pressed — RT64 creates its inspector on the keystroke, and `State::inspect()` returns
immediately while there is none — so leaving it available costs a null check per frame.

`BAR_INSPECTOR=0` turns the port's half off, for an A/B against the fork's own classification. RT64's
half stays on F1 regardless. **Tags in `hud.json` still apply with the panel off**: they are a saved
decision about how the game should look, not a debugging aid.

### The other keys

Developer mode arms three more of RT64's shortcuts for everyone, so they are part of the shipped
build too:

| Key | Does |
|---|---|
| **F1** | Opens and closes this menu |
| F2 | **Unbound.** RT64 uses it to toggle ray tracing for the session, with no menu entry saying so and nothing on screen to explain what changed. The fork removes the case from both key filters (`lib/rt64/src/hle/rt64_application.cpp`), so the key passes through to the game like any other. |
| F3 | Views RDRAM. Visibly reversible, plainly diagnostic. |
| F4 | Toggles texture replacements. Same. |

### The window

```
state 5   frame 1851   5 elements
[ ] Hold this frame   [ Save to hud.json ]   [ Clear overrides ]
[ part of an identity ] filter
 #  identity                        kind   x         y         class
 0  tex:0x000F8A20  dl:0x8021C4E0   tex    25..77    21..53    left     v
 1  tex:0x000F91C0  dl:0x8021C4E0   tex    208..293  21..42    right    v
 2  tex:0x000FA040  dl:0x8021C560   tex    236..292  51..68    right    v
 3  dl:0x8021C700   untex           untex  0..320    0..240    center   v
 4  fill:0x00010001 dl:0x8021C700   fill   0..320    0..240    center   v
```

| Column | Meaning |
|---|---|
| `#` | Position in this frame's display list. Not stable between frames. |
| `identity` | What `hud.json` calls this element. **This is the string you tag**, and which scheme it uses depends on the draw — see below. The second string is a weaker fallback, and can also be tagged. |
| `kind` | `tex`, `fill` or `untex`: which identity scheme this row got. Two rows can otherwise look alike and behave completely differently. |
| `x`, `y` | Extent in the game's own 320×240 pixels, before any widescreen arithmetic. |
| `class` | What the classifier decided, as a dropdown. A `*` after it means you have overridden it. |

**Identities are not all texture addresses.** An untextured rectangle carries whatever `G_SETTIMG`
last set, which belongs to some *other* element entirely, so identifying one by texture address gives
two unrelated draws the same name. That is not hypothetical: the pause screen's translucent black
overlay and the menu text beneath it came out sharing a `tex:` address, and tagging the overlay
stretched the text. So:

| `kind` | Identity | Second | Because |
|---|---|---|---|
| `tex` | `tex:<G_SETTIMG address>` | `dl:<display list>` | A textured element is named by its texture; the display list separates two uses of one texture. |
| `fill` | `fill:<fill colour>` | `dl:<display list>` | A fill-cycle rectangle has no texture. Its colour is far more specific than its display list and stable between frames — a backdrop keeps its colour. |
| `untex` | `dl:<display list>` | `untex` | A shaded or blended rectangle — what a translucent overlay is. The display list is all there is, so it *is* the identity, and the texture address is deliberately not offered because it belongs to somebody else. |
| `persp` `ortho` `rect` `proj` | `<kind>:<projection index>` | — | A whole **projection**, not a single draw: the 3D world, 2D drawn as geometry, the layer rectangles are drawn into, anything else. A projection is the only handle on anything drawn as geometry, because individual triangles are not hooked. |

**Projection rows mean something different from element rows:** `left` and
`right` give the layer a viewport origin, `center` lets it be widened with its aspect ratio
compensated (content proportional, more empty space either side), `stretch` suppresses that
compensation so the layer's 320-wide space maps across the whole frame, and `spill` is meaningless
and treated as `center`. The projection index is the one RT64's own Game editor shows — its
`Orthographic #4` is `ortho:4` here.

Two warnings specific to them. **The outline is the projection's scissor**, which for a full-screen
layer is the whole screen, so hovering will not tell two such layers apart — try them one at a time
instead. And **`ortho:<n>` is positional**, which makes it the weakest identity of the four: a
projection order that differs between screens moves the tag onto a different layer.

A `dl:` identity is the next weakest: several draws can share one display list, so a tag on
one may catch its neighbours. Check the outline before believing it, and if it over-matches, say so
— the element needs something more specific than this scheme currently provides.

The header's `state` is `currentGameState` (`gGameSettings + 0xA4`): **14** in the Controller Pak
prompts and every front-end menu, **2** for the boot attract sequence, **5** for a race. See
[technical/06](technical/06-graphics.md#knowing-when-the-hud-is-on-screen).

| Control | What it does |
|---|---|
| **Hover a row** | Outlines that element on the screen in yellow. |
| **Click a row** | Keeps the outline up in blue, so it stays while you use the dropdown. Click again to deselect. |
| **class dropdown** | `as classified` drops the override; the five classes apply from the **next frame**. No rebuild, no restart. |
| **Hold this frame** | Freezes the *list* on the frame that was current when you ticked it, while the game keeps running. For anything animated. |
| **Save to hud.json** | Writes every override into the tag table in the settings folder. Merges — entries you did not touch survive, and an identity is removed from the other four lists first. |
| **Clear overrides** | Drops every override. Does not touch `hud.json`. |
| **filter** | Substring match on either identity. |

### The six classes

| Class | What the fork emits | Use it for |
|---|---|---|
| `center` | Nothing. The element stays in the 4:3 box in the middle. | Anything that should keep its shape and position: HUD readouts, menu text. |
| `left` | `G_EX_ORIGIN_LEFT` on both edges, plus a widened scissor for that draw | An element pinned to the left edge of the screen. |
| `right` | `G_EX_ORIGIN_RIGHT` on both edges, plus a widened scissor | An element pinned to the right edge. |
| `stretch` | `rectAspect = G_EX_ASPECT_STRETCH`, origins left `G_EX_ORIGIN_NONE`, plus a widened scissor | Backgrounds, full-screen overlays, menu wipes — anything that should cover the widened frame. |
| `spill` | Nothing about placement. The game's 4:3 scissor is lifted for that draw only. | An element that is already in the right place at the right size and is merely being **cut off** at the old frame's edge. |
| `cover` | **Projections only.** `stretch`, and then magnify the layer's content about its centre by 320/274 and 240/206 — about 1.167 — putting BAR's overscan-safe rectangle on the frame's edges. | A layer whose geometry is authored inset for a television and so cannot reach the edges however the layer itself is scaled. The pause screen's backdrop is the worked example. |

Two distinctions that cost people days:

**`spill` is about clipping, not placement.** The other four decide where an element goes; this one
decides whether it may continue past where the 4:3 frame used to end. If an element looks
*positioned* wrongly, `spill` is not the answer — and if it looks *cut*, none of the other four are.
BAR scissors a race to `x 8..311`, so an element the game draws from `x 0` has eight units hidden
even on original hardware; `spill` reveals them.

**`G_EX_ASPECT_STRETCH` means "do not squeeze this to 4:3", not "cover the widened frame".** With
it, a 320-wide rectangle reaches the frame's full width and a narrower one is widened about its
centre by the same factor. Pinning an element's own edges to the frame's edges is a different
operation — that is what the origins do.

### Where a class comes from, and what beats what

Highest first:

1. **The inspector's dropdown**, in memory, from the next frame.
2. **`hud.json`**, read once at startup.
3. **The positional heuristic** in `lib/rt64/src/hle/rt64_bar_hud.cpp` — top/bottom bands, left/right
   thirds — which runs **only during a race**, because BAR's menus are authored as whole 4:3
   compositions and come apart pulled to the edges.

So a menu element is **listed and taggable everywhere**, but nothing in a menu moves until you have
explicitly tagged it. `BAR_HUD_ANCHOR=0` disables the heuristic without disabling tags, which is what
isolates the two.

### A worked example

1. Get the element on screen and press **F1**.
2. Find its row — hover rows until the yellow outline lands on it, or type part of a texture address
   into the filter.
3. **Click** the row so the outline stays up in blue.
4. Change the **class** dropdown and watch the next frame. If it is in the wrong *place*, try
   `left`, `right` or `stretch`; if it is being *cut off*, `spill`.
5. When it looks right, press **Save to hud.json**.

`Save to hud.json` writes to the per-user settings folder —
`%LOCALAPPDATA%\beetle-adventure-racing-recomp\hud.json` on Windows. The file is a list per class:

```json
{
    "center":  [],
    "left":    ["tex:0x000F8A20"],
    "right":   ["tex:0x000F91C0", "dl:0x8021C560"],
    "stretch": [],
    "spill":   []
}
```

### Promoting a tag

`hud.json` is where an element is **identified**, against a running game. Once the answer is known it
belongs in the build, so that everyone has it and nobody needs the right file in their settings
folder. Move the identity into `sBuiltinTags` in `lib/rt64/src/hle/rt64_bar_hud.cpp`:

```cpp
static const BuiltinTag sBuiltinTags[] = {
    { "tex:0x003C6E20", Class::Center },
};
```

and then clear it out of `hud.json`. The built-in table is consulted **after** the panel's override
and `hud.json`, so both still win over it — which is what lets a promoted answer be checked against a
different one without editing the array.

A `hud.json` repeating what the code already does is not wrong, but it is one more thing that can
disagree.

---

## How it is put together

Four pieces. The port half is `src/main/bar_inspector.{h,cpp}`; the rest is in the RT64 fork.

| Piece | Where | What |
|---|---|---|
| The hook | `lib/rt64/src/hle/rt64_bar_hud.h` | Four `extern "C"` function pointers, **null unless the port sets them**, so RT64 still builds and behaves identically on its own. |
| The draw call | `lib/rt64/src/hle/rt64_state.cpp` | `RT64_PortInspectorHook()` called from `State::inspect()` with an ImGui frame already open, and `BarHud::endFrame()` from `State::advanceWorkload`, which is the frame boundary. |
| The classifier | `lib/rt64/src/hle/rt64_bar_hud.cpp` | Builds each element's identity, asks the port for a tag, falls back to the positional heuristic, publishes the element, and returns the class. |
| The emit site | `lib/rt64/src/hle/rt64_rdp.cpp` | Turns a class into origins, an aspect mode or a widened scissor, in `RDP::drawRect`. |

Two threads meet in `bar_inspector.cpp`. The classifier runs on the thread that submits display lists
and fills a frame under construction; the panel runs on the thread that owns the renderer's UI and
reads the last frame that was completed. They swap under a lock, once per display list. The tag
lookup is behind an atomic flag that is false whenever both tables are empty, so the common case
costs one relaxed load per rectangle rather than an uncontended lock.

### Why `cover` exists, and how it was measured

`stretch` scales a layer; it does not resize the geometry inside it. BAR's pause backdrop is the
worked example. With its orthographic projection tagged `stretch`, the layer's full 320-wide space
does span the frame — and the backdrop still stopped short, because the quad is drawn at BAR's
overscan-safe rectangle *inside* that space.

The measurement that settled it, taken by sampling row brightness across a 1536×864 capture of a
paused race in the region clear of the panel:

```
y= 64   131.7
y= 72   148.3
y= 80    74.1   <- the backdrop's top edge
```

A hard step at y ≈ 78 of 864: **9% of the frame height, 21 of 240 game pixels**. Not a stretch that
fell short — the inset, exactly, matching the 22 and 17 that
[`KNOWN_ISSUES.md`](KNOWN_ISSUES.md) records for the overscan mask.

`cover` magnifies the layer by the ratio that puts that rectangle on the frame's edges, which is what
finally closed it. `BAR_HUD_COVER_INSET="l,t,r,b"` overrides the rectangle without a rebuild, since
it is measured from one game.

**What it costs:** anything the layer draws *outside* the inset rectangle is pushed off the frame.
That is inherent to magnifying, and why `cover` is tagged onto a backdrop rather than applied by
default.

### Differences from the Wave Race version

* **The classifier is in the renderer, not the port.** Wave Race rewrites display lists port-side
  (`src/dlrewrite.cpp`) before RT64 sees them; BAR classifies inside its RT64 fork. So the hook runs
  the other way round — RT64 calls out to the port, through pointers the port installs — and the
  element list is available in **both** the frontend and the headless build, where Wave Race's needs
  `build-fe`.
* **Rectangles and orthographic projections, but not triangles.** `RDP::drawRect` is hooked and so
  is the projection processor; `drawTris` is not. A piece of geometry can therefore be tagged through
  the projection that draws it, but never on its own.
* **Three identity schemes rather than two.** Wave Race names an element by texture or by display
  list; BAR adds `fill:<colour>` for fill-cycle rectangles, and makes `dl:` the *primary* identity of
  an untextured draw rather than its fallback. See [Identities](#the-window) above for why.
