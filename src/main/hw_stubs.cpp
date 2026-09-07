// hw_stubs.cpp — native stubs for libultra functions that poke RCP hardware registers directly.
//
// A handful of low-level libultra functions were recompiled raw (the decomp didn't symbol-match
// them, so they aren't in beetle-adventure-racing-recomp.toml `ignored`/`stubs`). They MEM_W/MEM_B KSEG1 register
// addresses (0xA4xxxxxx — AI/PI/SP), which aren't memory-mapped in the recomp and fault out of
// bounds. scripts/fix-recompiled.sh renames each generated definition to <name>__hwstub_orig so the
// wrapper below owns the symbol all callers bind to.
//
// Proper long-term fix: add these to beetle-adventure-racing-recomp.toml `stubs` (or instruction-patch the offending
// hardware accesses) and regenerate with N64Recomp. Stubbed here for bring-up.
#include "recomp.h"
#include <ultramodern/ultramodern.hpp>   // ultramodern::get_remaining_audio_bytes

// func_8000E460: osAiGetLength() — reads AI_LEN_REG (0xA4500004), the remaining audio DMA length (bytes).
// Audio IS wired now, so report the REAL queued audio length. When this returned 0, the audio manager
// (uvaudiomgr_rom.c) always believed the AI buffer was empty and synthesized the MAXIMUM samples every
// video frame; the SDL queue then filled far faster than it drained, so the sound lagged further and
// further behind the game (an ever-growing delay). get_remaining_audio_bytes() returns the queued audio
// in AI_LEN_REG byte units (queued_frames * 4); the game does >>2 to recover frames, so units match and
// the generator self-stabilizes to a bounded ~one-VI queue.
extern "C" void func_8000E460(uint8_t* rdram, recomp_context* ctx) {
    (void)rdram;
    ctx->r2 = ultramodern::get_remaining_audio_bytes();
}
