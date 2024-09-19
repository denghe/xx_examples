#pragma once

namespace Battle {

	inline void Explosion::Init(XY const& pos_, float radius_) {
		pos = pos_;
		//radius = radius_;	// unused
		scale = radius_ / cRadius * cScaleFix;
		radians = gLooper.rnd.Next<float>(xx::gNPI, xx::gPI);
		frame = gRes.explosion_13_[(int32_t)frameIndex];
		color = { 255,255,255,123 };
	}

	inline bool Explosion::Update() {
		frameIndex += cFrameInc;
		if (frameIndex >= (float)gRes._countof_explosion_13_) return true;
		frame = gRes.explosion_13_[(int32_t)frameIndex];
		return false;
	}

};
