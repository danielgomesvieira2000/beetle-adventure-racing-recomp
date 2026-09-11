# Contributing

This fork **welcomes both human and AI‑assisted contributions.** Our position is simple and
pragmatic: matching decompilation has an **objective, machine‑checkable success criterion** — the
code either compiles to the exact original bytes or it doesn't. That verification gate is the same
no matter who or what wrote the code, so we use every tool available, including LLMs, and then we
**verify and review**.

> *Note: the upstream project prohibits AI‑generated contributions. This is an independent fork with
> a different policy; we do not submit anything upstream. Respect upstream's rules in their repo.*

## The one rule that matters: it must match

A contribution is accepted only if, after your change, the build still reproduces the original ROM
byte‑for‑byte:

```bash
source ./.venv/bin/activate
scripts/check.sh            # must end in: build/beetleadventurerac.us.z64: OK
```

A "match" that changes the output bytes is not a match. Beyond byte‑exactness we also ask that the C
be **plausible** — something an N64‑era engineer might have written (simple control flow, sensible
types), not obfuscated code that only happens to hit the right bytes.

## And it must be clean and readable

Byte-exactness is the gate; **readable, idiomatic C is also required.** The goal is source that looks
like what a 1999 Paradigm engineer would have written, not a byte-matching puzzle:

- **Named structs and typed pointers over raw casts** once an object's shape is known
  (`obj->velocity` rather than `*(s32*)((u8*)p + 0x2C)`). Build the struct up as a module's accessors
  reveal its fields.
- **Register-width parameters** (`s32`/`u32`/`void*`) with narrow casts at the point of use (also
  required for matching — narrow-type params make IDO spill the argument).
- **Meaningful names** where behavior is clear; otherwise keep the `func_`/`D_` symbol and add a
  comment. Naming/typing never changes output bytes, so improving it is always safe (re-verify anyway).
- **Simple, idiomatic control flow** — no gratuitous gotos or dead variables that only exist to hit bytes.

Raw casts are an acceptable *interim* for a first match, but prefer clean structs in the same pass when
the shape is obvious. The Opus review pass checks every match for this.

## Model‑tiered AI workflow (how we use LLMs)

We deliberately use the **cheapest model that can do each job**, then audit with a stronger one. The
byte‑exact diff is the floor; model review raises the ceiling on quality. Full details in
[`docs/AI_WORKFLOW.md`](docs/AI_WORKFLOW.md). In short:

| Stage | Model tier | Purpose |
|---|---|---|
| **Write** (easy/medium functions) | **Haiku 4.5 / Sonnet 4.6** | First‑pass match using m2c output + the target asm. Cheap, fast, high‑volume. |
| **Hard functions** | **Opus 4.8+** | Float‑heavy, large, or register‑allocation‑sensitive functions the smaller models can't land. |
| **Review / audit** | **Opus 4.8+** | After a smaller model produces a byte‑match, audit for correctness, naming, types, and "does this read like real code." |

Empirically, a mid‑tier model (Sonnet 4.6) lands ~74% of functions in this pipeline; reserve Opus
for the hard tail and the review pass. Never let an unverified match merge — the diff gate is
mandatory regardless of tier.

## Workflow for a single function

1. **Pick a target** — `python3 score_functions.py` ranks the easiest remaining functions. Claim it
   (open an issue or note it) so two people/agents don't duplicate.
2. **Seed** — run `m2c` on the target for a first‑pass C approximation.
3. **Match** — write/refine C; diff against the target until it's identical:
   ```bash
   ./diff.py -mwo <FunctionName>      # asm-differ; or use objdiff (see AI_WORKFLOW.md)
   ```
   Use `decomp-permuter` for stubborn register/stack‑slot differences.
4. **Verify** — `scripts/check.sh --module <module>` (fast) then a full `scripts/check.sh`.
5. **Review** — have the audit tier check the diff for correctness and quality.
6. **Name & type** — replace `func_XXXXXX`/`D_XXXXXX` with meaningful names; add struct/enum types.
   Naming and comments never change output bytes, so they're always safe (re‑verify anyway).
7. **Commit** — small, focused commits referencing the function/module.

## Code style
- Run `clang-format` (config in repo) before committing.
- C89/C90‑era idioms to match IDO output; avoid C99‑only constructs unless they match.
- Prefer real types over `s32`/`void*` placeholders once a struct's shape is known — **but** changing
  a type that alters `sizeof`/alignment/signedness can break matching, so re‑verify after type changes.

## What's safe vs. risky for matching
- ✅ **Always safe:** renaming symbols/locals, comments, splitting files, formatting.
- ⚠️ **Can break the build:** changing types/struct layout/array sizes/signedness, reordering fields,
  anything that affects codegen. Always re‑run `scripts/check.sh` after these.

## Licensing
By contributing you agree your work is licensed under **AGPL‑3.0** (this fork's license, inherited
from upstream). Don't paste code from incompatible sources.

## Knowledge base, comments & the scribe loop (required)

Decompilation is a shared, compounding effort -- see [`docs/KNOWLEDGE_BASE.md`](docs/KNOWLEDGE_BASE.md).
Three rules apply to every contribution (human or agent):

1. **Read shared context first.** Before decompiling, read `include/structs.h`, `include/functions.h`,
   `include/global_exports.h`, and the module's header. **Use** those types/prototypes; do **not**
   invent a `typedef` or redeclare a global that already exists (it conflicts and breaks matching).
2. **Write discoveries back (scribe).** When you confirm a new struct/signature/export member/global
   type and the build still matches, add it to the right header (`structs.h`, `functions.h`, the
   module header) or note a convention in `docs/SYMBOLS.md`, so the next function is easier.
3. **Comment the code.** A brief `//` purpose comment per function, plus inline comments for
   non-obvious offsets/magic/fields (style: `void (*unk4)(s32); // fontSet`). **ASCII only** -- the
   asm-processor uses euc-jp, so non-ASCII breaks the build.
