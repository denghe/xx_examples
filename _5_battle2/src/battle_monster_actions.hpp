#pragma once

namespace Battle {

	XX_INLINE void Monster::Destroy() {
		scene->monsters.Remove(*this);
	}

	inline void Monster::TryRestoreBornAbility() {
		if (!ActionExists<Action_Stun>()
			&& !ActionExists<Action_SearchTarget>()
			&& !ActionExists<Action_MoveToTarget>()
			&& !ActionExists<Action_HitTarget>()
			) {
			Add_Action_SearchTarget(1300, 0.2);	// todo: get args from cfg?
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

	inline bool Monster::Hurt(Monster &tar) {
		// todo
		tar.statInfo.health -= 1;
		// todo: calculate damage
		scene->effectTextManager.Add(tar.pos, { 0, -1 }, { 255,222,131,127 }, scene->rnd.Next<int32_t>(1, 1000));
		if (tar.statInfo.health <= 0) {
			scene->explosions.Emplace().Init(tar.pos, radius / cRadius);
			return true;
		} else {
			Add_Action_SetColor({ 255,88,88,255 }, 0.1);
			return false;
		}
	}

	XX_INLINE bool Monster::BlocksLimit() {
		// calc aabb
		auto& sg = scene->blocks;
		xx::FromTo<xx::XY> aabb{ pos - cRadius, pos + cRadius };
		if (!sg.TryFixAABB(aabb)) {
			return true;
		}
		sg.ForeachAABB(aabb);
		auto guard = xx::MakeSimpleScopeGuard([&] { sg.ClearResults(); });
		for (auto b : sg.results) {
			b->PushOut(*this);
		}
		if (pos.IsOutOfEdge(gLooper.mapSize)) {
			return true;
		}
		return false;
	}
};
