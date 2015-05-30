#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include <allegro5\allegro.h>

using namespace std;

const int FPS = 60;
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
	while (true) {
		ALLEGRO_EVENT ev;
		al_wait_for_event(eventQueue.get(), &ev);

		if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {
			redraw = true;
		} else if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
			return 0;
		}

		if (redraw && al_is_event_queue_empty(eventQueue.get())) {
			redraw = false;
			al_clear_to_color(al_map_rgb(255, 0, 0));
			al_flip_display();
		}
	}
	
	return 0;
}