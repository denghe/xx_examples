#pragma once

namespace Battle {

	XX_INLINE void Monster::Destroy() {
		gScene->monsters.Remove(*this);	// unsafe
	}

	XX_INLINE void Monster::Kill(Monster* killer) {
		if (killer) {
			// todo: +EXP?
		}
		if (skills.len) {
			// drop item on the ground		// todo: crash. bug fix
			assert(skills.len == 1);
			auto s = std::move(skills[0]);
			skills.RemoveAt(0);
			gScene->items.Add(s);
			gScene->itemsSG.Add(s);
			s->owner.Reset();
			s->pos = pos;
		}
		gScene->explosions.Emplace().Init(pos, radius);
		Destroy();	// unsafe
	}

	XX_INLINE bool Monster::Hurt(Monster* killer, int32_t damage_) {
		// todo: calculate damage
		statInfo.health -= damage_;

		gScene->effectTextManager.Add(pos, { 0, -1 }, { 255,222,131,127 }, damage_);
		if (statInfo.health <= 0) {
			Kill(killer);	// unsafe
			return true;
		} else {
			Add_Action_SetColor({ 255,88,88,255 }, 0.1);
			return false;
		}
	}

	inline void Monster::TryRestoreBornAbility() {
		if (!ActionExists<Action_Stun>()
			&& !ActionExists<Action_Search>()
			&& !ActionExists<Action_MoveToItem>()
			&& !ActionExists<Action_MoveToTarget>()
			&& !ActionExists<Action_HitTarget>()
			) {
			Add_Action_SearchTarget(gLooper.mapSize.x);
		}

		if (!ActionExists<Action_SetColor>()) {
			color = cColor;
		}
	}

	inline void Monster::Stun(float durationSeconds) {
		if (!ActionExists<Action_Stun>()) {
			ActionTryRemoves<
				Action_Search,
				Action_MoveToItem,
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
		gScene->bladeLights.Emplace().Init(shootPos, radians, radius);
		gScene->monsters.Foreach9All<true>(shootPos.x, shootPos.y, [&](Monster& m)->void {
			auto d = m.pos - shootPos;
			auto r = m.radius + BladeLight::cRadius;
			auto mag2 = d.x * d.x + d.y * d.y;
			if (mag2 <= r * r) {	// cross
				m.Hurt(this, damage);
			}
		}, this);
	}

	inline void Monster::MakeFireball(XY const& shootPos, float radians, float radius, float speed, float lifeSeconds, int32_t damage) {
		gScene->projectiles.Emplace().Emplace<Projectile_Fireball>()->Init(this, shootPos, radians, radius, speed, lifeSeconds, damage);
	}
};
