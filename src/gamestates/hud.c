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
		ALLEGRO_FONT *font, *dialog;
		int alpha;
};

int Gamestate_ProgressCount = 1; // number of loading steps as reported by Gamestate_Load

void Gamestate_Logic(struct Game *game, struct GamestateResources* data) {
	// Called 60 times per second. Here you should do all your game logic.
	if (game->data->text && data->alpha < 0) {
		data->alpha+=1;
	}
	if (!game->data->text && data->alpha > -20) {
		data->alpha-=1;
	}
}

void Gamestate_Draw(struct Game *game, struct GamestateResources* data) {
	// Called as soon as possible, but no sooner than next Gamestate_Logic call.
	// Draw everything to the screen here.
	if (!game->data->tutorial) {
		DrawTextWithShadow(data->font, al_map_rgb(255,255,255), 10, game->viewport.height / 2 - 10,
		             ALLEGRO_ALIGN_LEFT, "<");
		DrawTextWithShadow(data->font, al_map_rgb(255,255,255), game->viewport.width - 10, game->viewport.height / 2 - 10,
		             ALLEGRO_ALIGN_RIGHT, ">");
	}

	al_draw_filled_rectangle(0, 0, 320, 20 + data->alpha, al_map_rgba(0,0,0,128));
	if (game->data->text) {
		DrawTextWithShadow(data->dialog, al_map_rgb(255,255,255), game->viewport.width / 2, 5 + data->alpha, ALLEGRO_ALIGN_CENTER, game->data->text);
	}

}

void Gamestate_ProcessEvent(struct Game *game, struct GamestateResources* data, ALLEGRO_EVENT *ev) {
	// Called for each event in Allegro event queue.
	// Here you can handle user input, expiring timers etc.
	if ((ev->type==ALLEGRO_EVENT_KEY_DOWN) && (ev->keyboard.keycode == ALLEGRO_KEY_ESCAPE)) {
		UnloadCurrentGamestate(game); // mark this gamestate to be stopped and unloaded
		// When there are no active gamestates, the engine will quit.
	}
	if (ev->type==ALLEGRO_EVENT_KEY_DOWN) {

		if (ev->keyboard.keycode == ALLEGRO_KEY_FULLSTOP) {
			game->data->skip = true;
		}

	}

	if (game->data->tutorial) {
		return;
	}

	if (ev->type==ALLEGRO_EVENT_KEY_DOWN) {
		if (ev->keyboard.keycode == ALLEGRO_KEY_LEFT) {
			game->data->desired_screen--;
			if (game->data->desired_screen < 0) {
				game->data->desired_screen = 3;
			}
			game->data->forward = false;
			ALLEGRO_EVENT ev;
			ev.user.type = DRSAUCE_EVENT_SWITCH_SCREEN;
			al_emit_user_event(&(game->event_source), &ev, NULL);
		} else if (ev->keyboard.keycode == ALLEGRO_KEY_RIGHT) {
			game->data->desired_screen++;
			if (game->data->desired_screen > 3) {
				game->data->desired_screen = 0;
			}
			game->data->forward = true;
			ALLEGRO_EVENT ev;
			ev.user.type = DRSAUCE_EVENT_SWITCH_SCREEN;
			al_emit_user_event(&(game->event_source), &ev, NULL);
		}
		PrintConsole(game, "KEY desired %d, current %d, forward %d, offset %d", game->data->desired_screen, game->data->current_screen, game->data->forward, game->data->offset);
	}

	if (ev->type==ALLEGRO_EVENT_MOUSE_AXES) {
		game->data->mousex = (ev->mouse.x / (float)al_get_display_width(game->display)) * game->viewport.width;
		game->data->mousey = (ev->mouse.y / (float)al_get_display_height(game->display)) * game->viewport.height;
		game->data->mouse_visible = true;
	}

	if (ev->type == DRSAUCE_EVENT_STATUS_UPDATE) {
		PrintConsole(game, "status: %d%d%d%d", game->data->status.atari, game->data->status.pegasus, game->data->status.tape, game->data->status.floppy);
	}
}

void* Gamestate_Load(struct Game *game, void (*progress)(struct Game*)) {
	// Called once, when the gamestate library is being loaded.
	// Good place for allocating memory, loading bitmaps etc.
	struct GamestateResources *data = malloc(sizeof(struct GamestateResources));
	data->font = al_load_font(GetDataFilePath(game, "fonts/PerfectDOSVGA437.ttf"), 32, 0);
	data->dialog = al_load_font(GetDataFilePath(game, "fonts/MonkeyIsland.ttf"), 8, 0);
	progress(game); // report that we progressed with the loading, so the engine can draw a progress bar
	return data;
}

void Gamestate_Unload(struct Game *game, struct GamestateResources* data) {
	// Called when the gamestate library is being unloaded.
	// Good place for freeing all allocated memory and resources.
	al_destroy_font(data->font);
	al_destroy_font(data->dialog);
	free(data);
}

void Gamestate_Start(struct Game *game, struct GamestateResources* data) {
	// Called when this gamestate gets control. Good place for initializing state,
	// playing music etc.
	data->alpha = -20;
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
