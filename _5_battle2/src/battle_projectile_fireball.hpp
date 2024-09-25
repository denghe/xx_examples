#pragma once

namespace Battle {

	inline void Projectile_Fireball::Init(xx::SpaceGridWeak<Monster> const& owner_, XY const& pos_, float radians_
		, float radius_, float moveSpeed_, float lifeSeconds_, int32_t damage_) {
		owner = owner_;
		pos = pos_;
		radians = radians_;
		radius = radius_;
		scale = radius / cRadius;
		moveSpeed = moveSpeed_;
		timeout = gScene->time + lifeSeconds_;
		damage = damage_;

		auto sin = std::sin(radians);
		auto cos = std::cos(radians);
		moveStep = XY{ cos, sin } * moveSpeed / gLooper.fps;
	}

	inline bool Projectile_Fireball::Update() {
		// timeout?
		if (timeout < gScene->time) return true;

		// hit check
		auto o = owner.GetPointer();
		if (auto m = gScene->monsters.FindFirstCrossBy9<true>(pos.x, pos.y, radius, o)) {
			m->Hurt(o, damage);
			return true;	// suicide
		}

		// handle frame animation
		frameIndex += 30 / gLooper.fps;	// todo: get from config? or res?
		if (frameIndex >= (float)ResTpFrames::_countof_eye_fire_) {
			frameIndex -= (float)ResTpFrames::_countof_eye_fire_;
		}
		frame = gRes.eye_fire_[(int32_t)frameIndex];	// for Draw()

		// move
		pos += moveStep;

		// block
		if (IsBlockIntersect()) return true;

		return false;
	}

};
