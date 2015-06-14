#include <iostream>
#include <vector>

#include "systems.h"

const unsigned int MAX_ENTITY_COUNT = 64;

using namespace std;

unsigned int newEntityIndex(World& world) {
	for (int i = 0; i < MAX_ENTITY_COUNT; ++i) {
		if (world.masks[i].none()) {
			return i;
		}
	}
	return MAX_ENTITY_COUNT;
}

void destroyEntity(World& world, unsigned int entity) {
	world.masks[entity].reset();
}

unsigned int createBlock(World& world, int x, int y, float dx, float dy, float speed, Point min, Point max, ALLEGRO_BITMAP* img) {
	unsigned int entity = newEntityIndex(world);
	world.masks[entity].set(COMPONENT_TYPE);
	world.masks[entity].set(COMPONENT_POINT);
	world.masks[entity].set(COMPONENT_UNITVECTOR);
	world.masks[entity].set(COMPONENT_SPEED);
	world.masks[entity].set(COMPONENT_COLLISIONMESH);
	world.masks[entity].set(COMPONENT_SKIN);
	world.types[entity] = TYPE_BLOCK;
	world.points[entity] = Point(x, y);
	world.unitVectors[entity] = UnitVector(dx, dy);
	world.speeds[entity] = Speed(speed);
	world.collisionMeshes[entity].mesh = vector < AABB > { AABB(min, max) };
	world.skins[entity] = Skin(std::shared_ptr<ALLEGRO_BITMAP>(img, [](ALLEGRO_BITMAP* b) { al_destroy_bitmap(b); }));
	return entity;
}

unsigned int createPlayer(World& world, int x, int y, float dx, float dy, float speed, vector<AABB>& mesh, vector<Skin>& imgs) {
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

void move(World& world, unsigned int axis) {
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

void physics(World& world, unsigned int axis) {
	vector<vector<int>> contacts = collisions(world);
	for (auto contact : contacts) {
		unsigned int entity = contact.back();
		contact.pop_back();
		//cout << contact.size() << endl;
		for (auto c : contact) {
			if (world.types[entity] == TYPE_PLAYER && world.types[c] == TYPE_BLOCK) {
				if (axis == 0) {
					world.points[entity].x += world.unitVectors[entity].dx * world.speeds[entity].speed * -1;
				} else if (axis == 1) {
					addBlockToPlayer(world, c, entity);
				}
			}
		}
	}
}

vector<vector<int>> collisions(const World& world) {
	vector<vector<int>> ret;
	for (int i = 0; i < MAX_ENTITY_COUNT; ++i) {
		ret.push_back(contacts(world, i));
		ret.back().push_back(i);
	}
	return ret;
}

bool intersects(const World& world, unsigned int entity1, unsigned int entity2) {
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

vector<int> contacts(const World& world, unsigned int entity) {
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

void addBlockToPlayer(World& world, unsigned int blockEntity, unsigned int playerEntity) {
	world.collisionMeshes[playerEntity].mesh.push_back(world.aabbs[blockEntity]);
	world.skinLists[playerEntity].imgs.push_back(world.skins[blockEntity]);
	destroyEntity(world, blockEntity);
}

void generateNewBlock(World& world, unsigned int level, ALLEGRO_DISPLAY* display) {
	int x = rand() % (WIDTH / TILESIZE) * TILESIZE;
	int y = -1 * TILESIZE;
	Point min{ x, y };
	Point max{ x + (int)TILESIZE, y + (int)TILESIZE };
	ALLEGRO_BITMAP* color = al_create_bitmap(TILESIZE, TILESIZE);
	al_set_target_bitmap(color);
	al_clear_to_color(al_map_rgb(rand() % 256, rand() % 256, rand() % 256));
	al_set_target_bitmap(al_get_backbuffer(display));
	createBlock(world, x, y, 0, 1, level, min, max, color);
}

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
	shared_ptr<ALLEGRO_BITMAP> black(al_create_bitmap(groundWidth, TILESIZE) , [](ALLEGRO_BITMAP* b) { al_destroy_bitmap(b); });
	shared_ptr<ALLEGRO_BITMAP> white(al_create_bitmap(TILESIZE, TILESIZE) , [](ALLEGRO_BITMAP* b) { al_destroy_bitmap(b); });
	al_set_target_bitmap(black.get());
	al_clear_to_color(al_map_rgb(0, 0, 0));
	al_set_target_bitmap(white.get());
	al_clear_to_color(al_map_rgb(255, 255, 255));
	al_set_target_bitmap(al_get_backbuffer(display));
	createPlayer(world, x, y, 0, 0, 5,
		vector < AABB > { AABB(baseMin, baseMax), AABB(leftMarkerMin, leftMarkerMax), AABB(rightMarkerMin, rightMarkerMax) },
		vector < Skin > { Skin(black), Skin(white), Skin(white) });
}
