import sys
import re
import os

def scan_L_symbols(filename: str):
    fname = os.path.basename(filename)

    base_name = fname.split('.rodata')[0]

    with open(filename) as f:
        lines = f.readlines()

    current_symbol = None
    found_L = False

    for line in lines:
        dlabel_match = re.match(r'dlabel\s+(\S+)', line)
        if dlabel_match:
            if found_L and current_symbol:
                newSymDeclareExpression = f"{current_symbol} = {current_symbol.replace("D_00", "0x")} // type:jtbl"

                with open(f"linker_scripts/us/module_files/{base_name}_symbol_addrs.txt", "a") as symsFile:
                    symsFile.write(newSymDeclareExpression + "\n")

                print(current_symbol)
            current_symbol = dlabel_match.group(1)
            found_L = False
            continue

        if re.search(r'\.L[0-9A-Fa-f]+', line):
            found_L = True

    if found_L and current_symbol:
        print(current_symbol)

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: python scan_L_symbols.py <input_file>")
        sys.exit(1)
    scan_L_symbols(sys.argv[1])
