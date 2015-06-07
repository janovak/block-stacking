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
#include <vector>

#include <allegro5\allegro.h>

#include "components.h"
#include "entities.h"
#include "systems.h"

using namespace std;

const int FPS = 60;
const int TILESIZE = 32;
const int WIDTH = 512;
const int HEIGHT = 512;

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
	vector<bool> keys(256, false);
	bool redraw = true;
	World world;

	validate(al_init(), "Failed to initialize Allegro");
	display = unique_ptr<ALLEGRO_DISPLAY, decltype(displayDeleter)>(al_create_display(WIDTH, HEIGHT), displayDeleter);
	validate(display.get(), "Failed to create display");
	eventQueue = unique_ptr<ALLEGRO_EVENT_QUEUE, decltype(eventQueueDeleter)>(al_create_event_queue(), eventQueueDeleter);
	validate(display.get(), "Failed to create event queue");
	timer = unique_ptr<ALLEGRO_TIMER, decltype(timerDeleter)>(al_create_timer(1 / (float)FPS), timerDeleter);
	validate(timer.get(), "Failed to create timer");

	al_register_event_source(eventQueue.get(), al_get_display_event_source(display.get()));
	al_register_event_source(eventQueue.get(), al_get_timer_event_source(timer.get()));

	al_start_timer(timer.get());
	auto start = chrono::system_clock::now();
	while (true) {
		ALLEGRO_EVENT ev;
		al_wait_for_event(eventQueue.get(), &ev);
		if (ev.type == ALLEGRO_EVENT_TIMER) {
			redraw = true;
		} else if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {
			keys[ev.keyboard.keycode] = true;
		} else if (ev.type == ALLEGRO_EVENT_KEY_UP) {
			keys[ev.keyboard.keycode] = false;
		} else if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
			return 0;
		}

		auto deltaT = chrono::duration_cast<chrono::seconds>(chrono::system_clock::now() - start);
		if (deltaT.count() >= 2) {
			generateNewBlock(world, 1, display.get(), WIDTH, TILESIZE);
			start = chrono::system_clock::now();
		}
		if (redraw && al_is_event_queue_empty(eventQueue.get())) {
			redraw = false;
			al_clear_to_color(al_map_rgb(255, 0, 0));
			move(world, 0);
			physics(world, 0);
			move(world, 1);
			physics(world, 1);
			draw(world);
			al_flip_display();
		}
	}
	return 0;
}