#pragma once

namespace Battle {

	XX_INLINE void Monster::Destroy() {
		gScene->monsters.Remove(*this);
	}

	inline void Monster::TryRestoreBornAbility() {
		if (!ActionExists<Action_Stun>()
			&& !ActionExists<Action_SearchTarget>()
			&& !ActionExists<Action_MoveToTarget>()
			&& !ActionExists<Action_HitTarget>()
			) {
			Add_Action_SearchTarget(2000, 0.2);	// todo: get args from cfg?
		}

		if (!ActionExists<Action_SetColor>()) {
			color = cColor;
		}
	}

	inline void Monster::Stun(float durationSeconds) {
		if (!ActionExists<Action_Stun>()) {
			ActionTryRemoves<
				Action_SearchTarget,
				Action_MoveToTarget,
				Action_HitTarget
				// ...
			>();
			Add_Action_Stun(durationSeconds);
		}
	}

	XX_INLINE bool Monster::BlocksLimit() {
		auto& sg = gScene->blocks;
		xx::FromTo<xx::XY> aabb{ pos - cRadius, pos + cRadius };	// pos to aabb
		if (!sg.TryLimitAABB(aabb)) {
			return true;	// bug?
		}
		sg.ClearResults();
		sg.ForeachAABB(aabb);	// search
		for (auto b : sg.results) {
			b->PushOut(*this);
		}
		if (pos.IsOutOfEdge(gLooper.mapSize)) {	
			return true;	// bug?
		}
		return false;
	}

	inline void Monster::MakeBladeLight(XY const& shootPos, float radians, float radius, int32_t damage) {
		gScene->bladeLights.Emplace().Init(shootPos, radians, radius / BladeLight::cRadius);
		gScene->monsters.Foreach9All<true>(shootPos.x, shootPos.y, [&](Monster& m)->xx::ForeachResult {
			auto d = m.pos - shootPos;
			auto r = m.radius + BladeLight::cRadius;
			auto mag2 = d.x * d.x + d.y * d.y;
			if (mag2 <= r * r) {	// cross
				
				// todo: calculate damage
				m.statInfo.health -= damage;

				gScene->effectTextManager.Add(m.pos, { 0, -1 }, { 255,222,131,127 }, damage);
				if (m.statInfo.health <= 0) {
					gScene->explosions.Emplace().Init(m.pos, radius / cRadius);
					return xx::ForeachResult::RemoveAndContinue;
				} else {
					Add_Action_SetColor({ 255,88,88,255 }, 0.1);
				}
			}
			return xx::ForeachResult::Continue;
		}, this);
	}

	inline void Monster::MakeFireball(XY const& shootPos, float radians, float radius, float speed, float lifeSeconds, int32_t damage) {
		gScene->projectiles.Emplace().Emplace<Projectile_Fireball>()->Init(this, shootPos, radians, radius, speed, lifeSeconds, damage);
	}
};
