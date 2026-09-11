Inside the ROM for Beetle Adventure Racing, a hidden yet very robust debug menu can be found.  While not everything functions perfectly, the debug menu has been extremely important in helping us understand how the game works "under the hood".  Other sites have already documented much of the debug features before; however, this guide contains newly discovered information on what the debug menu can actually do and also how to properly navigate its sometimes-daunting interface.  

After extensive testing and verification, this guide aims to provide a clean point of reference that's easy to understand, which hopefully will lead to quicker answers when it comes to documenting the game engine.  

## How to enable the Debug Menu

Simply change the byte located at `80025E7F` from `00` to `01`.  If you want to make a cheat for Project 64, you can use the following code to enable the menu:

`80025E7F 0001`

## Getting started

The menu can be accessed immediately upon game start up.  Press **D-Pad Down** to bring up a flashing cursor and title bar that reads `"N E E D  F O R  S P E E D"` (for those who don't know, BAR was originally conceived as an N64-exclusive Need for Speed title).  

Use the analog stick to move the cursor around.  If you hover it over the menu bar, the menu opens up with a wide range of options.  Let's call this the "main menu".

Press **D-Pad Down** to make the main menu disappear.  

Pressing **D-Pad Up** will bring up **another** menu.  We'll refer to this as the "game state menu" as it changes depending on which state the game engine is in.  To exit, you can either press **D-Pad Up** again or **D-Pad Down** twice.

If the debug menu is enabled, you can skip the company logos and intro cutscenes and go right to the Title screen, which really helps with going through numerous iterations.  As soon as the N64 logo is displayed, press the **L Trigger** to skip.

During a race with debug enabled, pressing **D-Pad Right** will cause the camera to stop immediately and track the player car as it drives away.  The camera will zoom in and keep tracking the player, even though there might be world geometry blocking the view.

## Known Game State Menus

In the NTSC ROM at address 0x58780, there are 16 FourCC tags for the game states.

Tag | Menu Name | Description
----|----------|------------
`intr` |INTRO     | Only pops up when cutscene ends.  "REPLAY" is the only menu button
`demo` |DEMO      | Credits only
`race` |RACE      | Debug race menu
`batl` |BATTLE    | Debug Beetle Battle menu
`vict` |VICTORY   | Championship win screen
`ufom` |UFO       | Free look camera
`medt` |SCENE EDITOR | General model viewer 
`tedt` |TASK EDITOR | Track/prop editor
`aied` |AIEDIT | AI Path editor
`txtv` |TEXTURE VIEW | Broken texture viewer (fixable?)
`cbar` |COLOR BARS   | color bars test screen
`slct` |SELECTION    | Save file select and main menu
`prof` |PROFILE CAR  | Car viewer (with 8 cars instead of 1)

## SELECTION

Selection refers to all the menu UI screens and is the first state that the game loads.  The save game menu pops up and after the program loads the save file, you can press **L Trigger** to skip all the company logos and intro cutscenes.  The controls for this menu don't appear to work.
