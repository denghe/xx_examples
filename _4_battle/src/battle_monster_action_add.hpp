#pragma once
#include "pch.h"
#include "battle.h"

namespace Battle {

	XX_INLINE void Monster::Add_Action_Stun(float durationSeconds) {
		// conflict checks
		assert(!ActionExists<Action_Move>());
		assert(!ActionExists<Action_SearchTarget>());
		assert(!ActionExists<Action_MoveToTarget>());
		assert(!ActionExists<Action_HitTarget>());
		// ...

		// create & init
		auto& o = ActionAdd<Action_Stun>();
		o.timeoutFrameNumber = scene->frameNumber + int32_t(durationSeconds * gLooper.fps);
	}

	XX_INLINE void Monster::Add_Action_SearchTarget(float searchRange, float castDelaySeconds) {
		// conflict checks
		assert(!ActionExists<Action_Move>());
		assert(!ActionExists<Action_SearchTarget>());
		assert(!ActionExists<Action_MoveToTarget>());
		assert(!ActionExists<Action_HitTarget>());

		// create & init
		auto& o = ActionAdd<Action_SearchTarget>();
		o.searchRange = searchRange;
		o.timeoutFrameNumber = scene->frameNumber + int32_t(castDelaySeconds * gLooper.fps);
	}

	XX_INLINE void Monster::Add_Action_MoveToTarget(float movementSpeed, float distanceLimit, float timeoutSeconds) {
		// conflict checks
		assert(!ActionExists<Action_Move>());
		assert(!ActionExists<Action_SearchTarget>());
		assert(!ActionExists<Action_MoveToTarget>());
		assert(!ActionExists<Action_HitTarget>());

		// create & init
		auto& o = ActionAdd<Action_MoveToTarget>();
		o.movementSpeed = movementSpeed;
		o.distanceLimit = distanceLimit;
		o.timeoutFrameNumber = scene->frameNumber + int32_t(timeoutSeconds * gLooper.fps);
	}

	XX_INLINE void Monster::Add_Action_HitTarget(float distanceLimit) {
		// conflict checks
		assert(!ActionExists<Action_Move>());
		assert(!ActionExists<Action_SearchTarget>());
		assert(!ActionExists<Action_MoveToTarget>());
		assert(!ActionExists<Action_HitTarget>());

		// create & init
		auto& o = ActionAdd<Action_HitTarget>();
		o.distanceLimit = distanceLimit;
	}

	// ...
};
