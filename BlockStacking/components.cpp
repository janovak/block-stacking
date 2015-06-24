#include <algorithm>
#include <memory>

#include "components.h"

UnitVector::UnitVector(const float& deltaX, const float& deltaY) {
	dx = deltaX;
	dy = deltaY;
}

Speed::Speed(const float& sp) {
	speed = sp;
}

AABB::AABB(const Point& p1, const Point& p2) {
	min = p1;
	max = p2;
}

Point::Point(const int& xCoor, const int& yCoor) {
	x = xCoor;
	y = yCoor;
}

Skin::Skin(const std::shared_ptr<ALLEGRO_BITMAP>& bitmap)
	 : img(bitmap) {
}
