# Working in this repository

A static-recompilation PC port of *Beetle Adventure Racing!* (N64, USA). Read
[`docs/technical/README.md`](docs/technical/README.md) before making a change to anything you have
not touched before — it is the reference for how the game, the recompiler, the runtime and the
renderer actually fit together, and it will save you rediscovering things that are already written
down.

## The documentation contract

`docs/technical/` is a published technical reference, aimed at people who want to decompile this
game or build their own port. Its value is that it describes the build that exists.

**Update the affected chapter in the same change that makes it wrong.** Not in a follow-up commit.
`docs/technical/README.md` carries the table mapping each source area to the chapter that owns it.
Three conventions apply to everything written there:

1. **Record the measurement, not the conclusion alone.** Name the switch someone can flip to
   re-derive a number.
2. **Keep negative results.** A refuted hypothesis that gets deleted is one someone will pay for
   again.
3. **Mark inference as inference.** Several claims are derived from unmatched assembly. Do not
   launder one into a fact by rewriting the sentence.

Working notes — current state, roadmap, live bugs — live in `docs/STATUS.md`, `docs/TODO.md`,
`docs/KNOWN_ISSUES.md`, `docs/PORT-STATUS.md` and `docs/PINNED_REVISIONS.md`. Those are
chronological and separate from the reference.

## Hard rules

* **Never commit game data.** No ROM, no ELF, no extracted asset. `.gitignore` blocks the obvious
  extensions; do not work around it.
* **Never edit `RecompiledFuncs/` or `RecompiledPatches/` by hand.** They are generated and
  git-ignored. Changes go in `scripts/fix-recompiled.sh` (address-anchored) or `patches/`
  (name-anchored, and strictly preferred).
* **Never change the decomp** (`lib/bar-decomp`). It is a faithful mirror of the original;
  modifications belong in this repo's `patches/` and `src/` layers.
* **`scripts/fix-recompiled.sh` is mandatory** after every `./N64Recomp` run, and every rule in it
  verifies its own end state. If it fails, an anchor rotted — re-find it, do not delete the rule.

## How to work

**Land one verified change at a time.** Fix the baseline, confirm it is actually right, and only
then add the layer on top. This matters more than usual here because several mechanisms affect the
same pixels — the aspect-ratio option, the present fill mode, the game's own viewport inset and the
renderer's window fit — and when more than one changes at once, a screenshot cannot say which is
responsible.

**Verify by running the game, not only by capturing a frame.** Several defects here were only ever
visible in motion. `docs/technical/08-diagnostics-and-methodology.md` documents the scripted-input
and internal-render screenshot tooling; use it rather than window-manager screenshots.

**Timebox investigations.** When one does not converge after a few measurements, write down what was
measured and check in before starting another round of probes.
