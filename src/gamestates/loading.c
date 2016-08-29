/*! \file loading.c
 *  \brief Loading screen.
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

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include <libsuperderpy.h>

/*! \brief Resources used by Loading state. */
struct LoadingResources {
		ALLEGRO_BITMAP *bg;
};

void Draw(struct Game *game, struct LoadingResources *data, float p) {
	if ((p != 0.0) && (p != 1.0)) {
		al_draw_bitmap(data->bg,0,0,0);
//		SetupViewport(game, game->viewport_config);
		al_flip_display();
                al_draw_bitmap(data->bg,0,0,0);
		al_rest(0.2);
	} else {
		al_clear_to_color(al_map_rgb(0,0,0));
//		SetupViewport(game, game->viewport_config);
		al_flip_display();
		al_rest(0.5);
                al_clear_to_color(al_map_rgb(0,0,0));
	}
}

void* Load(struct Game *game) {
	struct LoadingResources *data = malloc(sizeof(struct LoadingResources));

	data->bg = al_load_bitmap(GetDataFilePath(game, "loading.png"));

	return data;
}
void Unload(struct Game *game, struct LoadingResources *data) {
	al_destroy_bitmap(data->bg);
	free(data);
}

void Start(struct Game *game, struct LoadingResources *data) {}
void Stop(struct Game *game, struct LoadingResources *data) {}
