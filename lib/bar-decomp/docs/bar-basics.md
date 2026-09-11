The following is a summary of how the Beetle Adventure Racing game engine works, and how it's related to several other titles released for the N64.

## History of UltraVision

BAR was co-developed by Paradigm Entertainment, Inc. (PEI) and Electronic Arts (EA).  Development on the game was [first reported in magazines around April 1998](https://archive.org/details/vsixtyfour-issue-01-april-1998/page/n7/mode/2up?q=%22NEED+FOR+SPEED+64%22) and was originally going to be an N64-exclusive entry in EA's Need for Speed franchise.  

BAR uses an engine developed by Paradigm known as UltraVision.  This is the same engine used previously in other N64 games developed by Paradigm, starting with Pilotwings 64.

Aero Fighters Assault and F-1 World Grand Prix would be the next titles using UltraVision but F-1 World Grand Prix introduced an update to the file system that allows relocatable code overlays to be loaded into memory.  BAR became the next game to use this updated version of UltraVision. 

## UV Engine Structure

### Pilotwings 64

In Pilotwings 64, the UltraVision engine is divided into three sections: the kernel, the app, and the filesystem.  The kernel handles low-level functions like controller input, drawing graphics on-screen, initializing sound, switching game states, etc.  The app contains all the higher-level gameplay functions, and the filesystem is where all the game assets are stored.  No relocatable code is used so all the code is static.

### BAR

By the time BAR was in development, the UltraVision engine was updated to make better use of the N64's limited memory.  This new memory-optimized version can load specific code overlays upon engine initialization, allowing the UV engine to swap them out as needed during gameplay.  We refer to these overlays as modules.  The static code for the kernel and app sections were converted to relocatable modules and added to the filesystem as `UVMO` files.

While these overlays make it easier for the game to load larger 3D levels and optimizes memory management, this complicates decompiling efforts because now we need to compensate for all the possible relocatable addresses that the modules could possibly use.  

The bright side is that BAR's modules are uncompressed, thanks to the game's large ROM size.  Other titles from Paradigm (like F-1 World Grand Prix) use compressed modules, which makes it more difficult to decomp and rebuild a bit-matching ROM without understanding how the compression algorithm works and making tools to accommodate for that extra step. 

## Module Exports

To manage all 133 modules, the engine uses a section of static code that acts as a "docking bay" for all the required modules needed to make the game run.

Starting at address `80025BD8`, we have a large group of pointers that lead us to the relocatable modules.  Each kernel module starts with a `Uv` prefix.  Pointers starting after `80025C74` are related to BAR/gameplay-specific modules.

Module Pointer | Address
----------|------------
gUvAudiomgrExports | 80025BD8
gUvChanExports | 80025BDC
gUvCbckExports | 80025BE0
gUvCmidiExports | 80025BE4
gUvContExports | 80025BE8
gUvDobjExports | 80025BEC
gUvEarExports | 80025BF0
gUvEmitterExports | 80025BF4
gUvEnvExports | 80025BF8
gUvFmtxExports | 80025BFC
gUvFontExports | 80025C00
gUvFvecExports | 80025C04
gUvGfxMgrExports | 80025C08
gUvImtxExports | 80025C0C
gUvModelExports | 80025C10
gUvSprtExports | 80025C14
gUvStringExports | 80025C18
gUvTerraExports | 80025C1C
gUvTextureExports | 80025C20
gUvJanimExports | 80025C24
gUvIntersectExports | 80025C28
gUvMathExports | 80025C2C
unused? | 80025C30
gUvGeomExports | 80025C34
gUvDGeomExports | 80025C38
gUvTseqExports | 80025C3C
gUvFxExports | 80025C40
gUvGfxStateExports | 80025C44
gUvTexAnimExports | 80025C48
gUvSortExports | 80025C4C
gUvLightExports | 80025C50
gUvDynExports | 80025C54
gUvDebugExports | 80025C58
gUvQuatExports | 80025C5C
gUvQueryExports | 80025C60
gUvGuiExports | 80025C64
gUvGrphExports | 80025C68
gUvIsectExports | 80025C6C
gUvPfxExports | 80025C70
gGameGuiExports | 80025C74
gSndExports | 80025C78
gCamExports | 80025C7C
gMiscExports | 80025C80
gSceneExports | 80025C84
gScrnExports | 80025C88
gLightExports | 80025C8C
gReplayExports | 80025C90
gGlareExports | 80025C94
gSkidExports | 80025C98
gSprayExports | 80025C9C
gSparksExports | 80025CA0
gSmackExports | 80025CA4
gPieceExports | 80025CA8
gAiExports | 80025CAC
gTdataExports | 80025CB0
gRainExports | 80025CB4
gEnvExports | 80025CB8
gRippleExports | 80025CBC
gSplashExports | 80025CC0
gCarAudioExports | 80025CC4
gEnvSndExports | 80025CC8
gFlagExports | 80025CCC
gVolTestExports | 80025CD4
gBattleExports | 80025CD8
gPowerupExports | 80025CDC
gLetterExports | 80025CE0
gWeaponExports | 80025CE4
gRumbleExports | 80025CE8

If we follow one of these global pointers, it will take us to a jump table (so basically more pointers) for all of the functions that belong to that particular module.  We refer to this as the "exports" for a relocatable module.

Each module source file has a special `__entrypoint` function and an exports struct containing all the pointers to every normal function in that module.  Note that the entrypoint is not included in this struct.  

```
// An example of a simple exports struct.  There are more functions inside "intro.c" but only three are exported.
typedef struct Intro_Exports_s {
    /* 0x00 */ void (*func_intro_004004F0)(void);
    /* 0x04 */ void (*func_intro_004005CC)(void);
    /* 0x08 */ void (*func_intro_00400820)(void);
} Intro_Exports;

// Entrypoint function example.  The pointer for the module's exports is allocated, then the jumptable is populated
// with pointers to the exported functions.
void __entrypoint_func_intro_400000(Intro_Exports *arg0) {
    uvUpdateFileAllocPtr(arg0);
    arg0->func_intro_004004F0 = func_intro_004004F0;
    arg0->func_intro_004005CC = &func_intro_004005CC;
    arg0->func_intro_00400820 = func_intro_00400820;
```

When the entrypoint function is called, it initializes the module by assigning it a global module pointer via the `uvUpdateFileAllocPtr()` function.  Then, the executable code and the export pointers are written to memory, creating the jump table that allows the game to call the relocatable code.  

The easiest way to think of how module functions are accessed is through nested pointers.  Follow the global exports pointer first, pick which function you want in the exports struct and then follow that next function pointer:

`gExampleExportsPointer -> exampleFunctionPointer`

For example, to change the color of a font that's going to be displayed on-screen, we would do something like this:

`gUvFontExports->uvFontColor(0, 100, 200, 255);`

The line of code above uses the global exports pointer for `uvfont_rom` to call the `uvFontColor` function from that module.  This way of calling functions is used extensively through out the codebase so it's important to have a good understanding of how this works.

## FORM and Tags

The game assets and modules are stored in a file system that uses the FourCC header `FORM` for every single file in the game.  Pilotwings 64 and Aero Fighters Assault don't use a file table to look up the hundreds of assets in the ROM, but this was changed with F-1 World Grand Prix.  

Bloodangel from the PW64-decomp team recognized that this file structure is an implementation of the [Interchange File Format](https://en.wikipedia.org/wiki/Interchange_File_Format).  IFF has formed (pun intended) the basis for many other file formats that are commonly used today like .WAV, .AVI, and .AIFF.  In an interesting coincidence, IFF was co-developed by BAR's publisher EA back in 1985.

Each file has a FourCC "tag" assigned to it.  A majority of these tags start with "UV", followed by a two-letter suffix.  For example, `UVMO` stands for "**U**ltra**V**ision **Mo**dule" file.  You can think of this as the file's extension.

To make it easier to find assets, a table of contents is placed at the start of the file system that we refer to as the `filetable`.  This file table denotes the file's tag, along with the relative offset to the file's address.  To get the correct address for a file, simply add the address at the very end of the `filetable` (for the NTSC release, it's `0x25FD0`) with the offset of that file.  

Confusingly, the font files and the `filetable` appear to use the same file tag `UVFT`.  So depending on where it's used `UVFT` can either mean "**U**ltra**V**ision **F**on**t**" or "**U**ltra**V**ision **F**ile **T**able".
