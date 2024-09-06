#pragma once
#include "pch.h"
#include "battle.h"

namespace Battle {

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

