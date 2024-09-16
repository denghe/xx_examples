#pragma once

namespace Battle {

	XX_INLINE void Monster::Add_Action_Stun(float durationSeconds) {
		// conflict checks
		assert(!ActionExists<Action_SearchTarget>());
		assert(!ActionExists<Action_MoveToTarget>());
		assert(!ActionExists<Action_HitTarget>());
		// ...

		// create & init
		auto& o = ActionAdd<Action_Stun>();
		o.timeout = gLooper.time + durationSeconds;
	}

	XX_INLINE void Monster::Add_Action_SearchTarget(float searchRange, float castDelaySeconds) {
		// conflict checks
		assert(!ActionExists<Action_SearchTarget>());
		assert(!ActionExists<Action_MoveToTarget>());
		assert(!ActionExists<Action_HitTarget>());

		// create & init
		auto& o = ActionAdd<Action_SearchTarget>();
		o.searchRange = searchRange;
		o.timeout = gLooper.time + castDelaySeconds;
	}

	XX_INLINE void Monster::Add_Action_MoveToTarget(float movementSpeed, float distanceLimit, float timeoutSeconds) {
		// conflict checks
		assert(!ActionExists<Action_SearchTarget>());
		assert(!ActionExists<Action_MoveToTarget>());
		assert(!ActionExists<Action_HitTarget>());

		// create & init
		auto& o = ActionAdd<Action_MoveToTarget>();
		o.movementSpeed = movementSpeed;
		o.distanceLimit = distanceLimit;
		o.timeout = gLooper.time + timeoutSeconds;
	}

	XX_INLINE void Monster::Add_Action_HitTarget(float distanceLimit, float castDelaySeconds) {
		// conflict checks
		assert(!ActionExists<Action_SearchTarget>());
		assert(!ActionExists<Action_MoveToTarget>());
		assert(!ActionExists<Action_HitTarget>());

		// create & init
		auto& o = ActionAdd<Action_HitTarget>();
		o.distanceLimit = distanceLimit;
		o.castDelaySeconds = castDelaySeconds;
		o.timeout = 0;
	}

	XX_INLINE void Monster::Add_Action_SetColor(xx::RGBA8 color, float durationSeconds) {
		// create or ref & init
		auto& o = ActionAddOrRef<Action_SetColor>();
		o.color = color;
		o.timeout = gLooper.time + durationSeconds;
	}

	// ...
};
