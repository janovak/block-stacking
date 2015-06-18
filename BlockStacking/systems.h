/* Systems to modify and interact with entities and their components */

#pragma once

#include <vector>

#include "components.h"
#include "entities.h"

extern const unsigned int TILESIZE;
extern const unsigned int WIDTH;
extern const unsigned int HEIGHT;

unsigned int newEntityIndex(World&);
void destroyEntity(World&, unsigned int);
unsigned int createBlock(World&, int, int, float, float, float, Point, Point, Skin&);
unsigned int createPlayer(World&, int, int, float, float, float, std::vector<AABB>&, std::vector<Skin>&);
void processInput(World&, unsigned int, const std::vector<bool>&);
void draw(const World&);
void move(World&, unsigned int);
void physics(World&, unsigned int);
std::vector<std::vector<int>> collisions(const World&);
bool intersects(const World&, unsigned int, unsigned int);
std::vector<int> contacts(const World&, unsigned int);
void addBlockToPlayer(World&, unsigned int, unsigned int);
void generateNewBlock(World&, unsigned int, ALLEGRO_DISPLAY*);
void generateBase(World&, ALLEGRO_DISPLAY*);
bool checkLevelOver(World&, unsigned int);
void resetWorld(World&, ALLEGRO_DISPLAY*);
