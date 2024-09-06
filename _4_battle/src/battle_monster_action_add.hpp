#pragma once
#include "pch.h"
#include "battle.h"

namespace Battle {

	XX_INLINE void Monster::Add_Action_Move(float speed) {
		// conflict checks
		assert(!ActionExists<Action_Stun>());

		// create & init
		auto& o = ActionAdd<Action_Move>();
		o.type = ActionTypes::Move;
		o.speed = speed;
	}

	XX_INLINE void Monster::Add_Action_Stun(float durationSeconds) {
		// conflict checks
		assert(!ActionExists<Action_Move>());
		assert(!ActionExists<Action_AI_SearchTarget>());
		assert(!ActionExists<Action_AI_MoveToTarget>());
		assert(!ActionExists<Action_AI_HitTarget>());
		// ...

		// create & init
		auto& o = ActionAdd<Action_Stun>();
		o.timeoutFrameNumber = scene->frameNumber + int32_t(durationSeconds * gLooper.fps);
	}

	XX_INLINE void Monster::Add_Action_AI_SearchTarget(float searchRange, float castDelaySeconds) {
		// conflict checks
		assert(!ActionExists<Action_Stun>());

		// create & init
		auto& o = ActionAdd<Action_AI_SearchTarget>();
		o.searchRange = searchRange;
		o.timeoutFrameNumber = scene->frameNumber + int32_t(castDelaySeconds * gLooper.fps);
	}

	XX_INLINE void Monster::Add_Action_AI_MoveToTarget(float movementSpeed, float distanceLimit, float timeoutSeconds) {
		// conflict checks
		assert(!ActionExists<Action_Stun>());

		// create & init
		auto& o = ActionAdd<Action_AI_MoveToTarget>();
		o.movementSpeed = movementSpeed;
		o.distanceLimit = distanceLimit;
		o.timeoutFrameNumber = scene->frameNumber + int32_t(timeoutSeconds * gLooper.fps);
	}

	XX_INLINE void Monster::Add_Action_AI_HitTarget(float distanceLimit) {
		// conflict checks
		assert(!ActionExists<Action_Stun>());

		// create & init
		auto& o = ActionAdd<Action_AI_HitTarget>();
		o.distanceLimit = distanceLimit;
	}

	// ...
};
