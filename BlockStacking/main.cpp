/**
 * John Novak
 * janovak@mtu.edu
 * 06/16/2015
 *
 * A simple game where you move the ground to build a stack of blocks
 * with blocks falling from the sky.
 */

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <bitset>
#include <chrono>
#include <iostream>
#include <memory>
#include <random>
#include <ratio>
#include <string>
#include <time.h>
#include <vector>

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>

#include "components.h"
#include "entities.h"
#include "systems.h"

using namespace std;

const unsigned int FPS = 60;
const unsigned int TILESIZE = 32;
const unsigned int WIDTH = 512;
const unsigned int HEIGHT = 512;

void validate(bool ptr, string errorMessage) {
	if (!ptr) {
		cerr << errorMessage << endl;
		exit(-1);
	}
}

int main() {
	auto displayDeleter = [](ALLEGRO_DISPLAY* d) { al_destroy_display(d); };
	unique_ptr<ALLEGRO_DISPLAY, decltype(displayDeleter)> display;
	auto eventQueueDeleter = [](ALLEGRO_EVENT_QUEUE* eq) { al_destroy_event_queue(eq); };
	unique_ptr<ALLEGRO_EVENT_QUEUE, decltype(eventQueueDeleter)> eventQueue;
	auto timerDeleter = [](ALLEGRO_TIMER* t) { al_destroy_timer(t); };
	unique_ptr<ALLEGRO_TIMER, decltype(timerDeleter)> timer;
	auto fontDeleter = [](ALLEGRO_FONT* f) { al_destroy_font(f); };
	unique_ptr<ALLEGRO_FONT, decltype(fontDeleter)> font;
	vector<bool> keys(256, false);
	bool redraw = true;
	bool betweenLevels = false;
	int score = 0;
	unsigned int level = 1;
	World world = World();

	validate(al_init(), "Failed to initialize Allegro");
	al_init_font_addon();
	validate(al_init_ttf_addon(), "Failed to initialize tff addon");
	validate(al_install_keyboard(), "Failed to install keyboard");
	display = unique_ptr<ALLEGRO_DISPLAY, decltype(displayDeleter)>(al_create_display(WIDTH, HEIGHT), displayDeleter);
	validate(display.get(), "Failed to create display");
	eventQueue = unique_ptr<ALLEGRO_EVENT_QUEUE, decltype(eventQueueDeleter)>(al_create_event_queue(), eventQueueDeleter);
	validate(eventQueue.get(), "Failed to create event queue");
	timer = unique_ptr<ALLEGRO_TIMER, decltype(timerDeleter)>(al_create_timer(1 / (float)FPS), timerDeleter);
	validate(timer.get(), "Failed to create timer");
	ALLEGRO_PATH* path = al_get_standard_path(ALLEGRO_EXENAME_PATH);
	font = unique_ptr<ALLEGRO_FONT, decltype(fontDeleter)>(al_load_font("arial.ttf", 72, 0), fontDeleter);
	validate(font.get(), "Failed to load arial");
	srand(time(NULL));

	al_register_event_source(eventQueue.get(), al_get_display_event_source(display.get()));
	al_register_event_source(eventQueue.get(), al_get_timer_event_source(timer.get()));
	al_register_event_source(eventQueue.get(), al_get_keyboard_event_source());

	// Initialize the player
	generateBase(world, display.get());
	al_start_timer(timer.get());
	auto start = chrono::system_clock::now();
	while (true) {
		ALLEGRO_EVENT ev;
		al_wait_for_event(eventQueue.get(), &ev);
		if (ev.type == ALLEGRO_EVENT_TIMER) {
			redraw = true;
		}
		else if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {
			keys[ev.keyboard.keycode] = true;
		}
		else if (ev.type == ALLEGRO_EVENT_KEY_UP) {
			keys[ev.keyboard.keycode] = false;
		}
		else if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
			break;
		}
		auto deltaT = chrono::duration_cast<chrono::seconds>(chrono::system_clock::now() - start);
		if (betweenLevels) {
			// Display the score for 3 seconds between levels
			if (deltaT.count() > 3) {
				if (level++ > 10) {
					break;
				}
				betweenLevels = false;
			} else  {
				continue;
			}
		}
		if (deltaT.count() >= .5) {
			generateNewBlock(world, level, display.get());
			start = chrono::system_clock::now();
		}
		if (redraw && al_is_event_queue_empty(eventQueue.get())) {
			processInput(world, 0, keys);
			redraw = false;
			al_clear_to_color(al_map_rgb(255, 0, 0));
			move(world, 0);
			physics(world, 0);
			move(world, 1);
			physics(world, 1);
			draw(world);
			al_flip_display();
			if (checkLevelOver(world, 2)) {
				score += 3 * (HEIGHT / TILESIZE - 2);
				// Entity 0 is the player
				score -= world.collisionMeshes[0].mesh.size() - 3;
				resetWorld(world, display.get());
				al_flip_display();
				al_draw_textf(font.get(), al_map_rgb(255, 255, 255), WIDTH / 2, HEIGHT / 3, ALLEGRO_ALIGN_CENTRE, "%d", score);
				al_flip_display();
				betweenLevels = true;
			}
		}
	}
	return 0;
}