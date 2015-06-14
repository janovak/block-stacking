#include <algorithm>
#include <memory>

#include "components.h"

UnitVector::UnitVector(float deltaX, float deltaY) {
	dx = deltaX;
	dy = deltaY;
}

Speed::Speed(float sp) {
	speed = sp;
}

AABB::AABB(Point& p1, Point& p2) {
	min = p1;
	max = p2;
}

Point::Point(int xCoor, int yCoor) {
	x = xCoor;
	y = yCoor;
}

Skin::Skin() {
	img.reset();
}

Skin::Skin(std::shared_ptr<ALLEGRO_BITMAP>& bitmap)
	 : img(bitmap) {
}
