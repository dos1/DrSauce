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
		ALLEGRO_BITMAP *bg, *stage;
};

int Gamestate_ProgressCount = 1; // number of loading steps as reported by Gamestate_Load

float max(int a, int b) {
	return (a>b) ? a : b;
}

void Gamestate_Logic(struct Game *game, struct GamestateResources* data) {
	// Called 60 times per second. Here you should do all your game logic.
	//if (game->data->offset % 320 == 0) {
	  game->data->current_screen = game->data->offset / 320;
	//}
	//if (game->data->current_screen != game->data->desired_screen) {

		int desired = game->data->desired_screen;
		if (game->data->forward && game->data->desired_screen < game->data->current_screen) {
			desired += 4;
		}
		//else if (!game->data->forward && game->data->desired_screen > game->data->current_screen) {
		//	desired -= 4;
		//}

		if (game->data->offset != game->data->desired_screen * 320) {
			game->data->offset += max(abs((desired*320 - game->data->offset)/8), 1) * (game->data->forward ? 1 : -1);
		}

		if (game->data->offset >= 4*320) {
			game->data->offset -= 4*320;
		}
		if (game->data->offset < 0) {
			game->data->offset += 4*320;
		}

		//PrintConsole(game, "desired %d, current %d, forward %d, offset %d", game->data->desired_screen, game->data->current_screen, game->data->forward, game->data->offset);

	//}
}

void Gamestate_Draw(struct Game *game, struct GamestateResources* data) {
	// Called as soon as possible, but no sooner than next Gamestate_Logic call.
	// Draw everything to the screen here.
	al_set_target_bitmap(data->stage);
	al_draw_bitmap(data->bg, 0, 0, 0);
	al_draw_bitmap(game->data->atari, 0, 0, 0);
	al_draw_bitmap(game->data->pegasus, 320, 0, 0);
	al_draw_bitmap(game->data->tape, 320*2, 0, 0);
	al_draw_bitmap(game->data->floppy, 320*3, 0, 0);

	al_set_target_backbuffer(game->display);
	al_draw_bitmap(data->stage, -game->data->offset, 0, 0);
	al_draw_bitmap(data->stage, -game->data->offset+4*320, 0, 0);
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
	data->bg = al_load_bitmap(GetDataFilePath(game, "stage.png"));
	data->stage = al_create_bitmap(320*4, 180);
	progress(game); // report that we progressed with the loading, so the engine can draw a progress bar
	return data;
}

void Gamestate_Unload(struct Game *game, struct GamestateResources* data) {
	// Called when the gamestate library is being unloaded.
	// Good place for freeing all allocated memory and resources.
	al_destroy_bitmap(data->bg);
	al_destroy_bitmap(data->stage);
	free(data);
}

void Gamestate_Start(struct Game *game, struct GamestateResources* data) {
	// Called when this gamestate gets control. Good place for initializing state,
	// playing music etc.

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
