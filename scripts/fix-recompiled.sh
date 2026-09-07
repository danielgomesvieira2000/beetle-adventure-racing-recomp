#!/usr/bin/env bash
# Post-process N64Recomp output. Run AFTER `./N64Recomp beetle-adventure-racing-recomp.toml`, BEFORE building.
#
# ---------------------------------------------------------------------------------------------
# WHY THIS EXISTS, AND WHY IT VERIFIES ITSELF
#
# Two kinds of change live here:
#
#   * Codegen fixups — N64Recomp emits C that does not compile. Mechanical, address-independent.
#   * BAR runtime fixes — behavioural corrections injected into the generated C, anchored on N64
#     instruction addresses (the `// 0x........:` comments) rather than on file names, so they
#     survive re-chunking.
#
# The second kind is the dangerous kind. An anchor that stops matching does not fail: the rule
# quietly applies to nothing, the build succeeds, and the game misbehaves in a way that looks
# nothing like "a patch went missing" — an OOM on one track, dead controllers, audio crackle.
#
# So every rule now VERIFIES ITS END STATE rather than counting what it changed this run. That
# distinction matters because the rules are idempotent: a second run legitimately changes nothing,
# so "changed 0 files" cannot be the failure signal. Instead each rule asserts that its sentinel is
# present in the tree afterwards, which is true whether it applied now or on a previous run.
#
# If an assertion fails the build STOPS with the anchor that went missing. Set FIXRECOMP_STRICT=0
# to downgrade failures to warnings — only useful while deliberately regenerating against a
# changed decomp, never in a normal build.
#
# Migrating these to named RECOMP_PATCH / RECOMP_HOOK functions (which bind to symbol names, not
# addresses, and so cannot rot this way) is the real fix. It is blocked: patches are C
# cross-compiled to MIPS, and the LLVM installed here has no MIPS backend. See docs/PORT-STATUS.md.
# ---------------------------------------------------------------------------------------------
set -euo pipefail
RF="$(cd "$(dirname "$0")/.." && pwd)/RecompiledFuncs"
[ -d "$RF" ] || { echo "fix-recompiled: $RF not found (run N64Recomp first)"; exit 1; }

STRICT="${FIXRECOMP_STRICT:-1}"
FAILURES=0

# require <actual> <minimum> <rule name> <what to look for if it broke>
require() {
    local actual="$1" minimum="$2" name="$3" anchor="$4"
    if [ "$actual" -lt "$minimum" ]; then
        FAILURES=$((FAILURES+1))
        echo "fix-recompiled: *** FAILED: $name" >&2
        echo "                expected at least $minimum, found $actual" >&2
        echo "                anchor: $anchor" >&2
        echo "                The recompiled output no longer contains what this rule targets —" >&2
        echo "                most likely the decomp ELF moved. Re-find the anchor; do NOT just" >&2
        echo "                delete the rule, it is a behavioural fix the game needs." >&2
    else
        echo "fix-recompiled: ok   $name ($actual)"
    fi
}

# Count matching lines across the generated sources without tripping `set -e` on no-match.
count_in_funcs() { { grep -rhc -- "$1" "$RF"/*.c 2>/dev/null || true; } | awk '{s+=$1} END{print s+0}'; }

# ---------------------------------------------------------------------------------------------
# Codegen fixups. Mechanical; these are not address-anchored and cannot rot the same way.
# ---------------------------------------------------------------------------------------------

# A load into $zero (`lw $zero, ...`) is emitted as `0 = MEM_W(...)`, which is not valid C ($zero is
# the literal 0). Rewrite to `(void)MEM_W(...)` so the read still happens for its side effects.
sed -i -E 's/^([[:space:]]*)0 = /\1(void)/' "$RF"/*.c
zero_left=$(count_in_funcs '^[[:space:]]*0 = ')
if [ "$zero_left" -ne 0 ]; then
    FAILURES=$((FAILURES+1))
    echo "fix-recompiled: *** FAILED: \$zero-load rewrite left $zero_left line(s) unfixed" >&2
else
    echo "fix-recompiled: ok   \$zero-load rewrite (none left)"
fi

# recomp_overlays.inl emits some relocs with an empty `.type` field. These are MIPS reloc types
# beyond N64Recomp's reloc_names table; librecomp's RelocEntryType only covers 0..7 and the
# recompiled code resolves addresses inline, so they are inert for running the base game. Rewrite
# to R_MIPS_NONE so the table is valid C++.
inl="$RF/recomp_overlays.inl"
if [ -f "$inl" ]; then
    sed -i 's/\.type =[[:space:]]*}/.type = R_MIPS_NONE }/g' "$inl"
    empty_left=$({ grep -c '\.type =[[:space:]]*}' "$inl" || true; })
    [ "$empty_left" -eq 0 ] || { FAILURES=$((FAILURES+1)); echo "fix-recompiled: *** FAILED: $empty_left empty reloc .type field(s) remain" >&2; }
fi

# Overlay bridge: rename the generated uvDoModuleRelocs DEFINITION so src/main/overlay_bridge.cpp
# can own the symbol. Its wrapper registers the just-loaded relocatable module with librecomp so the
# module's recompiled functions resolve, then calls uvDoModuleRelocs_orig. Idempotent.
if grep -rlq 'RECOMP_FUNC void uvDoModuleRelocs(' "$RF"/*.c 2>/dev/null; then
    sed -i 's/RECOMP_FUNC void uvDoModuleRelocs(/RECOMP_FUNC void uvDoModuleRelocs_orig(/' "$RF"/*.c
fi
require "$(count_in_funcs 'uvDoModuleRelocs_orig')" 1 \
    "overlay bridge rename (uvDoModuleRelocs -> _orig)" "RECOMP_FUNC void uvDoModuleRelocs("

# Hardware-register stubs: libultra functions recompiled raw that poke RCP registers, which are not
# memory-mapped in the recomp. Rename each generated definition so src/main/hw_stubs.cpp can own the
# symbol. Add a name here AND a stub in hw_stubs.cpp together.
for fn in func_8000E460; do
    if grep -rlq "RECOMP_FUNC void ${fn}(" "$RF"/*.c 2>/dev/null; then
        sed -i "s/RECOMP_FUNC void ${fn}(/RECOMP_FUNC void ${fn}__hwstub_orig(/" "$RF"/*.c
    fi
    require "$(count_in_funcs "${fn}__hwstub_orig")" 1 \
        "hardware-register stub rename (${fn})" "RECOMP_FUNC void ${fn}("
done

# ---------------------------------------------------------------------------------------------
# BAR runtime correctness fixes. Address-anchored; each verifies its sentinel afterwards.
# ---------------------------------------------------------------------------------------------

# (A) Heap cap: _uvMemAllocInit (0x80002A88) sizes the heap as 0x80400000 - gMemBlock (4 MB), and
# func_80005074 bounds-checks pointers against the same top. BAR is a 4 MB game but the recomp backs
# 8 MB RDRAM, so [0x80400000,0x80800000) is unused Expansion-Pak space. Extend BOTH constants to
# 0x80800000 (fixes the player-race _uvMemAlloc OOM on Coventry Cove). The gfx-manager framebuffer
# reserve is left alone, so this only ADDS the upper 4 MB above the framebuffers.
for f in "$RF"/*.c; do
    awk '
        /\/\/ 0x80002AA4:|\/\/ 0x800050B0:/ { armed=1; print; next }
        armed && /S32\(0X8040 << 16\)/      { sub(/0X8040 << 16/, "0X8080 << 16"); armed=0; print; next }
        { armed=0; print }
    ' "$f" > "$f.tmp"
    if ! cmp -s "$f" "$f.tmp"; then mv "$f.tmp" "$f"; else rm -f "$f.tmp"; fi
done
require "$(count_in_funcs 'S32(0X8080 << 16)')" 2 \
    "heap cap 0x80400000 -> 0x80800000" "// 0x80002AA4: and // 0x800050B0: followed by S32(0X8040 << 16)"

# (B) REMOVED — was: clamp __osMaxControllers in __osPackReadData (func_8000E6E0) at 0x8000E718,
# because BAR's reimplemented osContInit leaves that byte 0, so zero read-button commands got packed
# and no input reached the game.
#
# It is redundant. src/main/os_unimpl_stubs.cpp writes 4 to that byte (0x80032231) on every
# __osSiRawStartDma, and by its own comment that happens during osContInit's SI ops — before the
# first read-button pack — so the clamp never had anything left to do. Verified by removing the
# injected line and rebuilding: the game still takes input and reaches a race.
#
# One rule fewer, and a genuine one: the host-side write binds to a fixed RDRAM address that is part
# of the game's data layout, whereas this rule was anchored to an instruction address in generated C.

# (C) RSPRecomp audio ucode (rsp/aspMain.cpp): same $zero-load codegen quirk as above.
ASP="$(cd "$(dirname "$0")/.." && pwd)/rsp/aspMain.cpp"
if [ -f "$ASP" ]; then
    sed -i -E 's/^([[:space:]]*)0 = /\1(void)/' "$ASP"
    asp_left=$({ grep -cE '^[[:space:]]*0 = ' "$ASP" || true; })
    [ "$asp_left" -eq 0 ] || { FAILURES=$((FAILURES+1)); echo "fix-recompiled: *** FAILED: $asp_left \$zero-load line(s) remain in rsp/aspMain.cpp" >&2; }
fi

# (D) Intro skip: func_intro_004005CC already skips the attract on START (0x1000) or A (0x8000). The
# game reads input at 0x81D00618; inject a B (0x4000) check right after so B also skips.
fb_fix='    { extern int bar_intro_skip(void); if (bar_intro_skip() && (ctx->r5 & 0X4000)) ctx->r24 = 0X1000; }  // BAR: B also skips the attract cinematic (game already skips on START/A)'
for f in "$RF"/*.c; do
    if grep -q '0x81D00618: andi' "$f" && ! grep -q 'B also skips the attract' "$f"; then
        awk -v fix="$fb_fix" '
            { print; if (prev ~ /0x81D00618:/ && $0 ~ /ctx->r24 = ctx->r5 & 0X1000;/) print fix; prev=$0 }
        ' "$f" > "$f.tmp" && mv "$f.tmp" "$f"
    fi
done
require "$(count_in_funcs 'B also skips the attract')" 1 \
    "attract intro-skip (B button)" "0x81D00618: andi -> ctx->r24 = ctx->r5 & 0X1000;"

# (E) Audio underrun on state transitions: func_uvcmidi_rom_00400940 stops the MIDI sequence player
# then SPINS until it reports stopped, or 2.0s. The player state only advances when the AUDIO thread
# runs, and this spin never yields, so under the cooperative scheduler it burns the full timeout on
# EVERY state transition and the host audio queue drains. Inject a 1ms yield at the loop top.
midi_fix='    { extern void yield_self_1ms(uint8_t* rdram); yield_self_1ms(rdram); }  // BAR FIX: MIDI stop-wait — pump the audio thread so the seq player actually stops (else this loop spins the full 2.0s timeout, starving audio -> underrun crackle on every state transition)'
for f in "$RF"/*.c; do
    if grep -q '^L_85600984:' "$f" && ! grep -q 'BAR FIX: MIDI stop-wait' "$f"; then
        awk -v fix="$midi_fix" '{ print; if ($0 ~ /^L_85600984:/) print fix }' "$f" > "$f.tmp" && mv "$f.tmp" "$f"
    fi
done
require "$(count_in_funcs 'BAR FIX: MIDI stop-wait')" 1 \
    "MIDI stop-wait yield" "label L_85600984 in func_uvcmidi_rom_00400940"

# (F) GENERAL cooperative preemption. ultramodern has no preemption: a heavy straight-line per-frame
# update starves the pri-110 audio manager. Inject a poll of a host-set "should-yield" flag (raised
# ~500Hz by src/main/bar_preempt.cpp) at EVERY function prologue; when set, on a game thread, yield.
# bar_consume_yield self-clears and gates to game threads, so the steady cost is one relaxed atomic
# load per call. Anchored on `int c1cs = 0;`, emitted once in every recompiled function body (absent
# from forward declares and from rsp/aspMain.cpp, so the RSP task thread is never touched).
preempt_fix='    { extern int bar_consume_yield(uint8_t* rdram); extern void yield_self_1ms(uint8_t* rdram); if (bar_consume_yield(rdram)) yield_self_1ms(rdram); }  // BAR FIX: cooperative-preempt (audio)'
for f in "$RF"/*.c; do
    if grep -q '^    int c1cs = 0;' "$f" && ! grep -q 'BAR FIX: cooperative-preempt' "$f"; then
        awk -v fix="$preempt_fix" '
            { print; if ($0 ~ /^[[:space:]]*int c1cs = 0;[[:space:]]*$/) print fix }
        ' "$f" > "$f.tmp" && mv "$f.tmp" "$f"
    fi
done
require "$(count_in_funcs 'BAR FIX: cooperative-preempt')" 1000 \
    "cooperative-preempt prologue poll" "the per-function-body line 'int c1cs = 0;'"

# (G) SP_STATUS hardware-register write in _uvScDlistRecover (store at 0x80004024). The display-list
# overflow recovery path writes 0x2902 to SP_STATUS_REG (0xA4040010) to reset the RSP. Under RT64 HLE
# there is no real RSP and the RCP register range is not backed by the rdram buffer, so the raw store
# faults whenever recovery runs. Guard it; normal RDRAM stores are unaffected.
sp_fix='    if (((uint32_t)ctx->r15 & 0xFF000000u) != 0xA4000000u) MEM_W(0X10, ctx->r15) = ctx->r14;  // BAR FIX: skip RCP-register (SP_STATUS 0xA4040010) write — RT64 HLE has no real RSP; the raw store faults'
for f in "$RF"/*.c; do
    if grep -q '0x80004024: sw' "$f" && ! grep -q 'BAR FIX: skip RCP-register' "$f"; then
        awk -v fix="$sp_fix" '
            { if ($0 ~ /^[[:space:]]*MEM_W\(0X10, ctx->r15\) = ctx->r14;[[:space:]]*$/ && prev ~ /0x80004024:/) print fix; else print; prev=$0 }
        ' "$f" > "$f.tmp" && mv "$f.tmp" "$f"
    fi
done
require "$(count_in_funcs 'BAR FIX: skip RCP-register')" 1 \
    "SP_STATUS hardware-write guard in _uvScDlistRecover" "0x80004024: sw -> MEM_W(0X10, ctx->r15) = ctx->r14;"

# (H) Draw distance, and the frustum the game culls against. func_uvfmtx_rom_00401F74 is BAR's
# glFrustum(dst, left, right, top, bottom, near, far) -- the single place a perspective projection is
# built -- and its caller stores the same six values in the camera channel, from which
# func_uvchannel_rom_00401658 derives the six culling planes. So this one function is the choke point
# for both what is drawn and what the game bothers to submit. src/main/bar_frustum.cpp explains the
# numbers and the channel guard; here we only hand it the arguments and write back what it changed.
#
# Anchored on the FUNCTION NAME rather than an instruction address, so unlike the rules above it
# cannot rot when the decomp ELF moves. The argument slots are the MIPS o32 layout this function is
# compiled with: a1..a3 are left, right and top, and bottom, near and far are at 0x10, 0x14 and 0x18
# of the caller's stack -- read them back from the same places the body is about to load them from.
#
# Every line carries its own /* */ comment marker rather than a leading // on the first line. An
# earlier version of this rule collapsed to a single line, whose leading // then commented out the
# whole hook: it compiled, the sentinel was present, the rule reported ok, and the hook did nothing.
# The verification below therefore also asserts the call and the declaration land on separate lines.
frustum_hook='    { /* BAR: draw distance + the frustum the game culls against (src/main/bar_frustum.cpp) */\n        extern void bar_frustum_adjust(uint8_t*, unsigned, unsigned*, unsigned*, unsigned*, unsigned*, unsigned*, unsigned*);\n        unsigned bar_l = (unsigned)ctx->r5, bar_r = (unsigned)ctx->r6, bar_t = (unsigned)ctx->r7;\n        unsigned bar_b = (unsigned)MEM_W(0X10, ctx->r29), bar_n = (unsigned)MEM_W(0X14, ctx->r29), bar_f = (unsigned)MEM_W(0X18, ctx->r29);\n        bar_frustum_adjust(rdram, (unsigned)ctx->r4, &bar_l, &bar_r, &bar_t, &bar_b, &bar_n, &bar_f);\n        ctx->r5 = (int32_t)bar_l; ctx->r6 = (int32_t)bar_r; ctx->r7 = (int32_t)bar_t;\n        MEM_W(0X10, ctx->r29) = (int32_t)bar_b; MEM_W(0X14, ctx->r29) = (int32_t)bar_n; MEM_W(0X18, ctx->r29) = (int32_t)bar_f;\n    }'
for f in "$RF"/*.c; do
    if grep -q 'RECOMP_FUNC void func_uvfmtx_rom_00401F74(' "$f" && ! grep -q 'bar_frustum_adjust' "$f"; then
        awk -v fix="$frustum_hook" '
            { print }
            /RECOMP_FUNC void func_uvfmtx_rom_00401F74\(/ { armed = 1; next }
            armed && /^[[:space:]]*int c1cs = 0;[[:space:]]*$/ { print fix; armed = 0 }
        ' "$f" > "$f.tmp" && mv "$f.tmp" "$f"
    fi
done
# Two separate lines must mention it: the declaration and the call. One line means the hook collapsed
# and is sitting inside a comment.
require "$(count_in_funcs 'bar_frustum_adjust')" 2 \
    "frustum hook (draw distance + culling width)" "RECOMP_FUNC void func_uvfmtx_rom_00401F74("
require "$(count_in_funcs 'bar_frustum_adjust(rdram, (unsigned)ctx->r4')" 1 \
    "frustum hook call is live (not collapsed into its comment)" "bar_frustum_adjust(rdram, ..."

# ---------------------------------------------------------------------------------------------
if [ "$FAILURES" -ne 0 ]; then
    echo "" >&2
    echo "fix-recompiled: $FAILURES rule(s) failed verification." >&2
    if [ "$STRICT" != "0" ]; then
        echo "                Refusing to leave a half-patched tree — the resulting build would run" >&2
        echo "                but misbehave in ways that look nothing like a missing patch." >&2
        echo "                Set FIXRECOMP_STRICT=0 to continue anyway (regeneration only)." >&2
        exit 1
    fi
    echo "                FIXRECOMP_STRICT=0 set — continuing with a half-patched tree." >&2
fi
echo "fix-recompiled: all rules verified."
