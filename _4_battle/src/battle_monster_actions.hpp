#pragma once

namespace Battle {

	XX_INLINE void Monster::Destroy() {
		scene->monsters.Remove(*this);
	}

	inline void Monster::TryAddBaseActions() {
		if (!ActionExists<Action_Stun>()
			&& !ActionExists<Action_SearchTarget>()
			&& !ActionExists<Action_MoveToTarget>()
			&& !ActionExists<Action_HitTarget>()
			) {
			Add_Action_SearchTarget(1300, 0.2);	// todo: get args from cfg?
		}
	}

	inline void Monster::Stun(float durationSeconds) {
		if (!ActionExists<Action_Stun>()) {
			ActionTryRemoves<
				Action_SearchTarget,
				Action_MoveToTarget,
				Action_HitTarget
				// ...
			>();
			Add_Action_Stun(durationSeconds);
		}
	}

	inline bool Monster::Hurt(Monster &tar) {
		// todo
		tar.health -= 1;
		// todo: damage number effect
		// todo: add change color to red action?
		return tar.health <= 0;;
	}

};
