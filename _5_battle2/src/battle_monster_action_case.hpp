#pragma once

namespace Battle {

	XX_INLINE void Monster::Case_(Action_Stun& o) {
		if (!ActionExists<Action_SetColor>()) {
			frameIndex = 0;
		}
		// timeout: suicide
		if (gLooper.time > o.timeout) {
			ActionRemove(o);
		}
	}

	XX_INLINE void Monster::Case_(Action_SearchTarget& o) {
		if (!ActionExists<Action_SetColor>()) {
			frameIndex = 1;
		}
		// delay
		if (gLooper.time < o.timeout) return;
		// search
		if (auto m = scene->monsters.FindNearestByRange<true>(
			scene->srdd, pos.x, pos.y, o.searchRange, this)) {
			target = m;
		}
		ActionRemove(o);	// suicide
		// next step
		Add_Action_MoveToTarget(2, 20, 10);		// todo: get args from cfg?
	}

	XX_INLINE void Monster::Case_(Action_MoveToTarget& o) {
		if (!ActionExists<Action_SetColor>()) {
			frameIndex = 2;
		}
		// lost target? timeout?
		if (!target.Exists() || gLooper.time > o.timeout) {
			// suicide
			ActionRemove(o);
			return;
		}
		// compare distance
		auto& t = target();	// ref
		auto d = t.pos - pos;
		auto r = t.radius + radius + o.distanceLimit;
		auto mag2 = d.x * d.x + d.y * d.y;
		// reached
		if (mag2 <= r * r) {
			ActionRemove(o);	// suicide
			// next step
			Add_Action_HitTarget(20, 0.5);		// todo: get args from cfg?
		} else {
			auto mag = std::sqrt(mag2);
			if (mag > 0) {
				// move to
				pos += (d / mag) * o.movementSpeed;
			}
		}
	}

	XX_INLINE void Monster::Case_(Action_HitTarget& o) {
		if (!ActionExists<Action_SetColor>()) {
			frameIndex = 2;
		}
		// lost target?
		if (!target.Exists()) {
			ActionRemove(o);	// suicide
			return;
		}
		
		// ensure distance
		auto& t = target();	// ref
		auto d = t.pos - pos;
		auto r = t.radius + radius + o.distanceLimit;
		auto mag2 = d.x * d.x + d.y * d.y;
		if (mag2 <= r * r) {	// reached
			// cast skills?
			for (auto& s : skills) {
				s->Cast(&t);		// todo: if not success( waiting cd ... ), suicide ?
			}
		} else {	// not reached: suicide this action, search again
			ActionRemove(o);
		}
	}

	XX_INLINE void Monster::Case_(Action_SetColor& o) {
		if (gLooper.time > o.timeout) {
			ActionRemove(o);	// suicide
			return;
		}
		color = o.color;
		frameIndex = 3;
	}
	// ...
};
