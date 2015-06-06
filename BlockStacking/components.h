#pragma once

#include <memory>
#include <vector>

#include <allegro5\allegro.h>

namespace std {
	template<>
	class default_delete < ALLEGRO_BITMAP > {
	public:
		void operator()(ALLEGRO_BITMAP* ptr) {
			al_destroy_bitmap(ptr);
		}
	};
}

enum Component{
	COMPONENT_POINT,
	COMPONENT_UNITVECTOR,
	COMPONENT_SPEED,
	COMPONENT_AABB,
	COMPONENT_SKIN,
	COMPONENT_COLLISIONMESH,
	COMPONENT_SKINLIST,
	TOTAL_COMPONENTS
};

struct Point {
	int x;
	int y;
};

struct UnitVector {
	float dx;
	float dy;
};

struct AABB {
	Point min;
	Point max;
};

struct Skin {
	std::unique_ptr<ALLEGRO_BITMAP> img;
	Skin();
	Skin& operator=(const Skin& s);
	Skin(const Skin& s);
};

struct Speed {
	float speed;
};

struct CollisionMesh {
	std::vector<AABB> mesh;
};

struct SkinList {
	std::vector<Skin> imgs;
};
