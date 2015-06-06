#include <vector>

#include "components.h"

unsigned int newEntityIndex(World&);
void destroyEntity(World&, unsigned int);
unsigned int createBlock(World&, int, int, float, float, float, Point, Point, ALLEGRO_BITMAP*);
void move(World&);
void draw(const World&);
unsigned int createPlayer(World&, int, int, float, float, float, std::vector<AABB>&, std::vector<Skin>&);
//void addBlockToPlayer(World&, unsigned int, unsigned int);
void generateNewBlock(World&, unsigned int,	ALLEGRO_DISPLAY*, const int, const int);
