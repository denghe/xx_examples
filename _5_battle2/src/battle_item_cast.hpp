#pragma once

namespace Battle {

	inline bool Item_Potion1::Cast(Monster* target) {
		(void)target;
		assert(owner);
		auto caster = owner.pointer;

		auto elapsedSeconds = gLooper.time - lastCastTime;
		if (elapsedSeconds < cCastDelay) return false;
		if (elapsedSeconds > gLooper.frameDelay * 1.1f && elapsedSeconds > cCastDelay) {
			elapsedSeconds = cCastDelay;		// first call: limit
		}

		if (quantity < 1) return false;

		auto& si = caster->statInfo;
		auto& sim = caster->statInfoMax;
		if (si.health == sim.health) return false;

		while (elapsedSeconds >= cCastDelay && quantity > 0) {
			elapsedSeconds -= cCastDelay;
			--quantity;

			si.health += int32_t(sim.health * cPercentage);
			if (si.health > sim.health) {
				si.health = sim.health;
				break;
			}
		}

		lastCastTime = gLooper.time - (elapsedSeconds - cCastDelay);

		return true;
	}

	inline bool Item_Staff1::Cast(Monster* target) {
		assert(owner);
		auto caster = owner.pointer;

		auto elapsedSeconds = gLooper.time - lastCastTime;
		if (elapsedSeconds < cCastDelay) return false;
		if (elapsedSeconds > gLooper.frameDelay * 1.1f && elapsedSeconds > cCastDelay) {
			elapsedSeconds = cCastDelay;		// first call: limit
		}

		XY d, shootPos;

		if (target) {
			d = target->pos - caster->pos;
		} else {
			d = caster->movementDirection;
		}

		auto radians = std::atan2(d.y, d.x);
		auto sin = std::sin(radians);
		auto cos = std::cos(radians);
		auto count = int(elapsedSeconds / cCastDelay);
		elapsedSeconds -= count * cCastDelay;
		lastCastTime = gLooper.time - elapsedSeconds;
		auto sdStep = cBulletSpeed / count;
		for (int i = 0; i < count; ++i) {
			auto distance = cShootDistance - sdStep * i;
			shootPos = caster->pos + XY{ cos * distance, sin * distance };
			// scene->MakeFireball( caster, shootPos, radians, cBulletRadius, cBulletSpeed, cBulletLifeSeconds, cBulletDamage );
		}

		return false;
	}

	inline bool Item_Sword1::Cast(Monster* target) {
		assert(owner);
		auto caster = owner.pointer;

		auto elapsedSeconds = gLooper.time - lastCastTime;
		if (elapsedSeconds < cCastDelay) return false;
		if (elapsedSeconds > gLooper.frameDelay * 1.1f && elapsedSeconds > cCastDelay) {
			elapsedSeconds = cCastDelay;		// first call: limit
		}

		XY d, shootPos;

		if (target) {
			d = target->pos - caster->pos;
		} else {
			d = caster->movementDirection;
		}

		auto radians = std::atan2(d.y, d.x);
		auto sin = std::sin(radians);
		auto cos = std::cos(radians);
		auto count = int(elapsedSeconds / cCastDelay);
		elapsedSeconds -= count * cCastDelay;
		lastCastTime = gLooper.time - elapsedSeconds;
		shootPos = caster->pos + XY{ cos * cShootDistance, sin * cShootDistance };
		for (int i = 0; i < count; ++i) {
			scene->MakeBlade(caster, shootPos, radians, cBulletRadius, cBulletDamage);
		}

		return false;
	}

};
