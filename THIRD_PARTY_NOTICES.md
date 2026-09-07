# Third-Party Notices & License Analysis

beetle-adventure-racing-recomp itself is licensed **AGPL-3.0** (see `COPYING`), inherited from the BeetleDecomp
symbol/function data it derives from. Vendored dependencies retain their own licenses.

## Dependency licenses

| Component | Role | License | Source |
|---|---|---|---|
| N64Recomp (+ RSPRecomp) | static recompiler | **MIT** | https://github.com/N64Recomp/N64Recomp/blob/main/LICENSE |
| N64ModernRuntime (librecomp, ultramodern) | runtime | **GPL-3.0** | https://github.com/N64Recomp/N64ModernRuntime/blob/main/COPYING |
| RT64 | renderer (D3D12 / Vulkan) | **MIT** (formerly restrictive; now MIT) | https://github.com/rt64/rt64/blob/main/LICENSE |
| RmlUi | UI | **MIT** | https://github.com/mikke89/RmlUi/blob/master/LICENSE.txt |
| lunasvg | SVG icons | **MIT** | https://github.com/sammycage/lunasvg |
| SDL2 / SDL3 | platform / input | **Zlib** | https://www.libsdl.org/license.php |
| miniaudio | audio backend | **Unlicense / MIT-0** | https://github.com/mackron/miniaudio/blob/master/LICENSE |
| BeetleDecomp | symbol / function data | **AGPL-3.0** | https://github.com/bryankruman/BeetleDecomp |

## Why AGPL-3.0 for this repo

- The symbol/function metadata feeding the recompiler is derived from the **AGPL-3.0** decomp,
  so the combined work must be copyleft.
- **AGPL-3.0 §13 and GPL-3.0 §13 are mutually compatible**, so the GPL-3.0 runtime
  (N64ModernRuntime) can be conveyed within an AGPL-3.0 combined work. AGPL-3.0 is the strongest
  copyleft in the dependency graph and absorbs the permissive deps (MIT / Zlib / Unlicense).
- This matches precedent: ports deriving from copyleft material adopt that copyleft (e.g.
  `cvlod_recomp` is GPL-3.0 because it imports Zelda64Recomp's GPL-3.0 framework). The permissive
  route some ports take (e.g. Paper Mario ReCut → MIT) is only available when the upstream decomp
  has *no* license — which is not our case.

## Obligations

- Ship **complete corresponding source** with any binary release (GPL / AGPL).
- **AGPL §13:** if AGPL-covered parts are modified and run as a network service, offer source to
  remote users. (Rare for a single-player port, but noted.)
- Preserve the copyright/license notices of the MIT / Zlib / Unlicense dependencies.
- **Never distribute game assets or a ROM.**

> Verify the exact `AGPL-3.0-only` vs `AGPL-3.0-or-later` posture against BeetleDecomp's own
> declaration; this repo follows whatever the decomp uses.

---

### Notes on the recompiler's generated code
N64Recomp (MIT) imposes no license on the C it generates; that output's effective license is
governed by its **inputs** — here, the AGPL-3.0 decomp data. Do not treat `RecompiledFuncs/` as
permissive.
