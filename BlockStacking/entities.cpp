#include "entities.h"

using namespace std;

World::World() : masks(MAX_ENTITY_COUNT), types(MAX_ENTITY_COUNT), points(MAX_ENTITY_COUNT),
	unitVectors(MAX_ENTITY_COUNT), speeds(MAX_ENTITY_COUNT), aabbs(MAX_ENTITY_COUNT),
	skins(MAX_ENTITY_COUNT), collisionMeshes(MAX_ENTITY_COUNT), skinLists(MAX_ENTITY_COUNT) {
}