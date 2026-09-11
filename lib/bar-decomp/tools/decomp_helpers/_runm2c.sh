#!/bin/bash
cd ~/projects/bar-decomp
for n in "$@"; do
  echo "===== $n ====="
  python3 tools/m2c/m2c.py --target mips-ido-c "asm/us/nonmatchings/modules/uvdyn_rom/func_uvdyn_rom_$n.s" 2>&1
  echo ""
done
