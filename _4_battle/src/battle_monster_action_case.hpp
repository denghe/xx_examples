#pragma once
#include "pch.h"
#include "battle.h"

namespace Battle {

	XX_INLINE void Monster::Case_(Action_Move& o, int32_t frameNumber, int32_t index) {
		pos += movementDirection * o.speed;
	}

	XX_INLINE void Monster::Case_(Action_Stun& o, int32_t frameNumber, int32_t index) {
		if (o.timeoutFrameNumber < frameNumber) {
			ActionRemove<Action_Stun>(index);	// careful: o will be free
		}
	}

	XX_INLINE void Monster::Case_(Action_SearchTarget& o, int32_t frameNumber, int32_t index) {
		if (frameNumber < o.timeoutFrameNumber) return;
		if (auto m = scene->monsters.FindNearestByRange(scene->srdd, pos.x, pos.y, o.searchRange)) {
			target = m;
		}
		ActionRemove<Action_SearchTarget>(index);
		Add_Action_MoveToTarget(20, 10, 5);
		// todo
	}

	XX_INLINE void Monster::Case_(Action_MoveToTarget& o, int32_t frameNumber, int32_t index) {
		// todo
	}

	XX_INLINE void Monster::Case_(Action_HitTarget& o, int32_t frameNumber, int32_t index) {
		// todo
	}

	// ...
};
