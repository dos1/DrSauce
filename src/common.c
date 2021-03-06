/*! \file common.c
 *  \brief Common stuff that can be used by all gamestates.
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

#include "common.h"
#include <libsuperderpy.h>

struct CommonResources* CreateGameData(struct Game *game) {
	struct CommonResources* data = calloc(1, sizeof(struct CommonResources));

	int flags = al_get_new_bitmap_flags();
	al_add_new_bitmap_flag(ALLEGRO_NO_PRESERVE_TEXTURE);
	data->atari = al_create_bitmap(320, 180);
	data->pegasus = al_create_bitmap(320, 180);
	data->tape = al_create_bitmap(320, 180);
	data->floppy = al_create_bitmap(320, 180);
	al_set_new_bitmap_flags(flags);

	data->offset = 0;

	data->text = NULL;
	data->doctor = false;

	data->sample = al_load_sample(GetDataFilePath(game, "warning.flac"));
	data->sample_instance = al_create_sample_instance(data->sample);
	al_attach_sample_instance_to_mixer(data->sample_instance, game->audio.fx);

	data->charge = 0;

	return data;
}

bool GlobalEventHandler(struct Game *game, ALLEGRO_EVENT *event) {
	if (event->type == ALLEGRO_EVENT_DISPLAY_RESUME_DRAWING) {
		int flags = al_get_new_bitmap_flags();
		al_add_new_bitmap_flag(ALLEGRO_NO_PRESERVE_TEXTURE);
		game->data->atari = al_create_bitmap(320, 180);
		game->data->pegasus = al_create_bitmap(320, 180);
		game->data->tape = al_create_bitmap(320, 180);
		game->data->floppy = al_create_bitmap(320, 180);
		al_set_new_bitmap_flags(flags);
	}
	return false;
}


void DestroyGameData(struct Game *game, struct CommonResources *resources) {
	al_destroy_bitmap(game->data->atari);
	al_destroy_bitmap(game->data->pegasus);
	al_destroy_bitmap(game->data->tape);
	al_destroy_bitmap(game->data->floppy);
	al_destroy_sample_instance(game->data->sample_instance);
	al_destroy_sample(game->data->sample);
	free(resources);
}

void StartGame(struct Game *game) {
	UnloadAllGamestates(game);

	LoadGamestate(game, "intro");
	LoadGamestate(game, "atari");
	LoadGamestate(game, "pegasus");
	LoadGamestate(game, "tape");
	LoadGamestate(game, "floppy");
	LoadGamestate(game, "stage");
	LoadGamestate(game, "hud");

	//StartGamestate(game, "atari");
	//StartGamestate(game, "pegasus");
	//StartGamestate(game, "tape");
	//StartGamestate(game, "floppy");
	//StartGamestate(game, "stage");
	StartGamestate(game, "intro");
	StartGamestate(game, "hud");

	game->data->status.atari = true;
	game->data->status.floppy = true;
	game->data->status.pegasus = true;
	game->data->status.tape = true;
	game->data->won = false;

	game->data->timer = 0;
}

void UpdateStatus(struct Game *game) {
	ALLEGRO_EVENT ev;
	ev.user.type = DRSAUCE_EVENT_STATUS_UPDATE;
	al_emit_user_event(&(game->event_source), &ev, NULL);

	if (!game->data->status.atari || !game->data->status.floppy || !game->data->status.pegasus || !game->data->status.tape) {
		if (!game->data->won) {
			al_play_sample_instance(game->data->sample_instance);
		}
	}
}
