# Knowledge base & the scribe loop

Decompilation gets *easier over time* when agents share a growing, machine-readable understanding of
the code instead of re-deriving it from assembly every function. This is our most important
reliability lever for the logic/float tail: agents that READ the shared types/signatures stop
guessing (and guessing wrong -> compile errors and mismatches, which is exactly how the first logic
wave failed). The knowledge base lives IN THE REPO, so it travels (unlike local agent memory) and is
both documentation AND compilable scaffolding.

## Where knowledge lives (READ these before decompiling)
| File | Holds |
|---|---|
| `include/structs.h` | shared / export struct types, with NAMED members + `// purpose` comments |
| `include/functions.h` | function prototypes (correct signatures, so calls match) |
| `include/global_exports.h` | the typed export globals (`gUvFmtxExports`, `gAiExports`, ...) |
| `include/<module>.h` | types/prototypes specific to one module |
| `docs/SYMBOLS.md` | human index: naming conventions, notable globals, module roles, gotchas |

These already exist and are under-used. Today most modules only `#include "common.h"` and redeclare
things locally. The new rule: use the shared headers.

## The three rules

### 1. READ before you write
Before decompiling, read `structs.h`, `functions.h`, `global_exports.h`, and the module's own header.
USE the types/prototypes you find. Do NOT invent a `typedef` or redeclare a global that already
exists there (a local `extern void* gXExports;` CONFLICTS with the typed declaration and breaks
`->member` access). If a type you need isn't there yet, add it (rule 2) rather than duplicating it.

### 2. WRITE discoveries back (the scribe step)
When you CONFIRM something new (build still matches), add it to the right header so the next agent
benefits:
- new shared/export struct -> `include/structs.h`
- new function signature -> `include/functions.h` (or the per-module header)
- new export member at an offset -> `include/structs.h` / `include/global_exports.h`
- a convention, role, or gotcha -> `docs/SYMBOLS.md`

Only write VERIFIED facts. This is what makes later functions EASIER instead of harder -- each
confirmed type/signature resolves cross-references for every future function that touches it.

### 3. COMMENT the code
Every decompiled function gets a brief `//` comment on what it does (when the behavior is clear).
Add inline comments for non-obvious offsets, magic numbers, and inferred field/global meanings --
mirroring the existing `structs.h` style: `void (*unk4)(s32); // fontSet`. Comments MUST be pure
ASCII (the asm-processor encodes source as euc-jp; a non-ASCII char breaks the build).

## Why this compounds
Match rate should CLIMB across the project rather than fall on the hard tail, because the shared
context grows with every batch. See `AI_WORKFLOW.md` for how READ -> decompile+comment -> SCRIBE
folds into the write -> Opus-review -> integrate pipeline.
