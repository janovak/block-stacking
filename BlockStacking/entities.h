/* Struct of arrays of componenets to represent all entities */

#pragma once

#include <bitset>
#include <vector>

#include "components.h"

extern const unsigned int MAX_ENTITY_COUNT;

struct World {
	std::vector<std::bitset<TOTAL_COMPONENTS>> masks;
	std::vector<EntityType> types;
	std::vector<Point> points;
	std::vector<UnitVector> unitVectors;
	std::vector<Speed> speeds;
	std::vector<AABB> aabbs;
	std::vector<Skin> skins;
	std::vector<CollisionMesh> collisionMeshes;
	std::vector<SkinList> skinLists;
	World();
};
