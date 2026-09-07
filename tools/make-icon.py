#!/usr/bin/env python3
"""make-icon.py -- build icons/app.ico (the Windows executable icon) from a source PNG.

The .ico is committed, because the build must not depend on Python or Pillow. This script exists so
that blob is reproducible: run it whenever the source art changes.

    python tools/make-icon.py [source.png] [-o icons/app.ico]

Two things it does that a plain "resize to 256x256" would not:

* **Crops to the artwork.** The source render sits on a transparent canvas with a wide margin, and
  every wasted row costs real detail at 16x16 -- the size Explorer and the taskbar actually use.
  Cropping to the alpha bounding box and re-padding to a square with a small, deliberate margin
  gives the car roughly a third more linear resolution at every icon size.

* **Writes every size Windows asks for.** Windows picks the nearest available entry and rescales if
  it has to, so shipping only 256x256 leaves the shell to do its own 16x16 downscale -- which is
  a plain box filter, and turns a detailed render to mush. Downscaling each size here with Lanczos
  is visibly sharper.
"""

import argparse
import io
import struct
import sys
from pathlib import Path

try:
    from PIL import Image
except ImportError:                                  # pragma: no cover
    sys.exit("make-icon: needs Pillow -- pip install Pillow")

REPO_ROOT = Path(__file__).resolve().parent.parent

# Windows uses 16 (taskbar, title bar, small Explorer), 32 (desktop), 48 (medium Explorer) and 256
# (extra-large / the Vista+ PNG-compressed entry) most; the rest avoid a rescale at other DPIs.
ICON_SIZES = [16, 20, 24, 32, 40, 48, 64, 128, 256]

# Fraction of the final canvas left as empty margin around the artwork. Icons that bleed to the very
# edge look larger than their neighbours in a list; a little air matches the platform's own icons.
MARGIN = 0.04

# Entries at or above this size are stored as PNG, smaller ones as uncompressed BMP. PNG-compressed
# icon entries were introduced for the 256x256 size and that is the size every consumer is known to
# accept them at; below it, plain BMP is what the format has always specified, and it is what the
# older shell code paths expect. The size cost is trivial at these dimensions.
PNG_FROM_SIZE = 256


def encode_bmp_entry(frame: Image.Image) -> bytes:
    """Encode one icon frame in the ICO flavour of BMP: a doubled-height BITMAPINFOHEADER, 32bpp
    bottom-up BGRA pixels, then the legacy 1bpp AND mask.

    The AND mask is vestigial for 32bpp icons -- the alpha channel is what actually gets composited
    -- but it must still be present and correctly sized, and some consumers do consult it, so it is
    derived from alpha rather than left zeroed.
    """
    w, h = frame.size
    px = frame.load()

    xor = bytearray()
    for y in range(h - 1, -1, -1):                   # BMP rows run bottom-to-top
        for x in range(w):
            r, g, b, a = px[x, y]
            xor += bytes((b, g, r, a))               # BMP channel order is BGRA

    # 1 bit per pixel, 0 = opaque; each row padded out to a 4-byte boundary.
    row_bytes = ((w + 31) // 32) * 4
    and_mask = bytearray()
    for y in range(h - 1, -1, -1):
        row = bytearray(row_bytes)
        for x in range(w):
            if px[x, y][3] == 0:
                row[x // 8] |= 0x80 >> (x % 8)
        and_mask += row

    header = struct.pack("<IiiHHIIiiII",
                         40,          # biSize
                         w, h * 2,    # biWidth, biHeight (XOR + AND stacked)
                         1, 32,       # biPlanes, biBitCount
                         0,           # biCompression = BI_RGB
                         len(xor) + len(and_mask),
                         0, 0, 0, 0)  # resolution and palette fields: unused
    return bytes(header + xor + and_mask)


def build_ico(frames: list[Image.Image]) -> bytes:
    """Assemble the ICONDIR / ICONDIRENTRY container around already-resized frames."""
    payloads = []
    for f in frames:
        if f.width >= PNG_FROM_SIZE:
            buf = io.BytesIO()
            f.save(buf, format="PNG", optimize=True)
            payloads.append(buf.getvalue())
        else:
            payloads.append(encode_bmp_entry(f))

    out = bytearray(struct.pack("<HHH", 0, 1, len(frames)))   # reserved, type 1 = icon, count
    offset = 6 + 16 * len(frames)
    for f, data in zip(frames, payloads):
        out += struct.pack("<BBBBHHII",
                           f.width % 256, f.height % 256,     # 256 is stored as 0
                           0, 0,                              # palette count, reserved
                           1, 32,                             # planes, bits per pixel
                           len(data), offset)
        offset += len(data)
    for data in payloads:
        out += data
    return bytes(out)


def square_with_margin(im: Image.Image) -> Image.Image:
    """Crop to the visible artwork, then centre it on a transparent square canvas."""
    bbox = im.getchannel("A").getbbox()
    if bbox is None:                                 # fully transparent source
        return im
    art = im.crop(bbox)

    side = int(round(max(art.size) / (1.0 - 2.0 * MARGIN)))
    canvas = Image.new("RGBA", (side, side), (0, 0, 0, 0))
    canvas.paste(art, ((side - art.width) // 2, (side - art.height) // 2))
    return canvas


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("source", nargs="?", default=REPO_ROOT / "assets/ui/icons/Icon.png",
                    type=Path, help="source PNG (default: assets/ui/icons/Icon.png)")
    ap.add_argument("-o", "--output", default=REPO_ROOT / "icons/app.ico", type=Path,
                    help="output .ico (default: icons/app.ico)")
    args = ap.parse_args()

    im = Image.open(args.source).convert("RGBA")
    print(f"make-icon: source {args.source} ({im.width}x{im.height})")

    squared = square_with_margin(im)
    print(f"make-icon: cropped to artwork and squared -> {squared.width}x{squared.height}")

    frames = [squared.resize((s, s), Image.LANCZOS) for s in ICON_SIZES]

    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_bytes(build_ico(frames))

    print(f"make-icon: wrote {args.output} "
          f"({args.output.stat().st_size:,} bytes, sizes {ICON_SIZES}, "
          f"PNG from {PNG_FROM_SIZE}px)")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
