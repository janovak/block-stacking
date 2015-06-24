/* Systems to modify and interact with entities and their components */

#pragma once

#include <vector>

#include "components.h"
#include "entities.h"

extern const unsigned int TILESIZE;
extern const unsigned int WIDTH;
extern const unsigned int HEIGHT;

unsigned int newEntityIndex(const World&);
void destroyEntity(World&, const unsigned int&);
unsigned int createBlock(World&, const int&, const int&, const float&, const float&, const float&, const Point&, const Point&, const Skin&);
unsigned int createPlayer(World&, const int&, const int&, const float&, const float&, const float&, const std::vector<AABB>&, const std::vector<Skin>&);
void processInput(World&, const unsigned int&, const std::vector<bool>&);
void draw(const World&);
void move(World&, const unsigned int&);
void physics(World&, const unsigned int&);
std::vector<std::vector<int>> collisions(const World&);
bool intersects(const World&, const unsigned int&, const unsigned int&);
std::vector<int> contacts(const World&, const unsigned int&);
void addBlockToPlayer(World&, const unsigned int&, const unsigned int&);
void generateNewBlock(World&, const unsigned int&, ALLEGRO_DISPLAY*);
void generateBase(World&, ALLEGRO_DISPLAY*);
bool checkLevelOver(const World&, const unsigned int&);
void resetWorld(World&, ALLEGRO_DISPLAY*);
