cd ~/projects/bar-decomp
source .venv/bin/activate
MOD="$1"; FN="$2"; CAND="$3"; PROTOFIX="$4"
SRC="src/modules/$MOD.c"
PRAGMA="#pragma GLOBAL_ASM(\"asm/us/nonmatchings/modules/$MOD/$FN.s\")"
cp "$SRC" /tmp/src.bak
if [ -n "$PROTOFIX" ]; then sed -i "$PROTOFIX" "$SRC"; fi
python3 - "$SRC" "$CAND" "$PRAGMA" <<'PYEOF'
import sys
src,cand,pr=sys.argv[1],sys.argv[2],sys.argv[3]
o=open(src).read(); c=open(cand).read().rstrip()
open(src,"w").write(o.replace(pr,c))
PYEOF
rm -f build/src/modules/$MOD.o
make build/src/modules/$MOD.o >/tmp/blog.txt 2>&1
if ! grep -q "Compiling" /tmp/blog.txt && grep -qi error /tmp/blog.txt; then echo BUILDERR; grep -i error /tmp/blog.txt | head; cp /tmp/src.bak "$SRC"; exit 0; fi
mips-linux-gnu-objdump -d build/src/modules/$MOD.o | sed -n "/<$FN>:/,/^$/p" | sed -E 's/^ +[0-9a-f]+:\t[0-9a-f ]+\t//' | sed -E 's/ +[0-9a-f]+ <.*>//; s/,/ /g' | awk 'NF{print $1}' > /tmp/mine.txt
cp /tmp/src.bak "$SRC"
grep -oE '/\* [0-9A-F]+ [0-9A-F]+ [0-9A-F]+ \*/  [a-z0-9.]+' asm/us/nonmatchings/modules/$MOD/$FN.s | awk '{print $NF}' > /tmp/tgt.txt
echo "MINE=$(wc -l </tmp/mine.txt) TGT=$(wc -l </tmp/tgt.txt)"
diff /tmp/mine.txt /tmp/tgt.txt
