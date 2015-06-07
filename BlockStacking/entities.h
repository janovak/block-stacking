#pragma once

#include <bitset>

#include "components.h"

extern const int MAX_ENTITY_COUNT;

struct World {
	std::bitset<TOTAL_COMPONENTS> masks[MAX_ENTITY_COUNT];
	EntityType types[MAX_ENTITY_COUNT];
	Point points[MAX_ENTITY_COUNT];
	UnitVector unitVectors[MAX_ENTITY_COUNT];
	Speed speeds[MAX_ENTITY_COUNT];
	AABB aabbs[MAX_ENTITY_COUNT];
	Skin skins[MAX_ENTITY_COUNT];
	CollisionMesh collisionMeshes[MAX_ENTITY_COUNT];
	SkinList skinLists[MAX_ENTITY_COUNT];
};