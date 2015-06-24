#include <iostream>
#include <vector>

#include "systems.h"

const unsigned int MAX_ENTITY_COUNT = 32;

using namespace std;

/* Find the first available ID for a new entity */
unsigned int newEntityIndex(const World& world) {
	for (int i = 0; i < MAX_ENTITY_COUNT; ++i) {
		if (world.masks[i].none()) {
			return i;
		}
	}
	return MAX_ENTITY_COUNT;
}

/* Free the specified entity's ID */
void destroyEntity(World& world, const unsigned int& entity) {
	world.masks[entity].reset();
	world.aabbs[entity] = AABB();
	world.points[entity] = Point();
	world.skins[entity] = Skin();
	world.speeds[entity] = 0.0f;
	world.unitVectors[entity] = UnitVector(0, 0);
	world.collisionMeshes[entity] = CollisionMesh{ vector<AABB>() };
	world.skinLists[entity] = SkinList{ vector<Skin>() };
}

/* Create a block entity */
unsigned int createBlock(World& world, const int& x, const int& y, const float& dx, const float& dy, const float& speed, const Point& min, const Point& max, const Skin& img) {
	unsigned int entity = newEntityIndex(world);
	world.masks[entity].set(COMPONENT_TYPE);
	world.masks[entity].set(COMPONENT_POINT);
	world.masks[entity].set(COMPONENT_UNITVECTOR);
	world.masks[entity].set(COMPONENT_SPEED);
	world.masks[entity].set(COMPONENT_COLLISIONMESH);
	world.masks[entity].set(COMPONENT_SKINLIST);
	world.types[entity] = TYPE_BLOCK;
	world.points[entity] = Point(x, y);
	world.unitVectors[entity] = UnitVector(dx, dy);
	world.speeds[entity] = Speed(speed);
	world.collisionMeshes[entity].mesh = vector < AABB > { AABB(Point(0, 0), Point(TILESIZE, TILESIZE)) };
	world.skinLists[entity].imgs = vector < Skin > { Skin(img) };
	return entity;
}

/* Create a player entity */
unsigned int createPlayer(World& world, const int& x, const int& y, const float& dx, const float& dy, const float& speed, const vector<AABB>& mesh, const vector<Skin>& imgs) {
	unsigned int entity = newEntityIndex(world);
	world.masks[entity].set(COMPONENT_TYPE);
	world.masks[entity].set(COMPONENT_POINT);
	world.masks[entity].set(COMPONENT_UNITVECTOR);
	world.masks[entity].set(COMPONENT_SPEED);
	world.masks[entity].set(COMPONENT_COLLISIONMESH);
	world.masks[entity].set(COMPONENT_SKINLIST);
	world.types[entity] = TYPE_PLAYER;
	world.points[entity] = Point(x, y);
	world.unitVectors[entity] = UnitVector(dx, dy);
	world.speeds[entity] = Speed(speed);
	world.collisionMeshes[entity].mesh = mesh;
	for (auto skin : imgs) {
		world.skinLists[entity].imgs.push_back(move(skin));
	}
	return entity;
}

/* Move all entities that can move */
void move(World& world, const unsigned int& axis) {
	for (int i = 0; i < MAX_ENTITY_COUNT; ++i) {
		if (world.masks[i].test(COMPONENT_POINT) && world.masks[i].test(COMPONENT_UNITVECTOR) && world.masks[i].test(COMPONENT_SPEED)) {
			if (axis == 0) {
				world.points[i].x += (world.unitVectors[i].dx * world.speeds[i].speed);
			} else if (axis == 1) {
				world.points[i].y += (world.unitVectors[i].dy * world.speeds[i].speed);
			}
		}
	}
}

/* Modify the selected entity's vector */
void processInput(World& world, const unsigned int& entity, const vector<bool>& keys) {
	float vector = 0.0f;
	if (keys[ALLEGRO_KEY_LEFT]) {
		vector--;
	}
	if (keys[ALLEGRO_KEY_RIGHT]) {
		vector++;
	}
	world.unitVectors[entity].dx = vector;
}

/* Draw all renderable entities to the screen */
void draw(const World& world) {
	for (int i = 0; i < MAX_ENTITY_COUNT; ++i) {
		if (world.masks[i].test(COMPONENT_POINT)) {
			if (world.masks[i].test(COMPONENT_SKIN)) {
				al_draw_bitmap(world.skins[i].img.get(), world.points[i].x, world.points[i].y, 0);
			} else if (world.masks[i].test(COMPONENT_SKINLIST)) {
				int x = world.points[i].x;
				int y = world.points[i].y;
				vector<Skin> imgs = world.skinLists[i].imgs;
				vector<AABB> meshes = world.collisionMeshes[i].mesh;
				for (size_t j = 0; j < imgs.size(); ++j) {
					al_draw_bitmap(imgs[j].img.get(), x + meshes.at(j).min.x, y + meshes.at(j).min.y, 0);
				}
			}
		}
	}
}

/* Handle collisions */
void physics(World& world, const unsigned int& axis) {
	vector<vector<int>> contacts = collisions(world);
	for (auto contact : contacts) {
		unsigned int entity = contact.back();
		contact.pop_back();
		for (auto c : contact) {
			if (world.types[entity] == TYPE_PLAYER && world.types[c] == TYPE_BLOCK) {
				if (axis == 0) {
					// Slowly retreat player away from intersecting block until
					// player is only touching the block. This doesn't need to
					// be a binary search because of how small the overlap will
					// be in general
					while (intersects(world, entity, c)) {
						world.points[entity].x += world.unitVectors[entity].dx * -1;
					}
				} else if (axis == 1) {
					Point block = world.points[c];
					Point player = world.points[entity];
					float difference = (block.y - player.y) % TILESIZE;
					// Move the block colliding with the player back so it is
					// only touching the player
					world.collisionMeshes[c].mesh.front().min.x = block.x - player.x;
					world.collisionMeshes[c].mesh.front().min.y = block.y - player.y - difference;
					world.collisionMeshes[c].mesh.front().max.x = world.collisionMeshes[c].mesh.front().min.x + TILESIZE;
					world.collisionMeshes[c].mesh.front().max.y = world.collisionMeshes[c].mesh.front().min.y + TILESIZE;
					addBlockToPlayer(world, c, entity);
				}
			}
		}
	}
}

/* Return all collisions between all entities */
vector<vector<int>> collisions(const World& world) {
	vector<vector<int>> ret;
	for (int i = 0; i < MAX_ENTITY_COUNT; ++i) {
		ret.push_back(contacts(world, i));
		// Push the entity being tested for collisions with other entities
		// onto the back of the vector
		ret.back().push_back(i);
	}
	return ret;
}

/* Return whether or not two entities are intersecting */
bool intersects(const World& world, const unsigned int& entity1, const unsigned int& entity2) {
	if (entity1 == entity2) { return true; }
	if (world.masks[entity1].test(COMPONENT_POINT) && world.masks[entity1].test(COMPONENT_COLLISIONMESH) &&
		world.masks[entity2].test(COMPONENT_POINT) && world.masks[entity2].test(COMPONENT_COLLISIONMESH)) {
		Point p1 = world.points[entity1];
		Point p2 = world.points[entity2];
		for (auto cm1 : world.collisionMeshes[entity1].mesh) {
			for (auto cm2 : world.collisionMeshes[entity2].mesh) {
				if (p1.x + cm1.min.x < p2.x + cm2.max.x &&
					p1.x + cm1.max.x > p2.x + cm2.min.x &&
					p1.y + cm1.min.y < p2.y + cm2.max.y &&
					p1.y + cm1.max.y > p2.y + cm2.min.y) {
					return true;
				}
			}
		}
	}
	return false;
}

/* Return all the entities an entity is colliding with */
vector<int> contacts(const World& world, const unsigned int& entity) {
	vector<int> ret;
	if (world.masks[entity].test(COMPONENT_POINT) && world.masks[entity].test(COMPONENT_COLLISIONMESH)) {
		for (int i = 0; i < MAX_ENTITY_COUNT; ++i) {
			if (entity == i) { continue; }
			if (world.masks[i].test(COMPONENT_POINT) && world.masks[i].test(COMPONENT_COLLISIONMESH)) {
				if (intersects(world, entity, i)) {
					ret.push_back(i);
				}
			}
		}
	}
	return ret;
}

/* Convert a block entity to be part of the player */
void addBlockToPlayer(World& world, const unsigned int& blockEntity, const unsigned int& playerEntity) {
	world.collisionMeshes[playerEntity].mesh.push_back(world.collisionMeshes[blockEntity].mesh.front());
	world.skinLists[playerEntity].imgs.push_back(world.skinLists[blockEntity].imgs.front());
	destroyEntity(world, blockEntity);
}

/* Generate a new block to fall from the sky */
void generateNewBlock(World& world, const unsigned int& level, ALLEGRO_DISPLAY* display) {
	int x = rand() % (WIDTH / TILESIZE) * TILESIZE;
	int y = -1 * TILESIZE;
	Point min{ x, y };
	Point max{ x + (int)TILESIZE, y + (int)TILESIZE };
	shared_ptr<ALLEGRO_BITMAP> color(al_create_bitmap(TILESIZE, TILESIZE), [](ALLEGRO_BITMAP* b) { al_destroy_bitmap(b); });
	al_set_target_bitmap(color.get());
	al_clear_to_color(al_map_rgb(rand() % 256, rand() % 256, rand() % 256));
	al_set_target_bitmap(al_get_backbuffer(display));
	createBlock(world, x, y, 0, 1, level, min, max, Skin(color));
}

/* Initalize the ground that the player controls */
void generateBase(World& world, ALLEGRO_DISPLAY* display) {
	int x = TILESIZE - WIDTH;
	int y = HEIGHT - TILESIZE;
	int groundWidth = 3 * WIDTH - 2 * TILESIZE;
	Point baseMin{ 0, 0 };
	Point baseMax{ groundWidth, (int)TILESIZE };
	Point leftMarkerMin{ (int)(WIDTH - TILESIZE), 0 };
	Point leftMarkerMax{ leftMarkerMin.x + (int)TILESIZE, leftMarkerMin.y + (int)TILESIZE };
	Point rightMarkerMin{ groundWidth - (int)WIDTH, 0 };
	Point rightMarkerMax{ rightMarkerMin.x + (int)TILESIZE, rightMarkerMin.y + (int)TILESIZE };
	shared_ptr<ALLEGRO_BITMAP> black(al_create_bitmap(groundWidth, TILESIZE), [](ALLEGRO_BITMAP* b) { al_destroy_bitmap(b); });
	shared_ptr<ALLEGRO_BITMAP> white(al_create_bitmap(TILESIZE, TILESIZE), [](ALLEGRO_BITMAP* b) { al_destroy_bitmap(b); });
	al_set_target_bitmap(black.get());
	al_clear_to_color(al_map_rgb(0, 0, 0));
	al_set_target_bitmap(white.get());
	al_clear_to_color(al_map_rgb(255, 255, 255));
	al_set_target_bitmap(al_get_backbuffer(display));
	createPlayer(world, x, y, 0, 0, 5,
		vector < AABB > { AABB(baseMin, baseMax), AABB(leftMarkerMin, leftMarkerMax), AABB(rightMarkerMin, rightMarkerMax) },
		vector < Skin > { Skin(black), Skin(white), Skin(white) });
	// Sentinel blocks
	createBlock(world, x - WIDTH, y, 0, 0, 0, Point(0, 0), Point(TILESIZE, TILESIZE), Skin(white));
	createBlock(world, groundWidth, y, 0, 0, 0, Point(0, 0), Point(TILESIZE, TILESIZE), Skin(white));
}

/* Check if we have reached the end of the level */
bool checkLevelOver(const World& world, const unsigned int& tilesFromTop) {
	if (world.points[0].y + world.collisionMeshes[0].mesh.back().min.y <= (int)(tilesFromTop * TILESIZE)) {
		return true;
	}
	return false;
}

/* Reset the world */
void resetWorld(World& world, ALLEGRO_DISPLAY* display) {
	for (int i = 0; i < MAX_ENTITY_COUNT; ++i) {
		destroyEntity(world, i);
	}
	generateBase(world, display);
}