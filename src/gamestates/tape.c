/*! \file empty.c
 *  \brief Empty gamestate.
 */
/*
 * Copyright (c) Sebastian Krzyszkowiak <dos@dosowisko.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include "../common.h"
#include <libsuperderpy.h>

struct GamestateResources {
		// This struct is for every resource allocated and used by your gamestate.
		// It gets created on load and then gets passed around to all other function calls.
		struct Character *tape;
		struct Character *drive;
		struct Character *timemachine;
		struct Character *status;
};

int Gamestate_ProgressCount = 1; // number of loading steps as reported by Gamestate_Load

void Gamestate_Logic(struct Game *game, struct GamestateResources* data) {
	// Called 60 times per second. Here you should do all your game logic.
	AnimateCharacter(game, data->status, 1);

}

void Gamestate_Draw(struct Game *game, struct GamestateResources* data) {
	// Called as soon as possible, but no sooner than next Gamestate_Logic call.
	// Draw everything to the screen here.
	al_set_target_bitmap(game->data->tape);
	DrawCharacter(game, data->drive, al_map_rgb(255,255,255), 0);
	DrawCharacter(game, data->status, al_map_rgb(255,255,255), 0);
	DrawCharacter(game, data->timemachine, al_map_rgb(255,255,255), 0);
//	DrawCharacter(game, data->tape, al_map_rgb(255,255,255), 0);
	al_set_target_backbuffer(game->display);
}

void Gamestate_ProcessEvent(struct Game *game, struct GamestateResources* data, ALLEGRO_EVENT *ev) {
	// Called for each event in Allegro event queue.
	// Here you can handle user input, expiring timers etc.
	if ((ev->type==ALLEGRO_EVENT_KEY_DOWN) && (ev->keyboard.keycode == ALLEGRO_KEY_ESCAPE)) {
		UnloadCurrentGamestate(game); // mark this gamestate to be stopped and unloaded
		// When there are no active gamestates, the engine will quit.
	}

	if (ev->type == DRSAUCE_EVENT_STATUS_UPDATE) {
		char name[5] = "0000";
		if (game->data->status.atari) {
			name[0] = '1';
		}
		if (game->data->status.pegasus) {
			name[1] = '1';
		}
		if (game->data->status.tape) {
			name[2] = '1';
		}
		if (game->data->status.floppy) {
			name[3] = '1';
		}
		SelectSpritesheet(game, data->status, name);
	}

	if (game->data->current_screen != 2) {
		return;
	}

}

void* Gamestate_Load(struct Game *game, void (*progress)(struct Game*)) {
	// Called once, when the gamestate library is being loaded.
	// Good place for allocating memory, loading bitmaps etc.
	struct GamestateResources *data = malloc(sizeof(struct GamestateResources));
	progress(game); // report that we progressed with the loading, so the engine can draw a progress bar

	data->drive = CreateCharacter(game, "drive");
	RegisterSpritesheet(game, data->drive, "working");
	LoadSpritesheets(game, data->drive);
	SelectSpritesheet(game, data->drive, "working");

	data->tape = CreateCharacter(game, "tape");
	RegisterSpritesheet(game, data->tape, "fixing");
	LoadSpritesheets(game, data->tape);
	SelectSpritesheet(game, data->tape, "fixing");

	data->status = CreateCharacter(game, "status");
	RegisterSpritesheet(game, data->status, "0000");
	RegisterSpritesheet(game, data->status, "0001");
	RegisterSpritesheet(game, data->status, "0010");
	RegisterSpritesheet(game, data->status, "0011");
	RegisterSpritesheet(game, data->status, "0100");
	RegisterSpritesheet(game, data->status, "0101");
	RegisterSpritesheet(game, data->status, "0110");
	RegisterSpritesheet(game, data->status, "0111");
	RegisterSpritesheet(game, data->status, "1000");
	RegisterSpritesheet(game, data->status, "1001");
	RegisterSpritesheet(game, data->status, "1010");
	RegisterSpritesheet(game, data->status, "1011");
	RegisterSpritesheet(game, data->status, "1100");
	RegisterSpritesheet(game, data->status, "1101");
	RegisterSpritesheet(game, data->status, "1110");
	RegisterSpritesheet(game, data->status, "1111");
	LoadSpritesheets(game, data->status);
	SelectSpritesheet(game, data->status, "1111");

	data->timemachine = CreateCharacter(game, "timemachine");
	RegisterSpritesheet(game, data->timemachine, "charging");
	LoadSpritesheets(game, data->timemachine);
	SelectSpritesheet(game, data->timemachine, "charging");

	return data;
}

void Gamestate_Unload(struct Game *game, struct GamestateResources* data) {
	// Called when the gamestate library is being unloaded.
	// Good place for freeing all allocated memory and resources.
	DestroyCharacter(game, data->status);
	DestroyCharacter(game, data->timemachine);
	DestroyCharacter(game, data->tape);
	DestroyCharacter(game, data->drive);
	free(data);
}

void Gamestate_Start(struct Game *game, struct GamestateResources* data) {
	// Called when this gamestate gets control. Good place for initializing state,
	// playing music etc.
	SetCharacterPosition(game, data->status, 776-640, 131, 0);
	SetCharacterPosition(game, data->timemachine, 848-640, 12, 0);
	SetCharacterPosition(game, data->tape, 82, 25, 0);
	SetCharacterPosition(game, data->drive, 669-640, 108, 0);
}

void Gamestate_Stop(struct Game *game, struct GamestateResources* data) {
	// Called when gamestate gets stopped. Stop timers, music etc. here.
}

void Gamestate_Pause(struct Game *game, struct GamestateResources* data) {
	// Called when gamestate gets paused (so only Draw is being called, no Logic not ProcessEvent)
	// Pause your timers here.
}

void Gamestate_Resume(struct Game *game, struct GamestateResources* data) {
	// Called when gamestate gets resumed. Resume your timers here.
}

// Ignore this for now.
// TODO: Check, comment, refine and/or remove:
void Gamestate_Reload(struct Game *game, struct GamestateResources* data) {}
