import argparse
import re
import sys


def scan_symbols(map_path, target_symbols):
    results = {}

    pattern = re.compile(r"^\s*(0x[0-9A-Fa-f]+)\s+([A-Za-z_][A-Za-z0-9_]*)\s*=")

    with open(map_path, "r") as f:
        for line in f:
            match = pattern.search(line)
            if not match:
                continue

            value, name = match.groups()
            # print(f"{name} = {hex(int(value,16))}")
            if name in target_symbols:
                results[name] = int(value, 16)

    return results


if __name__ == "__main__":
    if len(sys.argv) < 3:
        print("createKernelLinkSyms <input> <output>")
        exit(0)

    success = True
    targets = {
        "UVTS_25_ROM_END",
        "FORM0_ROM_START",
        "main_ROM_END",
        "MODULE_FILES_START",
        "main_ROM_START",
        "main_VRAM",
    }

    result = scan_symbols(sys.argv[1], targets)

    with open(sys.argv[2], "w", encoding="utf-8", newline="\n") as f:
        for sym in targets:
            if sym in result:
                f.write(f"{sym} = 0x{result[sym]:X};\n")
            else:
                f.write(f"{sym} NOT FOUND\n")
                success = False
                break

    if not success:
        raise KeyError(f"Script failed: couldn't find symbol '{sym}'")

    print("Done")
