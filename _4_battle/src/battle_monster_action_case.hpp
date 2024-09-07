#pragma once
#include "pch.h"
#include "battle.h"

namespace Battle {

	XX_INLINE void Monster::Case_(Action_Move& o) {
		// move
		pos += movementDirection * o.movementSpeed;
	}

	XX_INLINE void Monster::Case_(Action_Stun& o) {
		// timeout: suicide
		if (scene->frameNumber > o.timeoutFrameNumber) {
			ActionRemove(o);
		}
	}

	XX_INLINE void Monster::Case_(Action_SearchTarget& o) {
		// delay
		if (scene->frameNumber < o.timeoutFrameNumber) return;
		// search
		if (auto m = scene->monsters.FindNearestByRange<true>(
			scene->srdd, pos.x, pos.y, o.searchRange, this)) {
			target = m;
		}
		// suicide
		ActionRemove(o);
		// next step
		Add_Action_MoveToTarget(20, 10, 5);	// todo: get args from cfg?
	}

	XX_INLINE void Monster::Case_(Action_MoveToTarget& o) {
		// lost target? timeout?
		if (!target.Exists() || scene->frameNumber > o.timeoutFrameNumber) {
			// suicide
			ActionRemove(o);
			// next step
			Add_Action_SearchTarget(1300, 0.2);	// todo: get args from cfg?
			return;
		}
		auto& m = target();
		// compare distance
		auto d = m.pos - pos;
		auto r = m.radius + radius + o.distanceLimit;
		auto mag2 = d.x * d.x + d.y * d.y;
		// reached
		if (mag2 <= r * r) {
			auto bak = o.distanceLimit;
			// suicide
			ActionRemove(o);
			// next step
			Add_Action_HitTarget(bak);	// can't use o.distanceLimit because removed
		} else {
			auto mag = std::sqrt(mag2);
			if (mag > 0) {
				// move to
				pos += (d / mag) * o.movementSpeed;
			}
		}
	}

	XX_INLINE void Monster::Case_(Action_HitTarget& o) {
		// todo
	}

	// ...
};
