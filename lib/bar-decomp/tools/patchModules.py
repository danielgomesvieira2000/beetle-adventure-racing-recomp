import subprocess
import sys
import os
from pathlib import Path

PATCHED_MODULE_FILES_DIR = "patched_modules"

moduleFiles = [
    "ai.uvmo",
    "aiedit.uvmo",
    "airec.uvmo",
    "battle.uvmo",
    "bubble.uvmo",
    "cam.uvmo",
    "caranim.uvmo",
    "caraudio.uvmo",
    "cbars.uvmo",
    "dledit.uvmo",
    "demo.uvmo",
    "env.uvmo",
    "envsnd.uvmo",
    "expl.uvmo",
    "filerom.uvmo",
    "fileux.uvmo",
    "filmroll.uvmo",
    "flag.uvmo",
    "game.uvmo",
    "gamegui.uvmo",
    "glare.uvmo",
    "intro.uvmo",
    "letter.uvmo",
    "level1.uvmo",
    "light.uvmo",
    "logo.uvmo", # OK
    "med.uvmo",
    "menuslct.uvmo",
    "misc.uvmo",
    "motion.uvmo",
    "mult1.uvmo",
    "mult2.uvmo",
    "mult3.uvmo",
    "mult4.uvmo",
    "mult5.uvmo",
    "mult6.uvmo",
    "mult7.uvmo",
    "mult8.uvmo",
    "mult9.uvmo",
    "pause.uvmo",
    "piece.uvmo",
    "plyr.uvmo",
    "powerup.uvmo",
    "profgame.uvmo",
    "race.uvmo",
    "rain.uvmo",
    "replay.uvmo",
    "results.uvmo",
    "ripple.uvmo",
    "rumble.uvmo",
    "scene.uvmo",
    "scrn.uvmo",
    "selection.uvmo",
    "shadow.uvmo",
    "skid.uvmo",
    "smack.uvmo",
    "snd.uvmo",
    "sparks.uvmo",
    "splash.uvmo",
    "spray.uvmo",
    "tdata.uvmo",
    "ted.uvmo",
    "track1.uvmo",
    "track2.uvmo",
    "track3.uvmo",
    "track4.uvmo",
    "track5.uvmo",
    "track6.uvmo",
    "track7.uvmo",
    "triganim.uvmo",
    "txtview.uvmo",
    "ufo.uvmo",
    "ufogame.uvmo",
    "voltest.uvmo",
    "weapon.uvmo",
    "uvaudiomgr_rom.uvmo",
    "uvbill_rom.uvmo",
    "uvblit_rom.uvmo",
    "uvcback_rom.uvmo",
    "uvcmidi_rom.uvmo",
    "uvchannel_rom.uvmo",
    "uvcolor_rom.uvmo",
    "uvcont_rom.uvmo",
    "uvcontourld_rom.uvmo",
    "uvdgeom_rom.uvmo",
    "uvdbg_rom.uvmo",
    "uvdobj_rom.uvmo",
    "uvdsetld_rom.uvmo",
    "uvdyn_rom.uvmo",
    "uvear_rom.uvmo",
    "uvemitter_rom.uvmo",
    "uvenv_rom.uvmo",
    "uvenvld_rom.uvmo",
    "uvfmtx_rom.uvmo",
    "uvfvec_rom.uvmo",
    "uvfont_rom.uvmo",
    "uvfontld_rom.uvmo",
    "uvfx_rom.uvmo",
    "uvgeom_rom.uvmo",
    "uvgfxmgr_rom.uvmo",
    "uvgfxstate_rom.uvmo",
    "uvgrph_rom.uvmo",
    "uvgui_rom.uvmo",
    "uvimtx_rom.uvmo",
    "uvintersect_rom.uvmo",
    "uvisect_rom.uvmo",
    "uvjanim_rom.uvmo",
    "uvjanimld_rom.uvmo",
    "uvled_rom.uvmo",
    "uvlight_rom.uvmo",
    "uvmath_rom.uvmo",
    "uvmodel_rom.uvmo",
    "uvmodelld_rom.uvmo",
    "uvpfx_rom.uvmo",
    "uvpfxld_rom.uvmo",
    "uvquat_rom.uvmo",
    "uvquery_rom.uvmo",
    "uvsort_rom.uvmo",
    "uvsprt_rom.uvmo",
    "uvstring_rom.uvmo",
    "uvtexanim_rom.uvmo",
    "uvtseq_rom.uvmo",
    "uvtseqld_rom.uvmo",
    "uvterra_rom.uvmo",
    "uvterrald_rom.uvmo",
    "uvtexture_rom.uvmo",
    "uvtexturexref_rom.uvmo",
    "uvtextureld_rom.uvmo",
    "uvtrackld_rom.uvmo",
    "uvufileld_rom.uvmo",
    "uvvattr_rom.uvmo",
    "uvvolumeld_rom.uvmo",
    "victory.uvmo",
]

def makePatchedModuleFilesDir():
    Path(PATCHED_MODULE_FILES_DIR).mkdir(exist_ok=True)


def genPatchedFiles(genSymsAndRelocs):
    cmd = ["tools/moduleTool"]

    args = []
    for module in moduleFiles:
        print("Processing:", module)
        args.append("-a")
        args.append("bin/us/" + module)

        if (genSymsAndRelocs):
            args.append("-s")
            args.append("-w")

        args.append("-o")
        args.append(PATCHED_MODULE_FILES_DIR + "/" + module)
        subprocess.run(
            cmd + args,
            check=True,
        )
        args = []


def genRelocsSorts():
    for module in moduleFiles:
        print("Generating relocs sorts for:", module)
        subprocess.run(
            [   
                "./tools/genModuleRelocsSort",
                "bin/us/" + module,
            ],
            check=True,
        )

def genJtblSyms():
    for module in moduleFiles:
        try:
            subprocess.run(
                [   
                    sys.executable,
                    "./tools/genJtblSyms.py",
                    "asm/us/data/modules/" + module.split('.bin')[0] + ".rodata.s",
                ],
                check=True,
            )
        except subprocess.CalledProcessError as e:
            print("This module doesn't have rodata!")


createSymbolAddrsAndRelocAddrs = False # Don't set this to true if you don't know what you're doing

makePatchedModuleFilesDir()
genPatchedFiles(createSymbolAddrsAndRelocAddrs)

if (createSymbolAddrsAndRelocAddrs):
    genRelocsSorts()
    genJtblSyms()
