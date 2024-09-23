#pragma once

namespace Battle {

	XX_INLINE void Monster::Add_Action_Stun(float durationSeconds) {
		// conflict checks
		assert(!ActionExists<Action_Search>());
		assert(!ActionExists<Action_MoveToItem>());
		assert(!ActionExists<Action_MoveToTarget>());
		assert(!ActionExists<Action_HitTarget>());
		// ...

		// create & init
		auto& o = ActionAdd<Action_Stun>();
		o.timeout = gScene->time + durationSeconds;
	}

	XX_INLINE void Monster::Add_Action_SearchTarget(float searchRange) {
		// conflict checks
		assert(!ActionExists<Action_Search>());
		assert(!ActionExists<Action_MoveToItem>());
		assert(!ActionExists<Action_MoveToTarget>());
		assert(!ActionExists<Action_HitTarget>());

		// create & init
		auto& o = ActionAdd<Action_Search>();
		o.searchRange = searchRange;
		o.timeout = gScene->time + gScene->rnd.Next<float>(0.1, 0.5);
	}

	XX_INLINE void Monster::Add_Action_MoveToItem(float movementSpeed) {
		// conflict checks
		assert(!ActionExists<Action_Search>());
		assert(!ActionExists<Action_MoveToItem>());
		assert(!ActionExists<Action_MoveToTarget>());
		assert(!ActionExists<Action_HitTarget>());

		// create & init
		auto& o = ActionAdd<Action_MoveToItem>();
		o.movementSpeed = movementSpeed;
	}

	XX_INLINE void Monster::Add_Action_MoveToTarget(float movementSpeed, float timeoutSeconds) {
		// conflict checks
		assert(!ActionExists<Action_Search>());
		assert(!ActionExists<Action_MoveToItem>());
		assert(!ActionExists<Action_MoveToTarget>());
		assert(!ActionExists<Action_HitTarget>());

		// create & init
		auto& o = ActionAdd<Action_MoveToTarget>();
		o.movementSpeed = movementSpeed;
		o.timeout = gScene->time + timeoutSeconds;
	}

	XX_INLINE void Monster::Add_Action_HitTarget(float castDelaySeconds) {
		// conflict checks
		assert(!ActionExists<Action_Search>());
		assert(!ActionExists<Action_MoveToItem>());
		assert(!ActionExists<Action_MoveToTarget>());
		assert(!ActionExists<Action_HitTarget>());

		// create & init
		auto& o = ActionAdd<Action_HitTarget>();
		o.castDelaySeconds = castDelaySeconds;
		o.timeout = 0;
	}

	XX_INLINE void Monster::Add_Action_SetColor(xx::RGBA8 color, float durationSeconds) {
		// create or ref & init
		auto& o = ActionAddOrRef<Action_SetColor>();
		o.color = color;
		o.timeout = gScene->time + durationSeconds;
	}

	// ...
};
