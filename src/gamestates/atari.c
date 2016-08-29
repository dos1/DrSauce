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
		ALLEGRO_BITMAP *coal;
		struct Character *shovel;
		struct Character *atari;
		struct Character *meter;
};

int Gamestate_ProgressCount = 1; // number of loading steps as reported by Gamestate_Load

void Gamestate_Logic(struct Game *game, struct GamestateResources* data) {
	// Called 60 times per second. Here you should do all your game logic.

}

void Gamestate_Draw(struct Game *game, struct GamestateResources* data) {
	// Called as soon as possible, but no sooner than next Gamestate_Logic call.
	// Draw everything to the screen here.
	al_set_target_bitmap(game->data->atari);
	al_draw_bitmap(data->coal, 7, 52, 0);
	DrawCharacter(game, data->shovel, al_map_rgb(255,255,255), 0);
	DrawCharacter(game, data->atari, al_map_rgb(255,255,255), 0);
	DrawCharacter(game, data->meter, al_map_rgb(255,255,255), 0);
	al_set_target_backbuffer(game->display);
}

void Gamestate_ProcessEvent(struct Game *game, struct GamestateResources* data, ALLEGRO_EVENT *ev) {
	// Called for each event in Allegro event queue.
	// Here you can handle user input, expiring timers etc.
	if ((ev->type==ALLEGRO_EVENT_KEY_DOWN) && (ev->keyboard.keycode == ALLEGRO_KEY_ESCAPE)) {
		UnloadCurrentGamestate(game); // mark this gamestate to be stopped and unloaded
		// When there are no active gamestates, the engine will quit.
	}
}

void* Gamestate_Load(struct Game *game, void (*progress)(struct Game*)) {
	// Called once, when the gamestate library is being loaded.
	// Good place for allocating memory, loading bitmaps etc.
	struct GamestateResources *data = malloc(sizeof(struct GamestateResources));
	progress(game); // report that we progressed with the loading, so the engine can draw a progress bar

	data->coal = al_load_bitmap(GetDataFilePath(game, "coal.png"));

	data->atari = CreateCharacter(game, "atari");
	RegisterSpritesheet(game, data->atari, "burn");
	LoadSpritesheets(game, data->atari);
	SelectSpritesheet(game, data->atari, "burn");

	data->shovel = CreateCharacter(game, "shovel");
	RegisterSpritesheet(game, data->shovel, "shovel");
	RegisterSpritesheet(game, data->shovel, "use");
	RegisterSpritesheet(game, data->shovel, "full");
	LoadSpritesheets(game, data->shovel);
	SelectSpritesheet(game, data->shovel, "use");

	data->meter = CreateCharacter(game, "meter");
	RegisterSpritesheet(game, data->meter, "meter");
	LoadSpritesheets(game, data->meter);
	SelectSpritesheet(game, data->meter, "meter");

	return data;
}

void Gamestate_Unload(struct Game *game, struct GamestateResources* data) {
	// Called when the gamestate library is being unloaded.
	// Good place for freeing all allocated memory and resources.
	al_destroy_bitmap(data->coal);
	DestroyCharacter(game, data->atari);
	DestroyCharacter(game, data->shovel);
	DestroyCharacter(game, data->meter);
	free(data);
}

void Gamestate_Start(struct Game *game, struct GamestateResources* data) {
	// Called when this gamestate gets control. Good place for initializing state,
	// playing music etc.
	SetCharacterPosition(game, data->atari, 156, 89, 0);
	SetCharacterPosition(game, data->shovel, 72, 46, 0);
	SetCharacterPosition(game, data->meter, 213, 132, 0);
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
