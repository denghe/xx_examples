#pragma once

namespace Battle {

	XX_INLINE bool Projectile::IsBlockIntersect() {
		auto& sg = gScene->blocks;
		xx::FromTo<xx::XY> aabb{ pos - radius, pos + radius };	// pos to aabb
		if (!sg.TryLimitAABB(aabb)) {
			return true;	// bug?
		}
		sg.ClearResults();
		sg.ForeachAABB(aabb);	// search
		for (auto b : sg.results) {
			if (b->IntersectCircle(pos, radius)) return true;
		}
		if (pos.IsOutOfEdge(gLooper.mapSize)) {
			return true;	// bug?
		}
		return false;
	}

	inline void Projectile_Fireball::Init(xx::SpaceWeak<Monster> const& owner_, XY const& pos_, float radians_
		, float radius_, float moveSpeed_, float lifeSeconds_, int32_t damage_) {
		owner = owner_;
		pos = pos_;
		radians = radians_;
		radius = radius_;
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

		// todo: hit check

		// handle frame animation
		frameIndex += gLooper.fps / 10 * gLooper.frameDelay;	// todo: get from config? or res?
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
