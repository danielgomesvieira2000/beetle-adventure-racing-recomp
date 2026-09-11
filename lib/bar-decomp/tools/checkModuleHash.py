import hashlib
import sys
import random

# ANSI color codes and styles
COLOR_RED = '\033[91m'
COLOR_GREEN = '\033[92m'
COLOR_YELLOW = '\033[93m'
COLOR_CYAN = '\033[96m'
COLOR_BOLD = '\033[1m'
COLOR_DIM = '\033[2m'
COLOR_RESET = '\033[0m'

# Icons
CHECK_MARK = '✓'
CROSS_MARK = '✗'
INFO_MARK = 'ℹ'
WARN_MARK = '⚠'

BarModuleFiles = {
    "partial_ai.o.generated.uvmo": "12d1b6f41d3af24489af9d3f42f5bdc34265203ef73393ebe6869ea34cb74749",
    "partial_aiedit.o.generated.uvmo": "a071cea89e0b1a04e670e2f6e6bbe38c829e0b23b87e66fb1c889648aee663f4",
    "partial_airec.o.generated.uvmo": "4c4bbba8c4bae2032904cd2c25719d49083a5de73bb1745c76e5ca662ea8a5f7",
    "partial_battle.o.generated.uvmo": "d316030f99bac7b7b2a502ea0f8d4ccfffac0e236c73639e553546ee91747c66",
    "partial_bubble.o.generated.uvmo": "02092e2fc33bc0493f5d05b5b38485a0f8daefe7ef5e46b6c57135a2f2fd60be",
    "partial_cam.o.generated.uvmo": "46885b6cea193286b15341603a7c2c95adcf5c38b68bc32208b3ec7e3031ecd3",
    "partial_caranim.o.generated.uvmo": "77004883b3ae031b1789573d67a6472cdb4a32a2e42a3b6b11740eb1222a682f",
    "partial_caraudio.o.generated.uvmo": "82f98f2deb85dce3223b75da158c1587b26e61da95da41232651a911b00fd212",
    "partial_cbars.o.generated.uvmo": "ad37366aa7f619374ac48f4c824748ad6f0404210114f393feb3e29e806fc5d5",
    "partial_demo.o.generated.uvmo": "3ee5fe9f499649408e00711a2da5e1c7c54804822c14e9624d89b8c27470c858",
    "partial_dledit.o.generated.uvmo": "6c16f588f366354d0a2c9f0745e802187fbed8c6c1cdf43d3b5af5202d11f8a2",
    "partial_env.o.generated.uvmo": "5dafd8ba7abe15307237ef8739900d50dadfd25a94e09115028067c5b81a93d7",
    "partial_envsnd.o.generated.uvmo": "ac750ab0b51f34b7d72fb2fe079513fafea5b2f7e7ac56f0e4bfae9580373b83",
    "partial_expl.o.generated.uvmo": "9fc1f7b63d8f711f81a9ef09365fc839b3dc6c1c25190d068c3650d3f4bdbddc",
    "partial_filerom.o.generated.uvmo": "2a6857990587218eaded90fdf72704cfb3076db77b41913d205babe6aac0d5b6",
    "partial_fileux.o.generated.uvmo": "72a05178e4a4af19d150cc66a2137355cb59b79ac31765d7ce302a9efe7970a7",
    "partial_filmroll.o.generated.uvmo": "bf91bf6b68a1a36336d8479be23c8f6c86a7023679994b85aab482909bbfc54b",
    "partial_flag.o.generated.uvmo": "5e19a4f825481421e867cb0647d2208cc1c5a3ab2a729c469f499decf59ab6f5",
    "partial_gamegui.o.generated.uvmo": "79579e551b46ecb91f27801ea718b81faa37c5fe6b2ac67e2bc8df62caa52bb6",
    "partial_game.o.generated.uvmo": "6b53c4ae6e7d2548fa2abe076f2981943273b119383385e9b9bf985f3cfb06d7",
    "partial_glare.o.generated.uvmo": "393bddef2bac2a9cedf99d43ff41c191506662943f56de91bae14b9bb3bf6ddf",
    "partial_intro.o.generated.uvmo": "400def7266cb61c778154cbf8937648508bef74e5f9d0c48a3ec9251e8120fa5",
    "partial_letter.o.generated.uvmo": "31360164bb8c053b3b64792edf448a9e4317c23e6b74cc38e886e9ffd4297374",
    "partial_level1.o.generated.uvmo": "2c67f065e95c565c280887b8c8651fb98d9ce31f0dff18ca85a5f763e7c2d0d1",
    "partial_light.o.generated.uvmo": "61108f031f3e507027331ecb18b5739f78189d6e78362aea08b2f9a837ecc1c9",
    "partial_logo.o.generated.uvmo": "b7740214125f1cdacc98528eb032c8c858d96dcfc6510ea84a2f65f100b87495",
    "partial_med.o.generated.uvmo": "6131e17c973534990e2c7f36e1e3bd41b1e39a68f4804c868d59739c1bb62643",
    "partial_menuslct.o.generated.uvmo": "a13de4943e186b3182e084245d91d05c83fd3c3d3087b65b09593c01112af1d1",
    "partial_misc.o.generated.uvmo":  "5860afcbb240b66074f15fa58e11822aba8e36fa8d3b541e8194349014aef0ec",
    "partial_motion.o.generated.uvmo": "c5f6805cd2e6f355e8f6fb2707ad08414d8ef49fcab29e2790eab0ad2dc376ee",
    "partial_mult1.o.generated.uvmo": "c48b1118c8226978a6555604ac67e50d0211c3ce56613098e3cd626cbd8c00f4",
    "partial_mult2.o.generated.uvmo": "45d354763d9d979789128d780154db232a69667675b0d2128f6949fc821c5de1",
    "partial_mult3.o.generated.uvmo": "d75d460c1d691853ac91168abff93f3591726998126fcce052e12ed99f2f0a2f",
    "partial_mult4.o.generated.uvmo": "349017b838eb536b3da4e2323a58c3cc86cc5865e41e0a502cc83f887af55b5d",
    "partial_mult5.o.generated.uvmo": "396773f977e248356fd7094823bd7a48fc1ee56f2549e20bc9faad31d4a55b69",
    "partial_mult6.o.generated.uvmo": "079c8b1305a7053ad8efad8c38ef5f062778ce180feb577d32b7dd42fb974002",
    "partial_mult7.o.generated.uvmo": "f5dab17c25de3bb6027928d3bb5bcc25bad4d206a52bf012f3fe80e6614cc833",
    "partial_mult8.o.generated.uvmo": "df08088f3ea5c0bab96419274c4b6f81e3f09cda965527365a076a0d6ed7e265",
    "partial_mult9.o.generated.uvmo": "52614c4e7436c108b8b899bc95b24a64ce2434bd19d65d0ebcd9fa92a011f39b",
    "partial_pause.o.generated.uvmo": "01dff83be8b549559fadd408638ab3da12e5daf5488438da06ee4d15f34f1e9d",
    "partial_piece.o.generated.uvmo": "fd996a285b4005eb08fe97ede1b8f3628e6c2d4cf6b2b572ab817e17beccc5c0",
    "partial_plyr.o.generated.uvmo": "7b6aede819951ecbe0727c13ac795390eede40795e168bdfaed2fffd75fdc28f",
    "partial_powerup.o.generated.uvmo": "27c6a61b54b16d333a23353d6a3b85db1d9000e4dad0ea81f481d426f5fdc0a7",
    "partial_profgame.o.generated.uvmo": "ed65315c7ee969da7f2b4012a998c8e2428af91af600cc57207d2f5f8ab6bff2",
    "partial_race.o.generated.uvmo": "b0739fdbad2ffced725c489e79fa9be9459dbf86545445e0caf334b565fea261",
    "partial_rain.o.generated.uvmo": "7cd8a09cf2e6aff9afb6844002533f2a802a3a5a094f2574a9896e8a56581922",
    "partial_replay.o.generated.uvmo": "30ce0f69525c12f7bea90737312da21e9f46bc77607725587f8a6a526775c1b5",
    "partial_results.o.generated.uvmo": "9940182cf6f64c236dae489e218bbf99cdc2a0fbdd49142a098fe6d70aa26524",
    "partial_ripple.o.generated.uvmo": "a7ffdd8c01f80b2fa2366426e513e015cfb29b92463ef80de93e3f24d2df11c2",
    "partial_rumble.o.generated.uvmo": "bf73485f7c97273a4b7599d956feff38cfa23c99a740ae4712f4651d633c0571",
    "partial_scene.o.generated.uvmo": "888c8411956a5fde496c70787d39f9cc95885d6f4bd0e98039d808b08faf14c0",
    "partial_scrn.o.generated.uvmo": "9e5c27a4fbb5a2f30d1d84721c4b58925b97d124f85026793254d5d9e77e2755",
    "partial_selection.o.generated.uvmo": "12f427e15e36bedf1fa282d823768830429b54cfef0271ade7abc8449d5c5825",
    "partial_shadow.o.generated.uvmo": "8504461bca9379ec346237527a4845a7968320aff7ec38e749a09d7004c14209",
    "partial_skid.o.generated.uvmo": "672e29a23d641439f3feda8fbc8e06f3ffc47abb5bb46144be611806bfaafd69",
    "partial_smack.o.generated.uvmo": "4eab53744378a1b7b1321492d7d2588521b3e9555ed84d1eed976b157922f935",
    "partial_snd.o.generated.uvmo":  "9fe7eef58378e0bb882a0394eb351eafa7d0419a0bcec16779f38a6eb2d4d418",
    "partial_sparks.o.generated.uvmo": "3e464e9993aa4d21f3b0d655dbf909bfd3eb9f0502b00eb145e1dda7d3981d04",
    "partial_splash.o.generated.uvmo": "73083f3f46118da4509e86a4572e9e9bd4c76041ffee39725c2a4fedf7822d21",
    "partial_spray.o.generated.uvmo": "68a204076fc200eacae3b25481fd587ed842b9e121baea73ad7decdc5922494a",
    "partial_tdata.o.generated.uvmo": "7d8b1974e6facb0a0f2d5215374a445e60443eac89efde8c7c1ce5eb29edfe9c",
    "partial_ted.o.generated.uvmo": "82470bb7b8e021ad30b1872c97dcd841441f33f8f08fddce8e937c9f27647251",
    "partial_track1.o.generated.uvmo": "ff938295ea85040ec8268285a544aff3d0dedc9bbca74bfaacfe01441d3b7b1a",
    "partial_track2.o.generated.uvmo": "4e42b278ae4df1ead1a639d6b2572ea53b5b7090015786f0a31f1ba31feae9dd",
    "partial_track3.o.generated.uvmo": "4c31d21cbe7c175bbdcf575cd97d256063c8d1afd4a0de035955a09582d37df1",
    "partial_track4.o.generated.uvmo": "56d03fc6a4a3e10423167addff9c3c853ff4eec7f1673b34920d335fd4c21e92",
    "partial_track5.o.generated.uvmo": "b2ae03f44221f3cb61ab2ec9be13f9110147487fae1bb6bae6d8a05cff2d8cfd",
    "partial_track6.o.generated.uvmo": "d7e6f94559c72eed8933375192d3848bf6e1791a8417ba002d67d8d11f6dfe1c",
    "partial_track7.o.generated.uvmo": "9059caef0082ee5a280951ce9626a26c0bac3ce7e3c2eeed069ec9a6774778bf",
    "partial_triganim.o.generated.uvmo": "f8bc561be528085db7d8370753267e916deda0deb4f906d6914e451bde6828bf",
    "partial_txtview.o.generated.uvmo": "74c889cc3bd081042f2b3fafb5d12a2310df4cae5751b2ccae3091c493718b61",
    "partial_ufogame.o.generated.uvmo": "eca356e5eb559e372cb3eeb18fe14aa8e1ce8b95648a675ae87ba556ad023df4",
    "partial_ufo.o.generated.uvmo": "10fa1d326f89a483327f2f967593355075d22b7de1c35cddad7911d8c07d1cd7",
    "partial_uvaudiomgr_rom.o.generated.uvmo": "f99f6f14f4709de6dffec5837079d87cf60ff0e535a9cd5ecba3f986b91ed576",
    "partial_uvbill_rom.o.generated.uvmo": "89f1ab2c636e223351079952b11f7934a14f918a6e70486de36055231a31a9d2",
    "partial_uvblit_rom.o.generated.uvmo": "1f1d74a67ed86d6f40ec7a1c029a249bcd286c31675e4c75afb1c4440ba15ed4",
    "partial_uvcback_rom.o.generated.uvmo": "17f85531a270df22b160bd64a5a312f6dad54a05ca1e82a81e4b71ccecc15155",
    "partial_uvchannel_rom.o.generated.uvmo": "6406a7e932f79a4d0c2b377f21408e0eb4fb61b2c042ea9f4f495fafb982eef3",
    "partial_uvcmidi_rom.o.generated.uvmo": "1df8b9a8123a9334cbe5fae2cebb47ab59e15d59cad6cf61a05ffbce498cb92d",
    "partial_uvcolor_rom.o.generated.uvmo": "9df81230fae223434f86f68a8a2fa9cf5378d7fa520563b2d53a4f5302a49ab1",
    "partial_uvcontourld_rom.o.generated.uvmo": "367eb69896a70cfcd9dfeedd216c67523335cb15919b26a5217a1aa01fc2c2dd",
    "partial_uvcont_rom.o.generated.uvmo": "d06a8f8d6070d01992ca174db49030b9f143b2ec26e74b23afc273b92943e83e",
    "partial_uvdbg_rom.o.generated.uvmo": "6ed9b7849815edb4a10233ce4032622899c50957f6ec62372eb2dc0dbf4100f8",
    "partial_uvdgeom_rom.o.generated.uvmo": "acd17d95d77d1515d69fa41043c95bfac32169b6767df13faf453d5d7c343627",
    "partial_uvdobj_rom.o.generated.uvmo": "e2f330845763152ab3ff190f24293ce35b4b6bb833911086a877829e152da5a9",
    "partial_uvdsetld_rom.o.generated.uvmo": "602c46de4d63e37b235f69b9498301ec06198695810d1c3315a275df1d91fe92",
    "partial_uvdyn_rom.o.generated.uvmo":  "c8e3db44a2eb33a5b592dc56e4f918da45c2537b57ce62d1120a278176af9196",
    "partial_uvear_rom.o.generated.uvmo": "33bf3dd176e4eb30acfdda93e451a034876eefe65b21c8cee5be1fdc42a9c857",
    "partial_uvemitter_rom.o.generated.uvmo": "99773b238342892d4c3f32c4e7ab49a4b5643c4ef78319938e679438b149aabe",
    "partial_uvenvld_rom.o.generated.uvmo": "2ad1909c42ef8c623c718855215e39089890e63bc92cfccebaa077f193573d12",
    "partial_uvenv_rom.o.generated.uvmo": "b5fd5cfdd30fba5f63db55274ad24ef17361d1d27f1c423635e9956c2f80f112",
    "partial_uvfmtx_rom.o.generated.uvmo": "7f5eee0dc935530d0091c8507a1bf7120399af2d3953f1c4857c4690f53e7c95",
    "partial_uvfontld_rom.o.generated.uvmo": "c1222cc27bec22068751a3c026c965ec9ac8d42040c2be14f5a286887c5b7064",
    "partial_uvfont_rom.o.generated.uvmo": "3bd3860eb68e7a4e5b7dd24c6e0cbc4c329184e22e79938c23c8a2f42862cdd3",
    "partial_uvfvec_rom.o.generated.uvmo": "ce9491da183d172fa912fa15ebb930876cceb6a0d9a0383d039f78129a03616f",
    "partial_uvfx_rom.o.generated.uvmo": "e8678a71ccfd889f44c251260bbdcdfcb4f93d2e0757631f2b3dae7f3350ea55",
    "partial_uvgeom_rom.o.generated.uvmo": "f2a4c5576f9072b3a3a38781026eddd1baf178174620d2c9852a155082483177",
    "partial_uvgfxmgr_rom.o.generated.uvmo": "0be4126d64f6e5313c2eb535d0c34c686ea18d8c1dcd2e5a9a97844a85c1e8d8",
    "partial_uvgfxstate_rom.o.generated.uvmo": "a031fc98c8bdeff0b9cc21713dfe82f9b15b8d026fba29a556baf4632bb6fc08",
    "partial_uvgrph_rom.o.generated.uvmo": "b2c79f3d2962eca02311bd01bffc03daff0cd3e078db8b9b192185c1cadc098a",
    "partial_uvgui_rom.o.generated.uvmo":  "6399bd6b764792b9c7249a7b4042cd6b096dfd813dbb05b9ef541774359718d8",
    "partial_uvimtx_rom.o.generated.uvmo": "39b4cc71c2ea8bc25cd4c0e2b9d97a61707a446219114e0c55b1921d85a66e4a",
    "partial_uvintersect_rom.o.generated.uvmo": "05d91d41bce6f00a5c7b5422ec1cdce291e3d24339ce8667e83a8e9e6dba2d2b",
    "partial_uvisect_rom.o.generated.uvmo":  "76f4f110a22729316751ebad5cc82eec408608577116b9c9c60f407ca0c2b7ca",
    "partial_uvjanimld_rom.o.generated.uvmo": "831d5e1db405f64da8773018897cad93a0c7ef14a90f1973f66f754398911c9a",
    "partial_uvjanim_rom.o.generated.uvmo": "9449c0f6b0d3d4979cb063d8eaf43d59ea1b03fda344d441273c67277c6c76cc",
    "partial_uvled_rom.o.generated.uvmo": "d6a6676c8f3f440b9b72e4d12a57d482261ad9aa0bec7760b7405352e99aa55d",
    "partial_uvlight_rom.o.generated.uvmo": "3c94886834d90ca0259e91bfb1ca7ca07b4f3006f2eb102c2f190ae5b86695bf",
    "partial_uvmath_rom.o.generated.uvmo": "30c088355d2502b0152a500a92917f8998762622c0b1e4febd8af9ea73dde1c6",
    "partial_uvmodelld_rom.o.generated.uvmo": "0bd24fd58f3da25000788e1966d63af267b882b8a3df0810eb2a332cfb9f11ad",
    "partial_uvmodel_rom.o.generated.uvmo": "6b2a902e865ec1181860af4df550acd442061d0a86537e6bdb4bd793a8367e0f",
    "partial_uvpfxld_rom.o.generated.uvmo": "ae75616e5ef7c075829dab9214efcdc78f2419c5559da873ee80e7183a93bc13",
    "partial_uvpfx_rom.o.generated.uvmo": "6f294fd3433011ae1e938ab42dce56f9c73ffad8c62cbdfadab125da42769cd6",
    "partial_uvquat_rom.o.generated.uvmo": "1ae535250c3cfb1bd7df076768add580dedcec431535ced0fdf3d167c6997dad",
    "partial_uvquery_rom.o.generated.uvmo": "80dc8da52f764663fb6efdf6a0ace3b0feed4671626117ae71d4ce22dbce53c7",
    "partial_uvsort_rom.o.generated.uvmo": "fcc721f0a4716f4a5229a9e3fe0b54ee5257373db970b676a43edeaf61ce577a",
    "partial_uvsprt_rom.o.generated.uvmo": "3358b11eee88c42d794ef657d51e3b768aa3c7f688eb25455af8aa44e73d3902",
    "partial_uvstring_rom.o.generated.uvmo": "39c7294b0a3411b44c281992c0e68cba8e782a0db4a362cbc03462bbf0ffb137",
    "partial_uvterrald_rom.o.generated.uvmo": "cf44de2d66880c6892d28dc18efac8bca5bb5e60dd05d968647aadbbe912a420",
    "partial_uvterra_rom.o.generated.uvmo": "9fbbd4d1de1890f4e44e77e41aa4974f8090b9b41faafd3cf2fe884c3548b54a",
    "partial_uvtexanim_rom.o.generated.uvmo": "2d31b184f94bd38358b0aec81ec64dd49711fbb1a6a4bced9eea5d433cad2a5f",
    "partial_uvtextureld_rom.o.generated.uvmo": "529d5c748bd8b0a0375ac80890e8242f5cceb27956aa4f3598cc5fa330b21c83",
    "partial_uvtexture_rom.o.generated.uvmo": "1efe5e2a29e6f5e3b9b73d741ad28877e9e033c89cb72009e30ed74ccf9048ae",
    "partial_uvtexturexref_rom.o.generated.uvmo": "b6a1cc8d486ae751855b95bb0accf8a427bc71aa385993e981d16531a65f3e19",
    "partial_uvtrackld_rom.o.generated.uvmo": "d878dbdf03a94f261db51f39f5fcd071be4d513d1d28e8a68660c6ed2700bf3b",
    "partial_uvtseqld_rom.o.generated.uvmo": "1f7b14ee7cb05cf5c998fdb5778080f13fe24edef7b3213ef31d215c24f15f51",
    "partial_uvtseq_rom.o.generated.uvmo": "45e4330cfdafcf9f610d68ae2e97025c6aca521fdccf35eb117f7a4117f4f278",
    "partial_uvufileld_rom.o.generated.uvmo": "4f5f11cc7788689c0ea547e6801c300b8b0afc28b5acd229ddfcfb1541e5975d",
    "partial_uvvattr_rom.o.generated.uvmo": "acd63877e7f1822ef8dbd36b0dad84400d96e5afdfcb136c663714dff5d4cefd",
    "partial_uvvolumeld_rom.o.generated.uvmo": "dee1c4fb80904d9dbe6ce335481c1ec5fbc7e26e802cc939f8dd3363050c3f63",
    "partial_victory.o.generated.uvmo": "1d33efe100027b792cc3813ed6da1fd5c49ac49642d13f720d8a5bd8a0ab27fb",
    "partial_voltest.o.generated.uvmo": "923af8c1a2b0fa4aabe23d18c2f8ed0e1f0da354fc2e0fef0e6de1d603c4adfd",
    "partial_weapon.o.generated.uvmo": "525c750092e9ff35ba27fda4da5efc97db10838fc41b900247189062231ef451"
}


def getRandomCourseName():
    courses = [
        "Coventry Cove",
        "Mount Mayhem",
        "Inferno Isle",
        "Sunset Sands",
        "Metro Madness",
        "Wicked Woods",
    ]
    return courses[random.randrange(0, len(courses))]

def sha256_file(path):
    h = hashlib.sha256()

    with open(path, "rb") as f:
        while chunk := f.read(8192):
            h.update(chunk)

    return h.hexdigest()


if __name__ == "__main__":
    if len(sys.argv) != 3:
        print(f"{COLOR_YELLOW}{WARN_MARK} Usage: python checkModuleHash.py <file>{COLOR_RESET}")
        sys.exit(1)

    file_path = sys.argv[1]
    module_name = file_path.split("/")[-1]
    
    # Print header
    print(f"\n{COLOR_BOLD}{COLOR_CYAN}{'='*60}{COLOR_RESET}")
    print(f"{COLOR_BOLD}{COLOR_CYAN}  SHA256 Hash Verification{COLOR_RESET}")
    print(f"{COLOR_BOLD}{COLOR_CYAN}{'='*60}{COLOR_RESET}\n")
    
    print(f"{COLOR_DIM}{INFO_MARK} File: {COLOR_RESET}{COLOR_CYAN}{file_path}{COLOR_RESET}")
    print(f"{COLOR_DIM}{INFO_MARK} Checking hash...{COLOR_RESET}\n")
    
    calculated_hash = sha256_file(file_path)
    expected_hash = BarModuleFiles.get(module_name)

    if expected_hash is None:
        print(f"{COLOR_RED}{CROSS_MARK} Error: Module '{module_name}' not found in hash database.{COLOR_RESET}\n")
        sys.exit(1)

    if calculated_hash == expected_hash:
        print(f"{COLOR_GREEN}{COLOR_BOLD}{CHECK_MARK} SUCCESS!{COLOR_RESET}")
        print(f"{COLOR_GREEN}Hash verification passed.\nYour beetle passed the safety checks and is ready to race in {getRandomCourseName()}!{COLOR_RESET}")
    else:
        if sys.argv[2] == 'True':
            print(f"{COLOR_YELLOW}{WARN_MARK} MISMATCH!{COLOR_RESET}")
            print(f"{COLOR_YELLOW}Hash verification failed.\nYour wild Beetle will race, but don’t blame us if you crash near {getRandomCourseName()}! {COLOR_RESET}\n")
        else:            
            print(f"{COLOR_RED}{CROSS_MARK} MISMATCH!{COLOR_RESET}")
            print(f"{COLOR_RED}Hash verification failed.\nBad news buddy, your beetle just forgot how to beetle and cannot race in {getRandomCourseName()}!{COLOR_RESET}\n")
            print(f"{COLOR_YELLOW}Expected:{COLOR_RESET}")
            print(f"  {COLOR_BOLD}{expected_hash}{COLOR_RESET}")
            print(f"\n{COLOR_YELLOW}Calculated:{COLOR_RESET}")
            print(f"  {COLOR_BOLD}{calculated_hash}{COLOR_RESET}\n")
            sys.exit(1)
    
    print(f"{COLOR_BOLD}{COLOR_CYAN}{'='*60}{COLOR_RESET}\n")
