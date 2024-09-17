#pragma once

namespace Battle {

	inline void Item_Potion1::Init(Monster* owner_) {
		if (owner_) {
			owner = owner_;
		}
		pos = {};		// fill by user
		radius = 16;
		radians = 0;
		//stats.Reserve.Emplace
		cType = ItemTypes::Potion1;
		cQualitity = ItemQualities::Normal;
		cEquipmentLocation = ItemEquipmentLocations::Bag;
		cQuantityLimit = 99;
		quantity = 1;	// fill by user
		cCastDelay = 0.2f;
		lastCastTime = 0;
		cPercentage = 0.3f;
	}

	inline void Item_Staff1::Init(Monster* owner_) {
		if (owner_) {
			owner = owner_;
		}
		pos = {};		// fill by user
		radius = 32;
		radians = 0;
		stats.Reserve(1);
		stats.Emplace(StatTypes::Mana, 123);
		cType = ItemTypes::Staff1;
		cQualitity = ItemQualities::Normal;
		cEquipmentLocation = ItemEquipmentLocations::Weapon;
		cQuantityLimit = 0;
		quantity = 0;
		cCastDelay = 1.f;
		lastCastTime = 0;
		cShootDistance = 20;
		cBulletRadius = 16;
		cBulletSpeed = 300;
		cBulletLifeSeconds = 5;
		cBulletDamage = 10;
	}

	inline void Item_Sword1::Init(Monster* owner_) {
		if (owner_) {
			owner = owner_;
		}
		pos = {};		// fill by user
		radius = 32;
		radians = 0;
		stats.Reserve(1);
		stats.Emplace(StatTypes::Health, 123);
		cType = ItemTypes::Sword1;
		cQualitity = ItemQualities::Normal;
		cEquipmentLocation = ItemEquipmentLocations::Weapon;
		cQuantityLimit = 0;
		quantity = 0;
		cCastDelay = 2.f;
		lastCastTime = 0;
		cShootDistance = 20;
		cBulletRadius = 32;
		cBulletDamage = 5;
	}

	inline bool Item_Potion1::Cast(Monster* target) {
		assert(owner);

		auto elapsedSeconds = gLooper.time - lastCastTime;
		if (elapsedSeconds < cCastDelay) return false;

		target = owner.pointer;
		if (target->health == target->maxHealth) return false;

		while (elapsedSeconds >= cCastDelay) {
			elapsedSeconds -= cCastDelay;
			target->health += int32_t(owner.pointer->maxHealth * cPercentage);
			if (target->health > target->maxHealth) {
				target->health = target->maxHealth;
				break;
			}
		}

		lastCastTime = gLooper.time - (elapsedSeconds - cCastDelay);

		return true;
	}

	inline bool Item_Staff1::Cast(Monster* target) {
		assert(owner);
		
		auto elapsedSeconds = gLooper.time - lastCastTime;
		if (elapsedSeconds < cCastDelay) return false;

		XY d, shootPos;

		if (target) {
			d = target->pos - owner.pointer->pos;
		} else {
			d = owner.pointer->movementDirection;
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
			shootPos = owner.pointer->pos + XY{ cos * distance, sin * distance };
			// scene->MakeFireball( owner, shootPos, radians, cBulletRadius, cBulletSpeed, cBulletLifeSeconds, cBulletDamage );
		}

		return false;
	}

	inline bool Item_Sword1::Cast(Monster* target) {
		assert(owner);

		auto elapsedSeconds = gLooper.time - lastCastTime;
		if (elapsedSeconds < cCastDelay) return false;
		lastCastTime = gLooper.time;

		XY d, shootPos;

		if (target) {
			d = target->pos - owner.pointer->pos;
		} else {
			d = owner.pointer->movementDirection;
		}

#if 0
		auto radians = std::atan2(d);
		auto sin = std::sin(radians);
		auto cos = std::cos(radians);
		shootPos = owner.pointer->pos + XY{ cos * cShootDistance, sin * cShootDistance };
#else
		if (auto mag = std::sqrt(d.x * d.x + d.y * d.y); mag > 0) {
			shootPos = owner.pointer->pos + d / mag * cShootDistance;
		} else {
			shootPos = owner.pointer->pos + XY{ cShootDistance, 0 };
		}
#endif

		// todo:
		// scene->MakeBlade(( owner, shootPos, radians, cBulletRadius, cBulletDamage );

		return false;
	}

};
