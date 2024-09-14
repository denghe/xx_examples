#pragma once

namespace Battle {

	inline void BladeLight::Init(XY const& pos_, float radians_, float scale_) {
		pos = pos_;
		radians = radians_;
		scale = scale_;
		alpha = 1;
	}

	XX_INLINE int32_t BladeLight::UpdateCore() {
		switch (lineNumber) {
		case 0:
			// alpha = 1;
			return 1;
		case 1:
			alpha -= cFadeOutStep;
			if (alpha < 0) return -1;
			return 1;
		default:
			return -1;
		}
	}

	inline bool BladeLight::Update() {
		lineNumber = UpdateCore();
		return lineNumber < 0;
	}

};
