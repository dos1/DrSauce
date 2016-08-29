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
#include <math.h>
#include <libsuperderpy.h>

struct GamestateResources {
		// This struct is for every resource allocated and used by your gamestate.
		// It gets created on load and then gets passed around to all other function calls.
		ALLEGRO_BITMAP *coal;
		struct Character *shovel;
		struct Character *atari;
		struct Character *meter;
		bool shovel_locked, shovel_full;
		struct Timeline *timeline;

		float temperature;
		int coal_amount;

		int counter;
};

int Gamestate_ProgressCount = 1; // number of loading steps as reported by Gamestate_Load

void Gamestate_Logic(struct Game *game, struct GamestateResources* data) {
	// Called 60 times per second. Here you should do all your game logic.
	if (!data->shovel_locked) {
		SetCharacterPosition(game, data->shovel, game->data->mousex, game->data->mousey - 70, 0);
	}

	float old_temp = data->temperature;

	if (data->counter % 60 == 0) {
		if (data->temperature >= 10) {
			data->temperature += data->coal_amount / (5.0*data->temperature/50.0);
		} else {
			data->temperature += data->coal_amount / 2.0;
		}
		data->temperature -= 4;
		data->coal_amount --;
	}

	if (data->temperature > 100) {
		data->temperature = 100;
	}

	if (data->temperature < 0) {
		data->temperature = 0;
	}

	if ((old_temp > 25) && (data->temperature <= 25)) {
		SelectSpritesheet(game, data->meter, "meter-red");
		game->data->status.atari = false;
		ALLEGRO_EVENT ev;
		ev.user.type = DRSAUCE_EVENT_STATUS_UPDATE;
		al_emit_user_event(&(game->event_source), &ev, NULL);
	}
	if ((old_temp > 75) && (data->temperature <= 75)) {
		SelectSpritesheet(game, data->meter, "meter-orange");
		game->data->status.atari = true;
		ALLEGRO_EVENT ev;
		ev.user.type = DRSAUCE_EVENT_STATUS_UPDATE;
		al_emit_user_event(&(game->event_source), &ev, NULL);
	}
	if ((old_temp <= 25) && (data->temperature > 25)) {
		SelectSpritesheet(game, data->meter, "meter-orange");
		game->data->status.atari = true;
		ALLEGRO_EVENT ev;
		ev.user.type = DRSAUCE_EVENT_STATUS_UPDATE;
		al_emit_user_event(&(game->event_source), &ev, NULL);
	}
	if ((old_temp <= 75) && (data->temperature > 75)) {
		SelectSpritesheet(game, data->meter, "meter-green");
		game->data->status.atari = true;
		ALLEGRO_EVENT ev;
		ev.user.type = DRSAUCE_EVENT_STATUS_UPDATE;
		al_emit_user_event(&(game->event_source), &ev, NULL);
	}


	data->counter++;

	//PrintConsole(game, "temp %f, coal %d", data->temperature, data->coal_amount);

	AnimateCharacter(game, data->atari, 1);
	AnimateCharacter(game, data->meter, 1);
	TM_Process(data->timeline);
}

void Gamestate_Draw(struct Game *game, struct GamestateResources* data) {
	// Called as soon as possible, but no sooner than next Gamestate_Logic call.
	// Draw everything to the screen here.
	al_set_target_bitmap(game->data->atari);
	al_clear_to_color(al_map_rgba(0,0,0,0));
	al_draw_bitmap(data->coal, 7, 52, 0);

	DrawCharacter(game, data->atari, al_map_rgb(255,255,255), 0);
	DrawCharacter(game, data->meter, al_map_rgb(255,255,255), 0);
	int x = GetCharacterX(game, data->meter) + 18, y = GetCharacterY(game, data->meter) + 11;
	float angle = (data->temperature / 100.0) * ALLEGRO_PI;
	al_draw_line(x, y, x-(cos(angle)*11), y-(sin(angle)*9), al_map_rgb(50,50,50), 1);
	al_draw_filled_rectangle(x-1, y, x+1, y+2, al_map_rgb(0,0,0));

	if (game->data->mouse_visible) {
		if (game->data->mousex > 140 && !data->shovel_locked && !data->shovel_full) {
			DrawCharacter(game, data->shovel, al_map_rgb(255,255,255), ALLEGRO_FLIP_HORIZONTAL);
		} else {
			DrawCharacter(game, data->shovel, al_map_rgb(255,255,255), 0);
		}
	}

	if (game->data->mousey < 120) {
		DrawCharacter(game, data->atari, al_map_rgb(255,255,255), 0);
		DrawCharacter(game, data->meter, al_map_rgb(255,255,255), 0);
		int x = GetCharacterX(game, data->meter) + 18, y = GetCharacterY(game, data->meter) + 11;
		float angle = (data->temperature / 100.0) * ALLEGRO_PI;
		al_draw_line(x, y, x-(cos(angle)*11), y-(sin(angle)*9), al_map_rgb(50,50,50), 1);
		al_draw_filled_rectangle(x-1, y, x+1, y+2, al_map_rgb(0,0,0));
	}

	al_set_target_backbuffer(game->display);
}

bool FillShovel(struct Game *game, struct TM_Action *action, enum TM_ActionState state) {
	struct GamestateResources *data = TM_GetArg(action->arguments, 0);
	if (state == TM_ACTIONSTATE_RUNNING) {
		SelectSpritesheet(game, data->shovel, "full");
		data->shovel_locked = false;
		data->shovel_full = true;
		SetCharacterPosition(game, data->shovel, game->data->mousex, game->data->mousey - 70, 0);
	}
	return true;
}

void Gamestate_ProcessEvent(struct Game *game, struct GamestateResources* data, ALLEGRO_EVENT *ev) {
	// Called for each event in Allegro event queue.
	// Here you can handle user input, expiring timers etc.
	TM_HandleEvent(data->timeline, ev);
	if ((ev->type==ALLEGRO_EVENT_KEY_DOWN) && (ev->keyboard.keycode == ALLEGRO_KEY_ESCAPE)) {
		UnloadCurrentGamestate(game); // mark this gamestate to be stopped and unloaded
		// When there are no active gamestates, the engine will quit.
	}

	if (ev->type == DRSAUCE_EVENT_SWITCH_SCREEN) {
		SelectSpritesheet(game, data->shovel, "shovel");
		data->shovel_locked = false;
		data->shovel_full = false;
		TM_CleanQueue(data->timeline);
	}

	if (game->data->current_screen != 0) {
		return;
	}

	if ((ev->type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) && (game->data->mouse_visible)) {
		if ((game->data->mousex < 140) && (!data->shovel_locked) && (!data->shovel_full)) {
			SelectSpritesheet(game, data->shovel, "use");
			data->shovel_locked = true;
			SetCharacterPosition(game, data->shovel, 72, 46, 0);
			TM_AddDelay(data->timeline, 500);
			TM_AddAction(data->timeline, FillShovel, TM_AddToArgs(NULL, 1, data), "FillShovel");
		}

		if ((game->data->mousex > 140) && (game->data->mousey > 90) && (game->data->mousey < 120) && (data->shovel_full)) {
			SelectSpritesheet(game, data->shovel, "shovel");
			data->shovel_full = false;
			if (data->coal_amount < 0) {
				data->coal_amount = 0;
			}
			data->coal_amount += 6;
		}
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
	SelectSpritesheet(game, data->shovel, "shovel");

	data->meter = CreateCharacter(game, "meter");
	RegisterSpritesheet(game, data->meter, "meter");
	RegisterSpritesheet(game, data->meter, "meter-red");
	RegisterSpritesheet(game, data->meter, "meter-orange");
	RegisterSpritesheet(game, data->meter, "meter-green");
	LoadSpritesheets(game, data->meter);
	SelectSpritesheet(game, data->meter, "meter-orange");

	data->timeline = TM_Init(game, "atari");

	return data;
}

void Gamestate_Unload(struct Game *game, struct GamestateResources* data) {
	// Called when the gamestate library is being unloaded.
	// Good place for freeing all allocated memory and resources.
	al_destroy_bitmap(data->coal);
	DestroyCharacter(game, data->atari);
	DestroyCharacter(game, data->shovel);
	DestroyCharacter(game, data->meter);
	TM_Destroy(data->timeline);
	free(data);
}

void Gamestate_Start(struct Game *game, struct GamestateResources* data) {
	// Called when this gamestate gets control. Good place for initializing state,
	// playing music etc.
	SetCharacterPosition(game, data->atari, 156, 89, 0);
	SetCharacterPosition(game, data->shovel, 72, 46, 0);
	SetCharacterPosition(game, data->meter, 213, 132, 0);
	data->shovel_locked = false;
	data->shovel_full = false;

	data->temperature = 50;
	data->coal_amount = 20;
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
