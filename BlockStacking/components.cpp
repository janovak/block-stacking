#include "components.h"

Skin::Skin() {
	img.reset();
}

Skin& Skin::operator=(const Skin& s) {
	img.reset(s.img.get());
	return *this;
}

Skin::Skin(const Skin& s) {
	img.reset(s.img.get());
}
