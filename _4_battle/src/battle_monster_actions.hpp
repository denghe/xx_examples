#pragma once
#include "pch.h"
#include "battle.h"

namespace Battle {

	XX_INLINE void Monster::Destroy() {
		scene->monsters.Remove(*this);
	}

	inline void Monster::TryAddBaseActions() {
		if (!ActionExists<Action_Stun>()
			&& !ActionExists<Action_SearchTarget>()
			&& !ActionExists<Action_MoveToTarget>()
			&& !ActionExists<Action_HitTarget>()
			) {
			Add_Action_SearchTarget(1300, 0.2);	// todo: get args from cfg?
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

};
