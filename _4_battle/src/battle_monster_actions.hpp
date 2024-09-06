#pragma once
#include "pch.h"
#include "battle.h"

namespace Battle {

	inline void Monster::Stun(float durationSeconds) {
		if (!ActionExists<Action_Stun>()) {
			ActionTryRemoves<
				Action_Move,
				Action_AI_SearchTarget,
				Action_AI_MoveToTarget,
				Action_AI_HitTarget
				// ...
			>();
			Add_Action_Stun(durationSeconds);
		}
	}

};

