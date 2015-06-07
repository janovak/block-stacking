#include <vector>

#include "components.h"
#include "entities.h"

unsigned int newEntityIndex(World&);
void destroyEntity(World&, unsigned int);
unsigned int createBlock(World&, int, int, float, float, float, Point, Point, ALLEGRO_BITMAP*);
unsigned int createPlayer(World&, int, int, float, float, float, std::vector<AABB>&, std::vector<Skin>&);
void draw(const World&);
void move(World&, unsigned int);
void physics(World&, unsigned int);
std::vector<std::vector<int>> collisions(World&);
bool intersects(const World&, unsigned int, unsigned int);
std::vector<int> contacts(const World&, unsigned int);
//void addBlockToPlayer(World&, unsigned int, unsigned int);
void generateNewBlock(World&, unsigned int,	ALLEGRO_DISPLAY*, const int, const int);
