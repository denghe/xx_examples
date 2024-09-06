#pragma once
#include "pch.h"
#include "battle.h"

namespace Battle {

	XX_INLINE void Monster::ActionCall_Move(Action_Move& o, int32_t frameNumber, int32_t index) {
		pos += movementDirection * o.speed;
	}

	XX_INLINE void Monster::ActionCall_Stun(Action_Stun& o, int32_t frameNumber, int32_t index) {
		if (o.timeoutFrameNumber < frameNumber) {
			actions[index] = actions[--actionsLen];
			ActionClearFlag(ActionTypes::Stun);
		}
	}

	/*********************************************************************************************/

	bool Monster::ActionAdd_Move(float speed) {
		assert(actionsLen < _countof(actions));
		if (ActionExists(ActionTypes::Move) || ActionExists(ActionTypes::Stun)) return false;
		ActionSetFlag(ActionTypes::Move);
		auto& o = (Action_Move&)actions[actionsLen++];
		o.type = ActionTypes::Move;
		o.speed = speed;
		return true;
	}

	bool Monster::ActionAdd_Stun(int32_t numFrames) {
		assert(actionsLen < _countof(actions));
		if (ActionExists(ActionTypes::Stun)) return false;
		ActionRemove(ActionTypes::Move);
		ActionSetFlag(ActionTypes::Stun);
		auto& o = (Action_Stun&)actions[actionsLen++];
		o.type = ActionTypes::Stun;
		o.timeoutFrameNumber = scene->frameNumber + numFrames;
		return true;
	}

};
