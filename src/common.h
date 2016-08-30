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

		int charge;

		char* text;
		bool doctor;

		int mousex, mousey;
		bool mouse_visible;

		bool tutorial;

		bool won;

		int timer;

		struct {
				bool atari;
				bool floppy;
				bool pegasus;
				bool tape;
		} status;

		ALLEGRO_SAMPLE *sample;
		ALLEGRO_SAMPLE_INSTANCE *sample_instance;
};

struct CommonResources* CreateGameData(struct Game *game);
void DestroyGameData(struct Game *game, struct CommonResources *resources);
void StartGame(struct Game *game);
void UpdateStatus(struct Game *game);

typedef enum {
	DRSAUCE_EVENT_SWITCH_SCREEN = 512,
	DRSAUCE_EVENT_STATUS_UPDATE,
	DRSAUCE_EVENT_TUTORIAL
} DRSAUCE_EVENT_TYPE;
