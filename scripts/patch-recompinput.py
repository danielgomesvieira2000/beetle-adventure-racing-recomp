#!/usr/bin/env python3
"""Let the port assign controllers to players without going through the modal.

RecompFrontend assigns players exactly one way: the Controls tab's "Assign players" button opens a
modal, each player presses a button on the pad they want, and `commit_player_assignment()` writes
the result. That is the right flow when who is player two matters and the answer is not obvious.

It is the wrong flow for the common case. Until someone has been through that modal, NOTHING is
assigned -- `players::get_player_is_assigned(n)` is false for every n, `profiles::get_n64_input`
reads a profile index of -1, and a pad that is plugged in and working drives nothing at all. A
player who launches the game with a controller attached finds a dead controller and no hint that a
modal in a settings tab is what stands between them and driving.

This adds `players::auto_assign_controllers`, which does what committing a manual assignment does
-- fill the player list in order and give each player the profile that belongs to its controller --
from a list the caller supplies rather than from button presses. Beetle Adventure Racing is a
four-player game, so the order is simply the order SDL reports the pads in: the first pad is player
one, the second is player two, and so on up to four. Anyone who wants to choose differently still
has the modal, and this refuses to run while one is open, so the modal still wins.

Player one also keeps the keyboard, whatever else it has. A keyboard is always attached, so it
should always play: `get_n64_input` merges a player's controller and keyboard profiles, so giving
player one both means the pad and the keys work at the same time and neither has to be chosen.

Nothing upstream changes behaviour: the function is only what the port calls.

Scripted and idempotent because it patches a submodule -- `git submodule update` would otherwise
revert it silently, and the build would then fail to link with no explanation of what was lost.

Run from the repository root (scripts/setup.sh and scripts/setup.ps1 do this for you):
    python scripts/patch-recompinput.py
"""

import sys
from pathlib import Path

REPO = Path(__file__).resolve().parent.parent
HEADER = REPO / "lib" / "RecompFrontend" / "recompinput" / "include" / "recompinput" / "players.h"
SOURCE = REPO / "lib" / "RecompFrontend" / "recompinput" / "src" / "players.cpp"

HEADER_ANCHOR = """        InputDevice get_player_input_device(int player_index, bool temp_player = false);
    }"""

HEADER_REPLACEMENT = """        InputDevice get_player_input_device(int player_index, bool temp_player = false);

        // Assigns the given controllers to players in order, up to the maximum
        // number of players, as though they had been assigned through the modal
        // and committed. Passing none assigns the keyboard to player one, so a
        // machine with no pad attached still has a player. Does nothing while a
        // manual assignment is open.
        void auto_assign_controllers(SDL_GameController* const* controllers, size_t count);
    }"""

SOURCE_ANCHOR = """void playerassignment::stop_and_close_modal() {"""

SOURCE_REPLACEMENT = """void players::auto_assign_controllers(SDL_GameController* const* controllers, size_t count) {
    if (PlayerState.is_assigning) {
        return;
    }

    PlayerArray assigned{};
    const size_t limit = players::get_max_number_of_players();
    for (size_t i = 0; i < count && assigned.get_count() < limit; i++) {
        if (controllers[i] != nullptr) {
            assigned.add_controller_player(controllers[i]);
        }
    }
    if (assigned.get_count() == 0) {
        assigned.add_keyboard_player();
    }

    PlayerState.players = assigned;

    // The same profile assignment commit_player_assignment performs, so that a
    // player's own remapping follows its pad into whichever slot it lands in.
    for (int i = 0; i < (int)PlayerState.players.get_count(); i++) {
        Player &player = PlayerState.players[i];
        if (player.controller != nullptr) {
            int cont_profile_index = profiles::get_controller_profile_index_from_sdl_controller(player.controller);
            if (cont_profile_index >= 0) {
                profiles::set_input_profile_for_player(i, cont_profile_index, InputDevice::Controller);
            }
        } else {
            profiles::set_input_profile_for_player(i, profiles::get_or_create_mp_keyboard_profile_index(i), InputDevice::Keyboard);
        }
    }

    // Player one keeps the keyboard as well, and it is the single-player
    // keyboard profile -- the one the controls tab edits and the one that has
    // the default bindings, where a freshly created multiplayer keyboard
    // profile has none. get_n64_input reads a player's controller and keyboard
    // profiles and merges them, so player one can use either at any moment.
    profiles::set_input_profile_for_player(0, profiles::get_sp_keyboard_profile_index(), InputDevice::Keyboard);
}

void playerassignment::stop_and_close_modal() {"""


def patch(path, anchor, replacement, marker):
    if not path.exists():
        sys.exit(f"missing {path}\nRun: git submodule update --init --recursive")

    text = path.read_text(encoding="utf-8")

    if marker in text:
        print(f"  {path.name} already patched")
        return

    if anchor not in text:
        sys.exit(f"anchor not found in {path}; upstream has changed and this patch needs revisiting")

    path.write_text(text.replace(anchor, replacement, 1), encoding="utf-8")
    print(f"  {path.name} patched")


def main():
    print(">> Patching RecompFrontend/recompinput (players::auto_assign_controllers)...")
    patch(HEADER, HEADER_ANCHOR, HEADER_REPLACEMENT, "auto_assign_controllers")
    patch(SOURCE, SOURCE_ANCHOR, SOURCE_REPLACEMENT, "auto_assign_controllers")


if __name__ == "__main__":
    main()
