#pragma once

namespace Battle {

	inline void Explosion::Init(XY const& pos_, float scale_) {
		pos = pos_;
		scale = cScale * scale_;
		radians = gLooper.rnd.Next<float>(xx::gNPI, xx::gPI);
	}

	inline bool Explosion::Update() {
		frameIndex += cFrameInc;
		return frameIndex >= (float)ResFrames::frame_count_explosion_13_;
	}

};
