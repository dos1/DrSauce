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

		char* text;
		bool doctor;

		int mousex, mousey;
		bool mouse_visible;

		struct {
				bool atari;
				bool floppy;
				bool pegasus;
				bool tape;
		} status;
};

struct CommonResources* CreateGameData(struct Game *game);
void DestroyGameData(struct Game *game, struct CommonResources *resources);
void StartGame(struct Game *game);

typedef enum {
	DRSAUCE_EVENT_SWITCH_SCREEN = 512,
	DRSAUCE_EVENT_STATUS_UPDATE,
	DRSAUCE_EVENT_TUTORIAL
} DRSAUCE_EVENT_TYPE;
