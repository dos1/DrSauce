#define LIBSUPERDERPY_DATA_TYPE struct CommonResources
#include <libsuperderpy.h>

struct CommonResources {
		// Fill in with common data accessible from all gamestates.
		ALLEGRO_BITMAP *atari;
		ALLEGRO_BITMAP *floppy;
		ALLEGRO_BITMAP *pegasus;
		ALLEGRO_BITMAP *tape;
		int offset;
		int current_screen;
		int desired_screen;
		bool forward;
};

struct CommonResources* CreateGameData(struct Game *game);
void DestroyGameData(struct Game *game, struct CommonResources *resources);
void StartGame(struct Game *game);
