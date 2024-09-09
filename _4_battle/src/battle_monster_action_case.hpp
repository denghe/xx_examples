#pragma once

namespace Battle {

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
		Add_Action_MoveToTarget(2, 20, 10);		// todo: get args from cfg?
	}

	XX_INLINE void Monster::Case_(Action_MoveToTarget& o) {
		// lost target? timeout?
		if (!target.Exists() || scene->frameNumber > o.timeoutFrameNumber) {
			// suicide
			ActionRemove(o);
			return;
		}
		// ref to target
		auto& m = target();
		// compare distance
		auto d = m.pos - pos;
		auto r = m.radius + radius + o.distanceLimit;
		auto mag2 = d.x * d.x + d.y * d.y;
		// reached
		if (mag2 <= r * r) {
			// suicide
			ActionRemove(o);
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
		// lost target?
		if (!target.Exists()) {
			// suicide
			ActionRemove(o);
			return;
		}
		// wait cast delay?
		if (scene->frameNumber < o.timeoutFrameNumber) return;
		// ref to target
		auto& m = target();
		// compare distance
		auto d = m.pos - pos;
		auto r = m.radius + radius + o.distanceLimit;
		auto mag2 = d.x * d.x + d.y * d.y;
		// reached
		if (mag2 <= r * r) {
			// calc blade light pos & radians & scale
			auto r = std::atan2(d.y, d.x);
			auto sin = std::sin(r);
			auto cos = std::cos(r);
			auto p = pos + XY{ cos, sin } *(radius + BladeLight::cRadius * 0.5f);
			scene->bladeLights.Emplace().Init(p, r, 1);
			// hit
			scene->monsters.Foreach9All<true>(p.x, p.y, [&](Monster& mm)->xx::ForeachResult {
				auto zd = mm.pos - p;
				auto zr = mm.radius + BladeLight::cRadius;
				auto zmag2 = zd.x * zd.x + zd.y * zd.y;
				// cross
				if (zmag2 <= zr * zr) {
					if (Hurt(mm)) {
						scene->explosions.Emplace().Init(mm.pos, mm.radius / mm.cRadius);
						return xx::ForeachResult::RemoveAndContinue;
					}
				}
				return xx::ForeachResult::Continue;
			}, this);
			// refresh cast delay
			o.timeoutFrameNumber = scene->frameNumber + int32_t(o.castDelaySeconds * gLooper.fps);
		} else {
			// suicide
			ActionRemove(o);
		}
	}

	// ...
};
