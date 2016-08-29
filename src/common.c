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

	data->atari = al_create_bitmap(320, 180);
	data->pegasus = al_create_bitmap(320, 180);
	data->tape = al_create_bitmap(320, 180);
	data->floppy = al_create_bitmap(320, 180);

	data->offset = 0;

	data->text = NULL;
	data->doctor = false;

	return data;
}

void DestroyGameData(struct Game *game, struct CommonResources *resources) {
	al_destroy_bitmap(game->data->atari);
	al_destroy_bitmap(game->data->pegasus);
	al_destroy_bitmap(game->data->tape);
	al_destroy_bitmap(game->data->floppy);
	free(resources);
}

void StartGame(struct Game *game) {
	UnloadAllGamestates(game);

	LoadGamestate(game, "atari");
	LoadGamestate(game, "pegasus");
	LoadGamestate(game, "tape");
	LoadGamestate(game, "floppy");
	LoadGamestate(game, "stage");
	LoadGamestate(game, "hud");

	StartGamestate(game, "atari");
	StartGamestate(game, "pegasus");
	StartGamestate(game, "tape");
	StartGamestate(game, "floppy");
	StartGamestate(game, "stage");
	StartGamestate(game, "hud");
}

