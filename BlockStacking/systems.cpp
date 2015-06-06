#include <vector>

#include "entities.h"
#include "systems.h"

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
	world.masks[entity].set(COMPONENT_POINT);
	world.masks[entity].set(COMPONENT_UNITVECTOR);
	world.masks[entity].set(COMPONENT_SPEED);
	world.masks[entity].set(COMPONENT_AABB);
	world.masks[entity].set(COMPONENT_SKIN);
	world.points[entity].x = x;
	world.points[entity].y = y;
	world.unitVectors[entity].dx = dx;
	world.unitVectors[entity].dy = dy;
	world.speeds[entity].speed = speed;
	world.aabbs[entity].min = min;
	world.aabbs[entity].max = max;
	world.skins[entity].img.reset(img);
	return entity;
}

void move(World& world) {
	for (int i = 0; i < MAX_ENTITY_COUNT; ++i) {
		if (world.masks[i].test(COMPONENT_POINT) && world.masks[i].test(COMPONENT_UNITVECTOR) && world.masks[i].test(COMPONENT_SPEED)) {
			world.points[i].x += (world.unitVectors[i].dx * world.speeds[i].speed);
			world.points[i].y += (world.unitVectors[i].dy * world.speeds[i].speed);
		}
	}
}

void draw(const World& world) {
	for (int i = 0; i < MAX_ENTITY_COUNT; ++i) {
		if (world.masks[i].test(COMPONENT_POINT)) {
			if (world.masks[i].test(COMPONENT_SKIN)) {
				al_draw_bitmap(world.skins[i].img.get(), world.points[i].x, world.points[i].y, 0);
			}else if (world.masks[i].test(COMPONENT_SKINLIST)) {
				int x = world.points[i].x;
				int y = world.points[i].y;
				vector<Skin> imgs = world.skinLists[i].imgs;
				vector<AABB> meshes = world.collisionMeshes[i].mesh;
				for (size_t j = 0; j < imgs.size(); ++j) {
					al_draw_bitmap(imgs[j].img.get(), x + meshes.at(j).min.x, meshes.at(j).min.y, 0);
				}
			}
		}
	}
}

unsigned int createPlayer(World& world, int x, int y, float dx, float dy, float speed, vector<AABB>& mesh, vector<Skin>& imgs) {
	unsigned int entity = newEntityIndex(world);
	world.masks[entity].set(COMPONENT_POINT);
	world.masks[entity].set(COMPONENT_UNITVECTOR);
	world.masks[entity].set(COMPONENT_SPEED);
	world.masks[entity].set(COMPONENT_COLLISIONMESH);
	world.masks[entity].set(COMPONENT_SKINLIST);
	world.points[entity].x = x;
	world.points[entity].y = y;
	world.unitVectors[entity].dx = dx;
	world.unitVectors[entity].dy = dy;
	world.speeds[entity].speed = speed;
	world.collisionMeshes[entity].mesh = mesh;
	world.skinLists[entity].imgs = imgs;
	return entity;
}

/*void addBlockToPlayer(World& world, unsigned int blockEntity, unsigned int playerEntity) {
	world.collisionMeshes[playerEntity].mesh.push_back(world.aabbs[blockEntity]);
	//world.skinLists[playerEntity].imgs.push_back(world.skins[blockEntity]);
	destroyEntity(world, blockEntity);
}*/

void generateNewBlock(World& world, unsigned int level,	ALLEGRO_DISPLAY* display, const int WIDTH, const int TILESIZE) {
	int x = rand() % (WIDTH / TILESIZE) * TILESIZE;
	int y = -1 * TILESIZE;
	Point min{ x, y };
	Point max{ x + TILESIZE, y + TILESIZE};
	ALLEGRO_BITMAP* color = al_create_bitmap(TILESIZE, TILESIZE);
	al_set_target_bitmap(color);
	al_clear_to_color(al_map_rgb(rand() % 256, rand() % 256, rand() % 256));
	al_set_target_bitmap(al_get_backbuffer(display));
	createBlock(world, x, y, 0, 1, level, min, max, color);
}

