// SPDX-License-Identifier: AGPL-3.0-or-later
enum SfxId {
        BIGWTRFALL8,
        WATER5,
        SKID11,                 // tire skid (surprisingly bad loop)
        CHEROKEECRUZ1L1,
        CAMAROLOAD_PS22MB_L1,
        CAMARO_CRUZ_L1,
        BUMP15,
        GRAVELSKID,
        WETROADNOISE2,
        BUMP12,
        SPLASH,
        S_DRIVE,                // this is actually S_HORN
        S_HORN,                 // S_UP? (UI select)
        S_KLUNK,                // very short impact sfx (unused?)
        S_UP,                   // (UI cancel)
        PHONEGAG,               // "We're sorry, your call..."
        ICECRYSTALBREAK,
        WOODSLAT,               // Sunset Sands wooden bridge?
        AH1,                    // helicopter (fast)
        CHINOOK,                // helicopter (slow)
        UFO1,                   // UFO drone (far -growling)
        UFO2,                   // UFO drone (close - harsh synth loop)
        CROWDSKIJUMP,           // cheering crowd
        MOBSKIJUMP,             // booing crowd after failed jump
        GONDOLAS,               // squeaky cable car
        FRZH2OFALL,             // frozen waterfall impact
        STALACTITES,            // repurposed for Wicked Woods bridge breakable
        RUSHINGH2O,             // rushing water
        NULL,
        ICECRACK1,              // Mount Mayhem frozen waterfall shortcut
        STAINEDGLASS,           // Coventry / Wicked church glass impact
        DIRTROAD,
        GRAVELROAD,
        METALSCRAPE,
        PAVEDROAD,
        SNOWROAD,
        CRANEENGINE,            // Coventry Cove crane
        CRANEWINCH,
        TREE,                   // not sure where this is used
        MINEENTER,              // Coventry mineshaft
        TOLLBOOTH,              // Coventry bridge bar gate impact
        PHONEBOOTH,
        SAWHORSE,               // very similar to TOLLBOOTH
        CARLDGSUSP,             // suspension squeak
        BARRELEXPLODE,
        SIGNHIT,                // road sign impact
        ELEVATORDN,             // Sunset Sands elevator (with slam at end)
        ELEVATORUP,             // shorter stone elevator scrape
        H2OFALLECHO,            // Coventry Cove mineshaft waterfall?
        FIRE,                   // generic fire sfx
        WOODCREAK,
        PILLARSTOPPLE,          // Sunset Sands pillars collapsing
        POTTERYSMASH,
        GARAGEDOOR,             // Mount Mayhem glass door impact
        SNOWMAN5,               // snowman impact
        SNOWSLIDE,
        ICECRYSTALSMASH,        // final in-game version of ICECRYSTALBREAK
        WICKERBASKET,           // Sunset Sands prop?
        ELEVATORBELL,           // Sunset Sands elevator bell ding
        SCAFFOLDCREAK,          // Sunset / Wicked wooden bridge
        SCAFFOLDGROAN,
        WIDESIGNHIT,            // another road sign impact
        CAREXPLODE,
        CTRAINTRACKS,           // Metro Madness city train
        MOVIESCREEN,            // movie screen impact
        GASPUMPEXPLODE,         // Metro Madness gas pump
        BOARDEDTEMPLE,
        WATERFOUNTAIN,
        CARPUDDLESPLASH2,
        CHAINLINKFENCE,
        CONCRETESCRAPE,
        CURBBUMP,
        SLOTCOINSPULL,          // Metro Madness casino slot machine sfx
        SLOTHANDLEPULL,
        SLOTWINNER1,
        SLOTWINNER2,
        STANDSSKIJUMP,          // ski jump crowd screaming
        CRATEEXPLODE,
        TREXROAR,
        LAVABUBBLES,
        LOWRUMBLE,
        JTRAINTRACKS,           // Inferno Isle train
        CAMERA,                 // same as S_KLUNK
        SNOW,
        ICESKID,
        MONKEYS,
        ICETIRESPIN,
        CONSTRUCTBARREL,
        COBBLESTONEROAD,
        MOVIEPROJECTOR,
        TREXSTEP,               // t-rex footsteps
        CANNONBALLSHOT,         // pirate ship shortcut
        BULLFROG,
        BUSH,
        CRICKETS,
        JUNGLEAMBIENT1,
        ENGINE1CRUZ,            // player car engine sfx
        ENGINE1LOAD,
        ENGINE2CRUZ,
        ENGINE2LOAD,
        XINGBELL,               // Coventry Cove bridge bell
        ROCKSLIDE,
        CHURCH,                 // possibly unused?
        SURF,                   // Inferno Isle beach waves
        ENGINE3CRUZ,
        ENGINE3LOAD,
        INVINCIBILITY,          // Chrome Beetle power up
        PWRUPNITRO,             // Nitro Box impact (Ya-hoo!)
        DIRTSKID,
        V_GO,                   // "Go!"
        V_ONE,                  // "One!"
        V_THREE,                // "Three!"
        V_TWO,                  // "Two!"
        V_FINISH,               // "Finish!"
        V_LAPTWO,               // "Lap two!"
        V_COVE,                 // "Welcome to...Coventry Cove"
        V_WOODS,                // "Welcome to...Wicked Woods"
        V_MAYHEM,               // "Welcome to...Mount Mayhem"
        V_SANDS,                // "Welcome to...Sunset Sands"
        V_INFERNO,              // "Welcome to...Inferno Isle"
        V_METRO,                // "Welcome to...Metro Madness"
        V_FINALLAP,             // "It's the final lap!"
        V_TITLE,                // "Adventure Racing!"
        V_HOWDY,                // "Welcome!"
        HORNLOOP,               // annoying horn loop
        POINTS_1,               // 2 point box xylophone jingle
        POINTS_5,               // 5 point box jingle
        POINTS_10,              // 10 point box jingle
        MINEPLANT,              // Beetle Battle mine drop
        MISSILESHOT,            // Beetle Battle missile
        MISSILEHIT,             // unused?
        MISSILEHIT1,
        LADYBUG,                // Beetle Battle ladybug pickup
        FORCEFIELD,             // Inferno Isle electric fence
        HEALTH_GOOD1,           // "Now ya kickin'!"
        HEALTH_BAD,             // "Oh no!" (Inferno Isle hut impact)
        DRIVEWATER,
        LAVAFALL,
        V_SPLIT_30_1,           // "You're falling behind!"
        V_SPLIT_30_2,           // "Pick up the pace!"
        V_SPLIT_50_1,           // "Hurry up!"
        V_SPLIT_50_2,           // "Pedal to the metal!"
        V_SPLIT_60,             // "Disqualified!"
        WATERWHEEL,             // Coventry Cove prop
        JACK,                   // Wicked Woods jack-o-lantern laugh
        PURPWIND,               // Wicked Woods wind howling and chimes
        OWL,
        SPOOKY,                 // haunted house ghostsooooh
        SCREAM,                 // Wicked Woods dungeon shortcut
        CHAINS,
        BATS,
        LASTLADYBUG,            // "Head for the exit!"
        WOLVES,                 // spooky wolves howling
        COPSIREN,
        TORCHBREAK,             // fire torch snap
        CHROMECAR,              // Chrome Beetle drone
        PLYRHORN,
        CARLDGLITE,             // car jump landing
        WWCLKCHIME,             // Wicked Woods clock tower chime
        INVERT,                 // "Trip out"
        FOGWEAPON,              // "Get funky"
        V_WINNER,               // "Winner, winner, winner!"
        V_BATTLE1,              // "Let's get it on"
        V_BATTLE2,              // "Get down!"
        V_BATTLE3,              // "Here we go!"
        V_BATTLE4,              // "Let's get busy!"
        V_BONUS_50,             // "You got it!"
        V_BONUS_100,            // "You got them all!"
        V_GAME_OVER,            // "Game over!"
        V_READY,                // "Get ready!"
        STEALERHIT,             // Beetle Battle stealer hit laugh
        S_CONFIRM,              // UI confirm
        HEALTH_GOOD2,           // "Baby!"
        HEALTH_BAD_B1,          // "Nasty!"
        HEALTH_BAD_B2,          // "That's cold, baby!"
        BATTLEEXPLODE1,         // "Dyno-mite!"
        BATTLEEXPLODE2,         // "That's gotta hurt!" (Blu V.O.)
        BATTLEEXPLODE3,         // "You bad!"
        V_WINNER_A,             // "Awwww, yeaaaah!"
        V_WINNER_B,             // "Hail to the king, baby!"
        V_WINNER_C,             // "You're bumpin' with the flava now!"
        V_GROOVY,               // "Grrrrrooovy!"
        V_NICEWHL,              // "Nice wheels!"
        V_BEETLEBAT,            // "Beetle Battle"
        V_DUEL,                 // "Duel"
        V_ONE_PLAYER,           // "One Player"
        V_OPTIONS,              // "Options"
        V_RECORDS,              // "Records"
        V_OK,                   // "Ok!"
        DRAGON,                 // Dragon growling (unused)
        SLAMDOOR,               // Haunted door slamming loop (infamous glitch)
        V_NOVICE,               // "Novice circuit"
        V_ADVANCED,             // "Advanced circuit"
        V_PRO,                  // "Pro circuit"
        V_BONUS,                // "Bonus circuit"
        DRAGONFIRE,             // Dragon roaring and flames
        UFOMOVIE,               // Metro Madness movie theater sfx
        V_RECORDSET,            // "Record!"
        V_PLACE1,               // "First place!"
        V_PLACE2,               // "Second place!"
        V_PLACE3,               // "Third place!"
        V_PLACE4,               // "Fourth place!"
        V_PLACE5,               // "Fifth"
        V_PLACE6,               // "Sixth"
        V_PLACE7,               // "Seventh"
        V_PLACE8,               // "Eigth"
        V_YOU_PLACED,           // "You placed..."
        V_PLACE1A,              // "Congratulations!"
        V_PLACE1B,              // "Way to go!"
        V_PLACE1C,              // "You got first!"
        V_PLACE234A,            // "Great race!"
        V_PLACE5678A,           // "Nice try"
        V_PLACE8B,              // "Last place?!"
        V_PLACE8C,              // "You'll never win that way!"
        V_PLACE8D,              // "Dead last!"
        V_CONTINUE,             // "You're still in the game!"
        V_CHAMPSTOP1,           // "Better luck next time"
        V_CHAMPSTOP2,           // "Practice makes perfect!"
        V_CHAMPSTOP3,           // "You can do better"
        S_JOY_LR,               // UI select (left/right)
        V_AWARD_A,              // "Well done, you cleared advanced..."
        V_AWARD_N,              // "Congratulations, you won novice..."
        V_AWARD_P,              // "Buckle up and hit the bonus circuit, baby!"
        SLOWDOWN,               // "Funk-a-delic!"
        V_SINGLRACE,            // "Single race"
        V_CHAMPNSHIP,           // "Championship"
        V_FULL_GRID,            // "Full Grid"
        V_TIME_ATAK,            // "Time Attack"
        V_GOODCHOICE,           // "Good choice!"
        V_MULTIPLYR,            // "Two player"
        HORN2LOOP,
        HORN3LOOP,
        HORN4LOOP,
        ALIENHORN,              // "We come in peace"
        V_HOTLAVA,              // "Hot lava!! Huuh!"
        V_NEW_CONT,             // "You got a continue!"
        V_NEW_ARENA,            // "A new arena!"
        V_ALLRIGHT,             // "Allright!" (10 point box hit)
        V_THREEPLYR,            // "Three player"
        V_FOURPLYR,             // "Four player"
        V_EXPLOSION,            // "Ooh! That's gotta hurt!" (David V.O.)
        CROWDCLAP,              // Championship win
        CROWDCHEER,             // Bonus championship win
        CROWDMOAN,              // unused?
        V_CHEATMENU,            // "Cheat menu activated!"
        V_GOTCHEAT,             // "New cheat activated!"
        V_TRYAGAIN,             // "Nice try, but you need to get first"
        S_TALLYBELL,            // finish tallying points
        S_TALLYCOUNT,           // tallying points for championship
        V_CHAMPION              // "You're the champion!"
};

// strings for debug menu
enum Sfx {
        "BIGWTRFALL8         ",
        "WATER5              ",
        "SKID11              ", // tire skid (surprisingly bad loop)
        "CHEROKEECRUZ1L1     ",
        "CAMAROLOAD_PS22MB_L1",
        "CAMARO_CRUZ_L1      ",
        "BUMP15              ",
        "GRAVELSKID          ",
        "WETROADNOISE2       ",
        "BUMP12              ",
        "SPLASH              ",
        "S_DRIVE             ", // this is actually S_HORN
        "S_HORN              ", // S_UP? (UI select)
        "S_KLUNK             ", // very short impact sfx (unused?)
        "S_UP                ", // (UI cancel)
        "PHONEGAG            ", // "We're sorry, your call..."
        "ICECRYSTALBREAK     ",
        "WOODSLAT            ", // Sunset Sands wooden bridge?
        "AH1                 ", // helicopter (fast)
        "CHINOOK             ", // helicopter (slow)
        "UFO1                ", // UFO drone (far - "growling")
        "UFO2                ", // UFO drone (close - harsh synth loop)
        "CROWDSKIJUMP        ", // cheering crowd
        "MOBSKIJUMP          ", // booing crowd after failed jump
        "GONDOLAS            ", // squeaky cable car
        "FRZH2OFALL          ", // frozen waterfall impact
        "STALACTITES         ", // repurposed for Wicked Woods bridge breakable
        "RUSHINGH2O          ", // rushing water
        "NULL                ",
        "ICECRACK1           ", // Mount Mayhem frozen waterfall shortcut
        "STAINEDGLASS        ", // Coventry / Wicked church glass impact
        "DIRTROAD            ",
        "GRAVELROAD          ",
        "METALSCRAPE         ",
        "PAVEDROAD           ",
        "SNOWROAD            ",
        "CRANEENGINE         ", // Coventry Cove crane
        "CRANEWINCH          ",
        "TREE                ", // not sure where this is used
        "MINEENTER           ", // Coventry mineshaft
        "TOLLBOOTH           ", // Coventry bridge bar gate impact
        "PHONEBOOTH          ",
        "SAWHORSE            ", // very similar to TOLLBOOTH
        "CARLDGSUSP          ", // suspension squeak
        "BARRELEXPLODE       ",
        "SIGNHIT             ", // road sign impact
        "ELEVATORDN          ", // Sunset Sands elevator (with slam at end)
        "ELEVATORUP          ", // shorter stone elevator scrape
        "H2OFALLECHO         ", // Coventry Cove mineshaft waterfall?
        "FIRE                ", // generic fire sfx
        "WOODCREAK           ",
        "PILLARSTOPPLE       ", // Sunset Sands pillars collapsing
        "POTTERYSMASH        ",
        "GARAGEDOOR          ", // Mount Mayhem glass door impact
        "SNOWMAN5            ", // snowman impact
        "SNOWSLIDE           ",
        "ICECRYSTALSMASH     ", // final in-game version of ICECRYSTALBREAK
        "WICKERBASKET        ", // Sunset Sands prop?
        "ELEVATORBELL        ", // Sunset Sands elevator bell ding
        "SCAFFOLDCREAK       ", // Sunset / Wicked wooden bridge
        "SCAFFOLDGROAN       ",
        "WIDESIGNHIT         ", // another road sign impact
        "CAREXPLODE          ",
        "CTRAINTRACKS        ", // Metro Madness city train
        "MOVIESCREEN         ", // movie screen impact
        "GASPUMPEXPLODE      ", // Metro Madness gas pump
        "BOARDEDTEMPLE       ",
        "WATERFOUNTAIN       ",
        "CARPUDDLESPLASH2    ",
        "CHAINLINKFENCE      ",
        "CONCRETESCRAPE      ",
        "CURBBUMP            ",
        "SLOTCOINSPULL       ", // Metro Madness casino slot machine sfx
        "SLOTHANDLEPULL      ",
        "SLOTWINNER1         ",
        "SLOTWINNER2         ",
        "STANDSSKIJUMP       ", // ski jump crowd screaming
        "CRATEEXPLODE        ",
        "TREXROAR            ",
        "LAVABUBBLES         ",
        "LOWRUMBLE           ",
        "JTRAINTRACKS        ", // Inferno Isle train
        "CAMERA              ", // same as S_KLUNK
        "SNOW                ",
        "ICESKID             ",
        "MONKEYS             ",
        "ICETIRESPIN         ",
        "CONSTRUCTBARREL     ",
        "COBBLESTONEROAD     ",
        "MOVIEPROJECTOR      ",
        "TREXSTEP            ", // t-rex footsteps
        "CANNONBALLSHOT      ", // pirate ship shortcut
        "BULLFROG            ",
        "BUSH                ",
        "CRICKETS            ",
        "JUNGLEAMBIENT1      ",
        "ENGINE1CRUZ         ", // player car engine sfx
        "ENGINE1LOAD         ",
        "ENGINE2CRUZ         ",
        "ENGINE2LOAD         ",
        "XINGBELL            ", // Coventry Cove bridge bell
        "ROCKSLIDE           ",
        "CHURCH              ", // possibly unused?
        "SURF                ", // Inferno Isle beach waves
        "ENGINE3CRUZ         ",
        "ENGINE3LOAD         ",
        "INVINCIBILITY       ", // Chrome Beetle power up
        "PWRUPNITRO          ", // Nitro Box impact ("Ya-hoo!")
        "DIRTSKID            ",
        "V_GO                ", // "Go!"
        "V_ONE               ", // "One!"
        "V_THREE             ", // "Three!"
        "V_TWO               ", // "Two!"
        "V_FINISH            ", // "Finish!"
        "V_LAPTWO            ", // "Lap two!"
        "V_COVE              ", // "Welcome to...Coventry Cove"
        "V_WOODS             ", // "Welcome to...Wicked Woods"
        "V_MAYHEM            ", // "Welcome to...Mount Mayhem"
        "V_SANDS             ", // "Welcome to...Sunset Sands"
        "V_INFERNO           ", // "Welcome to...Inferno Isle"
        "V_METRO             ", // "Welcome to...Metro Madness"
        "V_FINALLAP          ", // "It's the final lap!"
        "V_TITLE             ", // "Adventure Racing!"
        "V_HOWDY             ", // "Welcome!"
        "HORNLOOP            ", // annoying horn loop
        "POINTS_1            ", // 2 point box xylophone jingle
        "POINTS_5            ", // 5 point box jingle
        "POINTS_10           ", // 10 point box jingle
        "MINEPLANT           ", // Beetle Battle mine drop
        "MISSILESHOT         ", // Beetle Battle missile
        "MISSILEHIT          ", // unused?
        "MISSILEHIT1         ",
        "LADYBUG             ", // Beetle Battle ladybug pickup
        "FORCEFIELD          ", // Inferno Isle electric fence
        "HEALTH_GOOD1        ", // "Now ya kickin'!"
        "HEALTH_BAD          ", // "Oh no!" (Inferno Isle hut impact)
        "DRIVEWATER          ",
        "LAVAFALL            ",
        "V_SPLIT_30_1        ", // "You're falling behind!"
        "V_SPLIT_30_2        ", // "Pick up the pace!"
        "V_SPLIT_50_1        ", // "Hurry up!"
        "V_SPLIT_50_2        ", // "Pedal to the metal!"
        "V_SPLIT_60          ", // "Disqualified!"
        "WATERWHEEL          ", // Coventry Cove prop
        "JACK                ", // Wicked Woods jack-o-lantern laugh
        "PURPWIND            ", // Wicked Woods wind howling and chimes
        "OWL                 ",
        "SPOOKY              ", // haunted house ghosts "ooooh"
        "SCREAM              ", // Wicked Woods dungeon shortcut
        "CHAINS              ",
        "BATS                ",
        "LASTLADYBUG         ", // "Head for the exit!"
        "WOLVES              ", // spooky wolves howling
        "COPSIREN            ",
        "TORCHBREAK          ", // fire torch snap
        "CHROMECAR           ", // Chrome Beetle drone
        "PLYRHORN            ",
        "CARLDGLITE          ", // car jump landing
        "WWCLKCHIME          ", // Wicked Woods clock tower chime
        "INVERT              ", // "Trip out"
        "FOGWEAPON           ", // "Get funky"
        "V_WINNER            ", // "Winner, winner, winner!"
        "V_BATTLE1           ", // "Let's get it on"
        "V_BATTLE2           ", // "Get down!"
        "V_BATTLE3           ", // "Here we go!"
        "V_BATTLE4           ", // "Let's get busy!"
        "V_BONUS_50          ", // "You got it!"
        "V_BONUS_100         ", // "You got them all!"
        "V_GAME_OVER         ", // "Game over!"
        "V_READY             ", // "Get ready!"
        "STEALERHIT          ", // Beetle Battle stealer hit laugh
        "S_CONFIRM           ", // UI confirm
        "HEALTH_GOOD2        ", // "Baby!"
        "HEALTH_BAD_B1       ", // "Nasty!"
        "HEALTH_BAD_B2       ", // "That's cold, baby!"
        "BATTLEEXPLODE1      ", // "Dyno-mite!"
        "BATTLEEXPLODE2      ", // "That's gotta hurt!" (Blu V.O.)
        "BATTLEEXPLODE3      ", // "You bad!"
        "V_WINNER_A          ", // "Awwww, yeaaaah!"
        "V_WINNER_B          ", // "Hail to the king, baby!"
        "V_WINNER_C          ", // "You're bumpin' with the flava now!"
        "V_GROOVY            ", // "Grrrrrooovy!"
        "V_NICEWHL           ", // "Nice wheels!"
        "V_BEETLEBAT         ", // "Beetle Battle"
        "V_DUEL              ", // "Duel"
        "V_ONE_PLAYER        ", // "One Player"
        "V_OPTIONS           ", // "Options"
        "V_RECORDS           ", // "Records"
        "V_OK                ", // "Ok!"
        "DRAGON              ", // Dragon growling (unused)
        "SLAMDOOR            ", // Haunted door slamming loop (infamous glitch)
        "V_NOVICE            ", // "Novice circuit"
        "V_ADVANCED          ", // "Advanced circuit"
        "V_PRO               ", // "Pro circuit"
        "V_BONUS             ", // "Bonus circuit"
        "DRAGONFIRE          ", // Dragon roaring and flames
        "UFOMOVIE            ", // Metro Madness movie theater sfx
        "V_RECORDSET         ", // "Record!"
        "V_PLACE1            ", // "First place!"
        "V_PLACE2            ", // "Second place!"
        "V_PLACE3            ", // "Third place!"
        "V_PLACE4            ", // "Fourth place!"
        "V_PLACE5            ", // "Fifth"
        "V_PLACE6            ", // "Sixth"
        "V_PLACE7            ", // "Seventh"
        "V_PLACE8            ", // "Eigth"
        "V_YOU_PLACED        ", // "You placed..."
        "V_PLACE1A           ", // "Congratulations!"
        "V_PLACE1B           ", // "Way to go!"
        "V_PLACE1C           ", // "You got first!"
        "V_PLACE234A         ", // "Great race!"
        "V_PLACE5678A        ", // "Nice try"
        "V_PLACE8B           ", // "Last place?!"
        "V_PLACE8C           ", // "You'll never win that way!"
        "V_PLACE8D           ", // "Dead last!"
        "V_CONTINUE          ", // "You're still in the game!"
        "V_CHAMPSTOP1        ", // "Better luck next time"
        "V_CHAMPSTOP2        ", // "Practice makes perfect!"
        "V_CHAMPSTOP3        ", // "You can do better"
        "S_JOY_LR            ", // UI select (left/right)
        "V_AWARD_A           ", // "Well done, you cleared advanced..."
        "V_AWARD_N           ", // "Congratulations, you won novice..."
        "V_AWARD_P           ", // "Buckle up and hit the bonus circuit, baby!"
        "SLOWDOWN            ", // "Funk-a-delic!"
        "V_SINGLRACE         ", // "Single race"
        "V_CHAMPNSHIP        ", // "Championship"
        "V_FULL_GRID         ", // "Full Grid"
        "V_TIME_ATAK         ", // "Time Attack"
        "V_GOODCHOICE        ", // "Good choice!"
        "V_MULTIPLYR         ", // "Two player"
        "HORN2LOOP           ",
        "HORN3LOOP           ",
        "HORN4LOOP           ",
        "ALIENHORN           ", // "We come in peace"
        "V_HOTLAVA           ", // "Hot lava!! Huuh!"
        "V_NEW_CONT          ", // "You got a continue!"
        "V_NEW_ARENA         ", // "A new arena!"
        "V_ALLRIGHT          ", // "Allright!" (10 point box hit)
        "V_THREEPLYR         ", // "Three player"
        "V_FOURPLYR          ", // "Four player"
        "V_EXPLOSION         ", // "Ooh! That's gotta hurt!" (David V.O.)
        "CROWDCLAP           ", // Championship win
        "CROWDCHEER          ", // Bonus championship win
        "CROWDMOAN           ", // unused?
        "V_CHEATMENU         ", // "Cheat menu activated!"
        "V_GOTCHEAT          ", // "New cheat activated!"
        "V_TRYAGAIN          ", // "Nice try, but you need to get first"
        "S_TALLYBELL         ", // finish tallying points
        "S_TALLYCOUNT        ", // tallying points for championship
        "V_CHAMPION          "  // "You're the champion!"
};
