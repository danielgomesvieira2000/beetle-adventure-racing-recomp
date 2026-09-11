# UI asset provenance and licensing

These assets back the RecompFrontend (`recompui`) launcher and menus. The build stages this whole
directory next to the executable as `assets/`, because recompui resolves every asset as
`<exe dir>/assets/<name>`.

## The shape of this directory is deliberate

It mirrors `danielgomesvieira2000/wave-race-64-recomp`, the reference N64: Recompiled port for this
project: a near-empty `recomp.rcss`, `icons/`, and `promptfont/`.

**`recomp.rcss` is intentionally tiny.** recompui's elements style themselves in code from a named
theme palette (Background1..3, Text, Primary, Secondary, Warning, Danger, Success, Border), which is
why every one of these ports looks alike. A stylesheet that sets its own colours does not restyle
those elements, it competes with them. The one thing the library genuinely cannot know is the font,
so that is all this file supplies. To restyle the menus, call `recompui::theme::set_theme_color`
from C++ rather than adding rules here.

An earlier attempt imported Zelda 64: Recompiled's 62 KB `recomp.rcss`. That was wrong: that sheet
belongs to Zelda64Recomp's older in-repo UI rather than to RecompFrontend, and it sets
`body { font-family: chiaro }` — a Nintendo-associated typeface that is not redistributable here.
Do not reintroduce it.

## recomp.rcss, icons/

From `danielgomesvieira2000/wave-race-64-recomp`, this project's own reference port.

`icons/Logo.svg` is the exception: it is generated from this project's own `icons/Icon.png` by
`scripts/make-launcher-logo.py`, and is what the launcher draws as its background. Re-run that
script after changing the icon rather than editing the SVG, which is a base64-embedded PNG.

## The typefaces are NOT in this directory

Lato and NotoEmoji are staged from RmlUi's sample assets at build time — see the asset-staging
block in `CMakeLists.txt`. RmlUi is already vendored inside `lib/RecompFrontend/recompui/lib/RmlUi`
and ships the whole family, so committing a second copy of binaries the tree already has would only
create two things to keep in step. Four files are staged:

| File | Why it is needed |
|---|---|
| `LatoLatin-Regular.ttf` | The primary font, registered by `src/frontend/bar_frontend.cpp`. |
| `LatoLatin-Bold.ttf` | The 700-weight face recompui's headings and labels ask for. |
| `LatoLatin-Italic.ttf` | The italic face, for the same reason. |
| `NotoEmoji-Regular.ttf` | recompui loads this **unconditionally** as a fallback face (`ui_state.cpp`'s `font_faces[]`). Without it, `Rml::LoadFontFace` fails at every startup. |

Lato is by Łukasz Dziedzic and Noto Emoji is by Google; both are under the **SIL Open Font
License 1.1**, and RmlUi ships the licence text alongside them.

The font's internal family name is **`LatoLatin`**, not `Lato`. RmlUi matches on the internal name,
and registering the wrong one fails silently in a way that wastes hours: every element lays out and
draws in the right place, with no text in any of them. It must stay in sync with
`kPrimaryFontFamily` in `src/frontend/bar_frontend.cpp` and with `body` in `recomp.rcss`.

## promptfont/

PromptFont, by Yukari "Shinmera" Hafner — controller and key glyphs used by `recompinput` for button
prompts. **SIL Open Font License 1.1**; `promptfont/LICENSE.txt` ships alongside as the licence
requires.
