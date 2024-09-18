#pragma once

namespace Battle {

	inline void BladeLight::Init(XY const& pos_, float radians_, float scale_) {
		pos = pos_;
		radians = radians_;
		radius = cRadius * scale_;
		alpha = 1;
		frame = gRes.blade_light;
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
		color.a = int8_t(255 * alpha);
		return lineNumber < 0;
	}

};
