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

	XX_INLINE void Monster::Case_(Action_SearchTarget& b, int32_t frameNumber, int32_t index) {
		// todo
	}

	XX_INLINE void Monster::Case_(Action_MoveToTarget& b, int32_t frameNumber, int32_t index) {
		// todo
	}

	XX_INLINE void Monster::Case_(Action_HitTarget& b, int32_t frameNumber, int32_t index) {
		// todo
	}

	// ...
};
