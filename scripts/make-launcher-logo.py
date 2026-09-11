#!/usr/bin/env python3
"""Build assets/ui/icons/Logo.svg — the launcher's background artwork — from icons/Icon.png.

recompui's LauncherMenu::set_launcher_background_svg lays the SVG out at 100% of the window
width, height auto, centred vertically. So the viewBox's aspect ratio is what decides how tall
the artwork is on screen, and the wide, short box below keeps the icon in the band between the
launcher's title (top 25%) and its menu (bottom 25%) rather than behind either of them.

lunasvg 3.x (recompui/lib/lunasvg) resolves <image href="data:image/png;base64,..."> through
plutovg_surface_load_from_image_base64, so the icon is embedded rather than referenced — the
launcher resolves one asset path, not two.

Re-run after changing icons/Icon.png:
    python scripts/make-launcher-logo.py
"""

import base64
import io
from pathlib import Path

from PIL import Image

REPO = Path(__file__).resolve().parent.parent
SOURCE = REPO / "icons" / "Icon.png"
OUTPUT = REPO / "assets" / "ui" / "icons" / "Logo.svg"

# The icon is square; the canvas is 680x120 so that the rendered band is roughly a third of the
# window height. 512 px is the largest the icon is ever drawn at (a 4K window's third-height band
# is about 360 px tall), so anything larger is only file size.
CANVAS_W, CANVAS_H = 680, 120
ICON_PX = 512


def main() -> None:
    icon = Image.open(SOURCE).convert("RGBA")
    icon = icon.resize((ICON_PX, ICON_PX), Image.LANCZOS)

    buf = io.BytesIO()
    icon.save(buf, format="PNG", optimize=True)
    data = base64.b64encode(buf.getvalue()).decode("ascii")

    x = (CANVAS_W - CANVAS_H) / 2
    svg = (
        f'<svg xmlns="http://www.w3.org/2000/svg" '
        f'xmlns:xlink="http://www.w3.org/1999/xlink" '
        f'viewBox="0 0 {CANVAS_W} {CANVAS_H}" width="100%" role="img">\n'
        f'  <title>Beetle Adventure Racing!</title>\n'
        f'  <image x="{x:g}" y="0" width="{CANVAS_H}" height="{CANVAS_H}" '
        f'preserveAspectRatio="xMidYMid meet" '
        f'xlink:href="data:image/png;base64,{data}"/>\n'
        f'</svg>\n'
    )

    OUTPUT.parent.mkdir(parents=True, exist_ok=True)
    OUTPUT.write_text(svg, encoding="utf-8")
    print(f"wrote {OUTPUT.relative_to(REPO)} ({len(svg) / 1024:.0f} KiB)")


if __name__ == "__main__":
    main()
