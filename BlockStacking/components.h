#pragma once

#include <iostream>
#include <memory>
#include <vector>

#include <allegro5\allegro.h>

enum EntityType {
	TYPE_BLOCK,
	TYPE_PLAYER
};

enum Component {
	COMPONENT_TYPE,
	COMPONENT_POINT,
	COMPONENT_UNITVECTOR,
	COMPONENT_SPEED,
	COMPONENT_AABB,
	COMPONENT_SKIN,
	COMPONENT_COLLISIONMESH,
	COMPONENT_SKINLIST,
	TOTAL_COMPONENTS
};

struct Type {
	EntityType type;
};

struct Point {
	int x;
	int y;
	Point() {};
	Point(int, int);
};

struct UnitVector {
	float dx;
	float dy;
	UnitVector() {};
	UnitVector(float, float);
};

struct AABB {
	Point min;
	Point max;
	AABB() {};
	AABB(Point&, Point&);
};

struct Skin {
	std::shared_ptr<ALLEGRO_BITMAP> img;
	Skin();
	Skin(std::shared_ptr<ALLEGRO_BITMAP>&);
};

struct Speed {
	float speed;
	Speed() {};
	Speed(float);
};

struct CollisionMesh {
	std::vector<AABB> mesh;
};

struct SkinList {
	std::vector<Skin> imgs;
};
