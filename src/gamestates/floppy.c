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
#include <stdio.h>
#include <libsuperderpy.h>

struct GamestateResources {
		// This struct is for every resource allocated and used by your gamestate.
		// It gets created on load and then gets passed around to all other function calls.
		ALLEGRO_BITMAP *pc;
		ALLEGRO_BITMAP *floppy;
		struct Character *floppies;
		struct Character *progress;
		struct Character *cursor;
		int nr_inside;
		bool taken;
		int taken_nr;
		int needed;
		bool needs_change;
		int counter;
		ALLEGRO_FONT *font_screen;
		ALLEGRO_FONT *font_disk;
		int blink;
		int chance;
};

int Gamestate_ProgressCount = 1; // number of loading steps as reported by Gamestate_Load

void Gamestate_Logic(struct Game *game, struct GamestateResources* data) {
	// Called 60 times per second. Here you should do all your game logic.
	AnimateCharacter(game, data->progress, 1);
	SetCharacterPosition(game, data->cursor, game->data->mousex, game->data->mousey, 0);
	data->blink++;

	if (data->needed == data->nr_inside) {
		data->counter--;
		if (data->counter == 0) {
			data->needs_change = true;
			while (data->needed == data->nr_inside) {
				data->needed = (rand() % 8) + 1;
			}
			data->counter = 1500;
			game->data->status.floppy = false;
			UpdateStatus(game);
		}
	}
}

void Gamestate_Draw(struct Game *game, struct GamestateResources* data) {
	// Called as soon as possible, but no sooner than next Gamestate_Logic call.
	// Draw everything to the screen here.
	al_set_target_bitmap(game->data->floppy);
	al_clear_to_color(al_map_rgba(0,0,0,0));
	al_draw_bitmap(data->pc, 1022-960, 20, 0);
	DrawCharacter(game, data->floppies, al_map_rgb(255,255,255), 0);

	if (data->needs_change) {
		if ((data->blink / 30) % 2) {
			char text[8] = "DISK ??";
			snprintf(text, 8, "DISK %d", data->needed);
			DrawTextWithShadow(data->font_screen, al_map_rgb(255,255,255), 320/2 - 37, 60 - 2, ALLEGRO_ALIGN_CENTER, "INSERT");
			DrawTextWithShadow(data->font_screen, al_map_rgb(255,255,255), 320/2 - 37, 75 - 4, ALLEGRO_ALIGN_CENTER, text);
		}
	} else {
		DrawCharacter(game, data->progress, al_map_rgb(255,255,255), 0);
	}


	if (game->data->mouse_visible && !data->taken) {
		DrawCharacter(game, data->cursor, al_map_rgb(255,255,255), 0);
	}
	if (data->taken) {
		al_draw_bitmap(data->floppy, 98, 27, 0);
		char text[8] = "DISK ??";
		snprintf(text, 8, "DISK %d", data->taken_nr);
		al_draw_text(data->font_disk, al_map_rgb(0,0,0), 320/2, 110, ALLEGRO_ALIGN_CENTER, text);
	}
	al_set_target_backbuffer(game->display);
}

void Gamestate_ProcessEvent(struct Game *game, struct GamestateResources* data, ALLEGRO_EVENT *ev) {
	// Called for each event in Allegro event queue.
	// Here you can handle ` input, expiring timers etc.
	if ((ev->type==ALLEGRO_EVENT_KEY_DOWN) && (ev->keyboard.keycode == ALLEGRO_KEY_ESCAPE)) {
		UnloadCurrentGamestate(game); // mark this gamestate to be stopped and unloaded
		// When there are no active gamestates, the engine will quit.
	}

	if (ev->type == DRSAUCE_EVENT_END_TUTORIAL) {
		data->counter = 886;
	}

	if (game->data->current_screen != 3) {
		return;
	}

	if ((ev->type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) && (game->data->mouse_visible)) {
		if (!data->taken && IsOnCharacter(game, data->floppies, game->data->mousex, game->data->mousey)) {
			data->taken = true;
			data->taken_nr = (rand() % 8) + 1;
			if (rand() % data->chance == 0) {
				data->taken_nr = data->needed;
				data->chance = 16;
			} else {
				data->chance--;
			}
		} else if (data->taken) {
			data->taken = false;
			data->chance = 16;
			bool wasgood = (data->nr_inside != data->needed);
			data->nr_inside = data->taken_nr;
			data->needs_change = (data->nr_inside != data->needed);
			game->data->status.floppy = !data->needs_change;

			if (data->needs_change != wasgood) {
				UpdateStatus(game);
			}
		}
	}
}

void* Gamestate_Load(struct Game *game, void (*progress)(struct Game*)) {
	// Called once, when the gamestate library is being loaded.
	// Good place for allocating memory, loading bitmaps etc.
	struct GamestateResources *data = malloc(sizeof(struct GamestateResources));
	progress(game); // report that we progressed with the loading, so the engine can draw a progress bar

	data->pc = al_load_bitmap(GetDataFilePath(game, "pc.png"));
	data->floppy = al_load_bitmap(GetDataFilePath(game, "floppy.png"));

	data->floppies = CreateCharacter(game, "floppies");
	RegisterSpritesheet(game, data->floppies, "stack");
	LoadSpritesheets(game, data->floppies);
	SelectSpritesheet(game, data->floppies, "stack");

	data->progress = CreateCharacter(game, "progress");
	RegisterSpritesheet(game, data->progress, "progress");
	LoadSpritesheets(game, data->progress);
	SelectSpritesheet(game, data->progress, "progress");

	data->cursor = CreateCharacter(game, "cursor");
	RegisterSpritesheet(game, data->cursor, "pointer");
	LoadSpritesheets(game, data->cursor);
	SelectSpritesheet(game, data->cursor, "pointer");

	data->font_disk = al_load_font(GetDataFilePath(game, "fonts/PerfectDOSVGA437.ttf"), 16, 0);
	data->font_screen = al_load_font(GetDataFilePath(game, "fonts/MonkeyIsland.ttf"), 8, 0);

	return data;
}

void Gamestate_Unload(struct Game *game, struct GamestateResources* data) {
	// Called when the gamestate library is being unloaded.
	// Good place for freeing all allocated memory and resources.
	al_destroy_bitmap(data->pc);
	al_destroy_bitmap(data->floppy);
	DestroyCharacter(game, data->floppies);
	DestroyCharacter(game, data->progress);
	DestroyCharacter(game, data->cursor);
	al_destroy_font(data->font_disk);
	al_destroy_font(data->font_screen);
	free(data);
}

void Gamestate_Start(struct Game *game, struct GamestateResources* data) {
	// Called when this gamestate gets control. Good place for initializing state,
	// playing music etc.
	SetCharacterPosition(game, data->floppies, 1128-960, 110, 0);
	SetCharacterPosition(game, data->progress, 1069-960, 66, 0);
	data->needs_change = false;
	data->taken = false;
	data->counter = 1500;
	data->needed = 1;
	data->nr_inside = 1;
	data->chance = 16;
	data->blink = 0;
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
