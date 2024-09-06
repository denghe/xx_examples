#pragma once
#include "pch.h"
#include "battle.h"

namespace Battle {

	XX_INLINE void Monster::Case_Action_Move(Action_Move& o, int32_t frameNumber, int32_t index) {
		pos += movementDirection * o.speed;
	}

	XX_INLINE void Monster::Case_Action_Stun(Action_Stun& o, int32_t frameNumber, int32_t index) {
		if (o.timeoutFrameNumber < frameNumber) {
			actions[index] = actions[--actionsLen];
			ActionClearFlag(ActionTypes::Stun);
		}
	}

	XX_INLINE void Monster::Case_Action_AI_SearchTarget(Action_Move& b, int32_t frameNumber, int32_t index) {
		// todo
	}

	XX_INLINE void Monster::Case_Action_AI_MoveToTarget(Action_Stun& b, int32_t frameNumber, int32_t index) {
		// todo
	}

	XX_INLINE void Monster::Case_Action_AI_HitTarget(Action_Stun& b, int32_t frameNumber, int32_t index) {
		// todo
	}

	// ...
};

