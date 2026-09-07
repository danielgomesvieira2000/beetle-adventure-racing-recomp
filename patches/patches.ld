/* Linker script for the recomp patches (C compiled to MIPS, see patches/Makefile).
 *
 * The load address must not collide with anything the game itself occupies, because
 * patch symbols keep these addresses when the recompiler resolves relocations:
 *
 *   0x80000400          game entrypoint (beetle-adventure-racing-recomp.toml `entrypoint`)
 *   0x80800000 upwards  the ~133 relocatable UV overlay modules
 *
 * 0x80000000 -- which this script used to specify -- sits 1 KiB below the entrypoint,
 * so .text grew straight into the game's own code. 0x8F000000 is still KSEG0 and is
 * clear of both the game image and the module window.
 */
SECTIONS {
    .text 0x8F000000 : { *(.text) *(.text.*) }
    .data            : { *(.data) *(.data.*) }
    .rodata          : { *(.rodata) *(.rodata.*) }
    .bss             : { *(.bss) *(.bss.*) *(COMMON) }

    /DISCARD/ : { *(.reginfo) *(.MIPS.abiflags) *(.pdr) *(.comment) *(.gnu.attributes) }
}
