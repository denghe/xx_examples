#pragma once

namespace Battle {

	inline void Projectile_Fireball::Init(xx::SpaceWeak<Monster> const& owner_, XY const& pos_, float radians_
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
		if (auto m = gScene->monsters.FindFirstCrossBy9(pos.x, pos.y, radius)) {

			// todo: calculate damage
			m->statInfo.health -= damage;

			gScene->effectTextManager.Add(m->pos, { 0, -1 }, { 255,222,131,127 }, gScene->rnd.Next<int32_t>(1, 1000));
			if (m->statInfo.health <= 0) {
				// todo: add exp to owner?
				gScene->explosions.Emplace().Init(m->pos, radius / cRadius);
				m->Destroy();
			} else {
				m->Add_Action_SetColor({ 255,88,88,255 }, 0.1);
			}

			return true;
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
