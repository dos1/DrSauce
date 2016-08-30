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
		struct Timeline *timeline;
		ALLEGRO_BITMAP *bg;
		ALLEGRO_BITMAP *sos;
		ALLEGRO_BITMAP *machine;
		ALLEGRO_BITMAP *bg1, *bg2;
		ALLEGRO_BITMAP *bird;
		ALLEGRO_AUDIO_STREAM *music, *music2;

		ALLEGRO_SAMPLE *sample;
		ALLEGRO_SAMPLE_INSTANCE *sample_instance;

		bool show;
		bool finished;
		int rotation;
};

int Gamestate_ProgressCount = 3; // number of loading steps as reported by Gamestate_Load

void Gamestate_Logic(struct Game *game, struct GamestateResources* data) {
	// Called 60 times per second. Here you should do all your game logic.
	TM_Process(data->timeline);
}

void Gamestate_Draw(struct Game *game, struct GamestateResources* data) {
	// Called as soon as possible, but no sooner than next Gamestate_Logic call.
	// Draw everything to the screen here.
	al_draw_bitmap(data->bg, 0, 0, 0);
	al_draw_bitmap(data->bird, 0, 0 ,0);
	if (data->show) {
		al_draw_bitmap(data->machine, 0, 0 ,0);
		al_draw_bitmap(data->sos, 0, 0 ,0);
	}
}

void Gamestate_ProcessEvent(struct Game *game, struct GamestateResources* data, ALLEGRO_EVENT *ev) {
	// Called for each event in Allegro event queue.
	// Here you can handle user input, expiring timers etc.
	TM_HandleEvent(data->timeline, ev);
	if ((ev->type==ALLEGRO_EVENT_KEY_DOWN) && (ev->keyboard.keycode == ALLEGRO_KEY_ESCAPE)) {
		UnloadCurrentGamestate(game); // mark this gamestate to be stopped and unloaded
		// When there are no active gamestates, the engine will quit.
	}
}


bool TimeTravel(struct Game *game, struct TM_Action *action, enum TM_ActionState state) {
	struct GamestateResources *data = TM_GetArg(action->arguments, 0);
	if (state == TM_ACTIONSTATE_RUNNING) {
		data->show = true;
		al_play_sample_instance(data->sample_instance);
	}
	return true;
}

bool StartOthers(struct Game *game, struct TM_Action *action, enum TM_ActionState state) {
	//struct GamestateResources *data = TM_GetArg(action->arguments, 0);
	if (state == TM_ACTIONSTATE_RUNNING) {
		StartGamestate(game, "atari");
		StartGamestate(game, "pegasus");
		StartGamestate(game, "tape");
		StartGamestate(game, "floppy");
		StartGamestate(game, "stage");
	}
	return true;
}

bool Finish(struct Game *game, struct TM_Action *action, enum TM_ActionState state) {
	struct GamestateResources *data = TM_GetArg(action->arguments, 0);
	if (state == TM_ACTIONSTATE_RUNNING) {
		data->finished = true;
		game->data->tutorial = false;
		game->data->desired_screen=2;
		game->data->forward = true;
		game->data->charge=0;
		al_set_audio_stream_playing(data->music, false);
		al_set_audio_stream_playing(data->music2, true);

		int x, y;
		al_get_mouse_cursor_position(&x, &y);
		game->data->mousex = (x / (float)al_get_display_width(game->display)) * game->viewport.width;
		game->data->mousey = (y / (float)al_get_display_height(game->display)) * game->viewport.height;
		game->data->mouse_visible = true;

	}
	return true;
}

bool Rotate(struct Game *game, struct TM_Action *action, enum TM_ActionState state) {
	struct GamestateResources *data = TM_GetArg(action->arguments, 0);
	if (state == TM_ACTIONSTATE_RUNNING) {
		PrintConsole(game, "rotation %d", data->rotation);
		data->rotation++;
		if (data->rotation == 60) {
			data->rotation = 0;
			game->data->desired_screen++;
			if (game->data->desired_screen > 3) {
				game->data->desired_screen = 0;
			}
			game->data->forward = true;
		}
	}
	return !game->data->tutorial;
}

bool Speak(struct Game *game, struct TM_Action *action, enum TM_ActionState state) {
//	struct GamestateResources *data = TM_GetArg(action->arguments, 0);
	ALLEGRO_AUDIO_STREAM *stream = TM_GetArg(action->arguments, 1);
	char *text = TM_GetArg(action->arguments, 2);

	if (state == TM_ACTIONSTATE_INIT) {
		al_set_audio_stream_playing(stream, false);
		al_set_audio_stream_playmode(stream, ALLEGRO_PLAYMODE_ONCE);
	}

	if (state == TM_ACTIONSTATE_START) {
		//al_rewind_audio_stream(stream);
		al_attach_audio_stream_to_mixer(stream, game->audio.voice);
		al_set_audio_stream_playing(stream, true);

		game->data->text = text;
	}

	if (state == TM_ACTIONSTATE_RUNNING) {
		return !al_get_audio_stream_playing(stream);
	}

	if (state == TM_ACTIONSTATE_DESTROY) {
		al_destroy_audio_stream(stream);
		game->data->text = NULL;
	}
	return false;
}

void* Gamestate_Load(struct Game *game, void (*progress)(struct Game*)) {
	// Called once, when the gamestate library is being loaded.
	// Good place for allocating memory, loading bitmaps etc.
	struct GamestateResources *data = malloc(sizeof(struct GamestateResources));
	data->timeline = TM_Init(game, "intro");
	progress(game); // report that we progressed with the loading, so the engine can draw a progress bar
	data->bg = al_load_bitmap(GetDataFilePath(game, "bg.png"));
	progress(game); // report that we progressed with the loading, so the engine can draw a progress bar
	data->music = al_load_audio_stream(GetDataFilePath(game, "music1.flac"), 4, 1024);
	al_attach_audio_stream_to_mixer(data->music, game->audio.music);
	al_set_audio_stream_playmode(data->music, ALLEGRO_PLAYMODE_LOOP);

	data->music2 = al_load_audio_stream(GetDataFilePath(game, "music2.flac"), 4, 1024);
	al_set_audio_stream_playing(data->music2, false);
	al_attach_audio_stream_to_mixer(data->music2, game->audio.music);
	al_set_audio_stream_playmode(data->music2, ALLEGRO_PLAYMODE_LOOP);

	progress(game); // report that we progressed with the loading, so the engine can draw a progress bar

	data->machine = al_load_bitmap(GetDataFilePath(game, "machin.png"));
	data->sos = al_load_bitmap(GetDataFilePath(game, "dr.png"));
	data->bird = al_load_bitmap(GetDataFilePath(game, "pidgey.png"));

	data->sample = al_load_sample(GetDataFilePath(game, "boom.flac"));
	data->sample_instance = al_create_sample_instance(data->sample);
	al_attach_sample_instance_to_mixer(data->sample_instance, game->audio.fx);


	TM_AddAction(data->timeline, Speak, TM_AddToArgs(NULL, 3, data, al_load_audio_stream(GetDataFilePath(game, "voice/0.flac"), 4, 1024),
	                                                 "A crazy scientist from the future"), "speak");
	TM_AddAction(data->timeline, Speak, TM_AddToArgs(NULL, 3, data, al_load_audio_stream(GetDataFilePath(game, "voice/1.flac"), 4, 1024),
	                                                 "built a time machine"), "speak");
	TM_AddAction(data->timeline, Speak, TM_AddToArgs(NULL, 3, data, al_load_audio_stream(GetDataFilePath(game, "voice/2.flac"), 4, 1024),
	                                                 "and he went back in time."), "speak");

	TM_AddDelay(data->timeline, 500);
	TM_AddAction(data->timeline, TimeTravel, TM_AddToArgs(NULL, 1, data), "timetravel");
	TM_AddDelay(data->timeline, 1500);

	TM_AddAction(data->timeline, Speak, TM_AddToArgs(NULL, 3, data, al_load_audio_stream(GetDataFilePath(game, "voice/3.flac"), 4, 1024),
	                                                 "Unfortunately, his time machine broke!"), "speak");
	TM_AddAction(data->timeline, Speak, TM_AddToArgs(NULL, 3, data, al_load_audio_stream(GetDataFilePath(game, "voice/4a.flac"), 4, 1024),
	                                                 "Oh oh!"), "speak");
	TM_AddAction(data->timeline, Speak, TM_AddToArgs(NULL, 3, data, al_load_audio_stream(GetDataFilePath(game, "voice/4b.flac"), 4, 1024),
	                                                 "- said crazy scientist"), "speak");

	//---------------
	TM_AddDelay(data->timeline, 250);
	TM_AddAction(data->timeline, StartOthers, TM_AddToArgs(NULL, 1, data), "start");
	TM_AddDelay(data->timeline, 250);
	TM_AddQueuedBackgroundAction(data->timeline, Rotate, TM_AddToArgs(NULL, 1, data), 0, "rotate");

	TM_AddAction(data->timeline, Speak, TM_AddToArgs(NULL, 3, data, al_load_audio_stream(GetDataFilePath(game, "voice/5.flac"), 4, 1024),
	                                                 "Now he got some pieces of ancient technology"), "speak");
	TM_AddAction(data->timeline, Speak, TM_AddToArgs(NULL, 3, data, al_load_audio_stream(GetDataFilePath(game, "voice/6.flac"), 4, 1024),
	                                                 "and he's trying to fix his time machine."), "speak");
	TM_AddAction(data->timeline, Speak, TM_AddToArgs(NULL, 3, data, al_load_audio_stream(GetDataFilePath(game, "voice/7.flac"), 4, 1024),
	                                                 "I need all of these working together!"), "speak");
	TM_AddAction(data->timeline, Speak, TM_AddToArgs(NULL, 3, data, al_load_audio_stream(GetDataFilePath(game, "voice/8.flac"), 4, 1024),
	                                                 "- said crazy scientist"), "speak");
//		TM_AddAction(data->timeline, StartOthers, TM_AddToArgs(NULL, 1, data), "start");
TM_AddAction(data->timeline, Finish, TM_AddToArgs(NULL, 1, data), "finish");

  return data;
}

void Gamestate_Unload(struct Game *game, struct GamestateResources* data) {
	// Called when the gamestate library is being unloaded.
	// Good place for freeing all allocated memory and resources.
	TM_Destroy(data->timeline);
	free(data);
}

void Gamestate_Start(struct Game *game, struct GamestateResources* data) {
	// Called when this gamestate gets control. Good place for initializing state,
	// playing music etc.
	data->show = false;
	data->finished = false;
	game->data->tutorial = true;
	data->rotation = 0;
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
