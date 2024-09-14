#pragma once

namespace Battle {

	inline void Item::Init(xx::SpaceWeak<Monster> const& owner_, XY const& pos_, float radians_) {
		owner = owner_;
		pos = pos_;
		radians = radians_;
	}

	XX_INLINE int32_t Item::UpdateCore() {
		switch (lineNumber) {
		case 0:
			return 1;
		case 1:
			return 1;
		default:
			return -1;
		}
	}

	inline bool Item::Update() {
		lineNumber = UpdateCore();
		return lineNumber < 0;
	}

};
