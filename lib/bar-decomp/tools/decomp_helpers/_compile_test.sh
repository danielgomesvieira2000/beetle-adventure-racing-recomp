#!/bin/bash
set -e
cd ~/projects/bar-decomp
source .venv/bin/activate 2>/dev/null || true
.venv/bin/python3 tools/asm-processor/build.py --input-enc=utf-8 --output-enc=euc-jp --convert-statics=global-with-filename --drop-mdebug-gptab tools/ido-static-recomp/build/5.3/out/cc -- mips-linux-gnu-as -march=vr4300 -32 -G0 -- -c -G 0 -use_readonly_const -non_shared -fullwarn -verbose -Xcpluscomm -I include -I . -I include/libc -I include/2.0I -I include/2.0I/PR -I include -I bin -I tools/ultralib/include -nostdinc -Wab,-r4300_mul -woff 649,838,712,516 -DVERSION_US=1 -I include -I . -I include/libc -I include/2.0I -I include/2.0I/PR -I include -I bin -I tools/ultralib/include -fullwarn -verbose -woff 624,649,838,712,516,513,596,564,594,709,807 -mips2 -32 -EB -D_MIPS_SZLONG=32 -DNDEBUG -DF3DEX_GBI_2 -D_FINALROM -DCOMPILING_LIBULTRA -DTARGET_N64 -DSSSV -DWIN32 -DLANGUAGE_C -D_LANGUAGE_C -D_FINALROM -DNDEBUG -O2 -o /tmp/test_uvdyn.o _test_uvdyn.c 2>&1
echo "=== COMPILE OK ==="
mips-linux-gnu-objdump -d -m mips:4300 -EB /tmp/test_uvdyn.o
