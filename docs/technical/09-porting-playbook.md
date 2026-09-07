# 09 — Porting playbook

Generalised from this port: the order to do the work in, the traps that cost the most, and what to
measure before believing anything. Written for someone starting a static-recompilation port of an N64
title — this one or another.

## Before you start: is the game a good candidate?

Check these five things. Each one that goes the wrong way adds weeks.

| Question | Good answer | Why |
|---|---|---|
| What **graphics microcode**? | Stock F3DEX/F3DEX2 | A custom ucode means extending the renderer's HLE or running a recompiled ucode against an LLE path |
| Does it use the **TLB**? | No — flat KSEG0 | TLB-mapped code means address translation in the recompiled output |
| What **save medium**? | One, and a simple one | Each of EEPROM / SRAM / Flash / Controller Pak is separate work |
| Does it use the **high-level** libultra controller API? | Yes | If it reaches raw SI/PIF DMA, you implement joybus yourself ([05](05-runtime-host.md#input--the-low-level-sipif-path)) |
| Does a **decompilation** exist? | Yes, even an unfinished one | You need its ELF and module layout, not its C |

BAR answered well on four of five: stock F3DEX2, no TLB, Controller Pak only, an existing decomp —
and the low-level SI path, which was real work.

The thing that made it *hard* was not on this list: **~133 relocatable modules**. Check for an
overlay system before estimating.

## The order to do it in

**Get one thing verifiably right before layering the next on top.** Several of the hardest debugging
sessions in this project came from changing two things that affect the same pixels at once — the
aspect-ratio option, the present fill mode, the game's own viewport inset and the renderer's window
fit all move the same picture, and a screenshot cannot say which one is responsible. Landing them one
at a time keeps every result attributable.

1. **Decomp ELF and module layout.** Everything depends on it. Confirm the entry point two ways.
2. **Recompile and link.** Expect it not to compile: fix the codegen quirks
   ([07](07-codegen-fixups-and-patches.md#codegen-fixups)) and stub whatever will not link.
3. **First boot.** Overlay registration is where a module-based game will stop dead. Instrument
   module loads before you need to.
4. **Rendering.** Getting a picture at all, then getting it *correct* at the game's native aspect,
   then — separately, later — widescreen.
5. **Input.** Find out which API the game actually uses before implementing anything.
6. **Audio.** Verify the sample transform empirically. Do not assume a byteswap.
7. **Saves.**
8. **Frontend, settings, features.**

Only after each of those is signed off should enhancements land, and enhancements should sit behind a
switch that defaults to off until the base is verified.

## The traps, ranked by what they cost here

### 1. Silent failures that look like success

The dominant failure mode in this whole project. Ranked examples, all real:

* **A static library never contributed the patch.** The patch compiled, linked without a warning, and
  changed nothing, because a static library only contributes members for symbols still undefined when
  it is scanned, and the original had already been pulled in. Compile patches **directly into the
  executable**. ([07](07-codegen-fixups-and-patches.md#the-linker-trap--how-a-patch-silently-fails-to-replace-anything))
* **An injected hook collapsed into its own comment.** It compiled, the sentinel was present, the
  verification passed, the hook did nothing. A sentinel that can appear inside a comment is not a
  verification.
* **A patch's coordinate convention was inverted**, so half of it silently never matched.
* **A font family name was wrong.** Every element laid out and drew in the right place, with no text
  in any of them.
* **An address-anchored codegen rule stopped matching** after the ELF moved. Nothing failed; the game
  just misbehaved in an unrelated-looking way.
* **A hardware register stubbed to zero** removed the input to a control loop, so the audio generator
  ran open-loop and the sound drifted further behind every second.

**The countermeasure is to make every mechanical change assert its own end state**, and to prefer
bindings that cannot rot: a symbol name over an instruction address, a data address over a code
address, an exported getter over a baked constant.

### 2. Two mechanisms for the same thing

Every time this port found a defect that resisted explanation, it turned out that two things were
doing the same job and disagreeing:

* Two scissor systems in the game — the RDP scissor and the sprite layer's software scissor.
* Two coverage tests in the renderer — 80 % in the projection processor, 100 % in the framebuffer
  renderer. A projection one pixel narrow passed one and failed the other, and got a widened matrix
  inside a 4:3 viewport.
* Two writers of the same config file disagreeing about a field's default.
* Two SDL event pollers, each swallowing what the other needed.

**When you find one, look for the other.** And when you fix it, make them literally the same code
path, not two matched constants.

### 3. Cooperative scheduling has no preemption

A runtime that runs N64 threads cooperatively will starve the audio thread during any long
straight-line compute, because nothing yields. Real hardware preempted. The fix that works is a host
timer raising a flag plus a yield polled at every function prologue, gated to low-priority game
threads — cheap on the hot path, and safe because the yield only fires at a point the game thread
chose. Forcible `SuspendThread`-style preemption is not safe: it can resume the scheduler mid-mutation
of a lockless queue, deadlock on the heap lock, and tear RDRAM writes.
([05](05-runtime-host.md#cooperative-preemption))

Also watch for the inverse: **a game spin-loop waiting on another thread's state** will burn its full
timeout every time, because the other thread never gets to run.

### 4. Timing on the host

* **Raise the Windows timer resolution to 1 ms.** At the default ~15.6 ms, every timed wait quantises
  and a menu frame costs several quanta. This capped menus at ~10 fps here and was misread as jitter,
  because a race — which always has work queued and never waits — was unaffected.
* **Watch for queue-bounce busy-spins.** A game event queue of capacity 3 that the game does not
  drain, plus a runtime that re-queues on overflow, is a 15 M/sec spin that starves everything.
* **Separate the clocks before trying to fix a speed problem.** In this game, motion steps with the
  game loop, segment length runs on the wall clock, presentation runs on the VI, and the audio
  sequencer's tempo runs on the audio completion path. A lever that moves one of those moves neither
  of the others. Measure which clock the symptom is on before choosing a knob.

### 5. Display geometry

* **The game's own frustum is probably 4:3, and it culls against it.** Widening what the renderer
  draws without widening what the game submits produces pop-in at the new margins, not a wider view.
  Find the one function that builds projections; it is likely also the source of the culling planes.
* **Overscan insets are real.** A game authored for a CRT may deliberately draw into a smaller
  rectangle. Fix that at the source rather than compensating in the renderer, because the inset also
  breaks the renderer's "does this reach the edges" tests.
* **2D HUD elements need origins the game never emitted.** Anchoring is a translation, the scissor has
  to travel with the element, and there is usually at least one element that is geometry rather than a
  texture rectangle.
* **Knowing "is the HUD on screen" is a game-state problem, not a rendering one.** Only the host can
  read it out of RDRAM, and the obvious state value is usually too coarse. Look for the grouping the
  game's own code uses.

### 6. DPI, on Windows

Set process DPI **awareness** before `SDL_Init`, and leave DPI **scaling** off. Awareness keeps both
geometry and mouse positions in physical pixels; scaling puts them in different units, which is a
click landing a scale factor away from where the button drew.

## What to build early, before you need it

The tooling in [08](08-diagnostics-and-methodology.md) paid for itself many times over, and all of it
would have been cheaper to build before the investigations that needed it:

* **Env-gated tracing on every subsystem**, zero-cost when unset. Module loads, game state, loop rate,
  joybus traffic, every 2D rectangle, every projection.
* **Scripted input**, bypassing the focus gate, on a frame timeline.
* **Internal-render screenshot capture** on the same frame timeline, plus a burst mode for animations
  the input timeline cannot sample.
* **A crash-runner script** with the debug heap disabled.
* **A verifying post-processing step** for generated code, that fails the build rather than warning.

The single highest-value item is the pairing of scripted input with screenshots on **the same frame
counter**, because it turns "does this look right" into a reproducible command.

## What to write down

Findings, measurements and dead ends, as they are made. Negative results are the expensive part and
the first thing to be lost. This project's documentation carries eight refuted hypotheses for one
open issue specifically so nobody re-runs those experiments — that list is worth more than the
paragraph of conclusions above it.

Record the **switch that produced the number**, not just the number. A measurement someone can
re-derive survives a rebuild; a bare figure does not.
