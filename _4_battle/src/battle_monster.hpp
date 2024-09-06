#pragma once
#include "pch.h"
#include "battle.h"

namespace Battle {

	inline void Monster::Init(Scene* scene_) {
		scene = scene_;
		auto radians = scene->rnd.Next<float>(-gPI, gPI);
		pos = gLooper.mapSize_2;
		movementDirection.x = std::cos(radians);
		movementDirection.y = std::sin(radians);
		auto n = ++scene->autoId;
		id = n;
		TryAddBaseActions();
	}

	/*********************************************************************************************/

	XX_INLINE void Monster::TryAddBaseActions() {
		ActionAdd_Move(2);
	}

	/*********************************************************************************************/

#define CONCAT_NAME( a, b ) a##b
#define CASE_ACTION_CALL(NAME) case ActionTypes::NAME: CONCAT_NAME(ActionCall_, NAME)((CONCAT_NAME(Action_, NAME)&)b, frameNumber, i); break;

	inline int32_t Monster::Update() {
		auto frameNumber = scene->frameNumber;
		auto posBak = pos;

		// execute all actions
		for (int32_t i = actionsLen - 1; i >= 0; --i) {
			auto& b = actions[i];
			switch (b.type) {
				CASE_ACTION_CALL(Move);
				CASE_ACTION_CALL(Stun);
				// ... more case
			}
		}

		// moved out of the screen? suicide
		if (pos.x < scene->screenAreaMin.x || pos.x > scene->screenAreaMax.x ||
			pos.y < scene->screenAreaMin.y || pos.y > scene->screenAreaMax.y) {
			return -1;
		}

		// try restore something
		TryAddBaseActions();

		// update space grid?
		if (posBak == pos) return 0;
		else return 1;
	}

};
