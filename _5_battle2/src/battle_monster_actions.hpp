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

	inline bool Monster::Hurt(Monster &tar) {
		// todo: calculate damage
		tar.statInfo.health -= 1;
		gScene->effectTextManager.Add(tar.pos, { 0, -1 }, { 255,222,131,127 }, gScene->rnd.Next<int32_t>(1, 1000));
		if (tar.statInfo.health <= 0) {
			gScene->explosions.Emplace().Init(tar.pos, radius / cRadius);
			return true;
		} else {
			Add_Action_SetColor({ 255,88,88,255 }, 0.1);
			return false;
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
};
