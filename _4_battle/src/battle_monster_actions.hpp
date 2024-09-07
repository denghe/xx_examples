#pragma once
#include "pch.h"
#include "battle.h"

namespace Battle {

	inline void Monster::TryAddBaseActions() {
#if 0
		if (!ActionExists<Action_Stun>()
			&& !ActionExists<Action_Move>()) {
			Add_Action_Move(2);
		}
#else
		if (!ActionExists<Action_Stun>()
			&& !ActionExists<Action_Move>()
			&& !ActionExists<Action_SearchTarget>()
			&& !ActionExists<Action_MoveToTarget>()
			&& !ActionExists<Action_HitTarget>()
			) {
			Add_Action_SearchTarget(1300, 0.2);	// todo: get args from cfg?
		}
#endif
	}

	inline void Monster::Stun(float durationSeconds) {
		if (!ActionExists<Action_Stun>()) {
			ActionTryRemoves<
				Action_Move,
				Action_SearchTarget,
				Action_MoveToTarget,
				Action_HitTarget
				// ...
			>();
			Add_Action_Stun(durationSeconds);
		}
	}

};

