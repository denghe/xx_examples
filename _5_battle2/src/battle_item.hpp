#pragma once

namespace Battle {

	inline void Item_Potion1::Init(Monster* owner_) {
		if (owner_) {
			owner = owner_;
		}
		pos = {};		// fill by user
		radius = 16;
		radians = 0;
		statPack = {};
		cType = ItemTypes::Potion1;
		cQualitity = ItemQualities::Normal;
		cEquipmentLocation = ItemEquipmentLocations::Bag;
		cQuantityLimit = 99;
		quantity = 1;	// fill by user
		cChargeSpeed = 0.2f / gLooper.fps;
		chargePool = 0;
		cPercentage = 0.3f;
	}

	inline void Item_Staff1::Init(Monster* owner_) {
		if (owner_) {
			owner = owner_;
		}
		pos = {};		// fill by user
		radius = 32;
		radians = 0;
		statPack = StatPack::Make(1);
		statPack[0].type = StatTypes::Mana;
		statPack[0].value = 123;
		cType = ItemTypes::Staff1;
		cQualitity = ItemQualities::Normal;
		cEquipmentLocation = ItemEquipmentLocations::Weapon;
		cQuantityLimit = 0;
		quantity = 0;
		cChargeSpeed = 1.f / gLooper.fps;
		chargePool = 0;
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
		statPack = StatPack::Make(1);
		statPack[0].type = StatTypes::Health;
		statPack[0].value = 123;
		cType = ItemTypes::Sword1;
		cQualitity = ItemQualities::Normal;
		cEquipmentLocation = ItemEquipmentLocations::Weapon;
		cQuantityLimit = 0;
		quantity = 0;
		cChargeSpeed = 2.f / gLooper.fps;
		chargePool = 0;
		cShootDistance = 20;
		cBulletRadius = 32;
		cBulletDamage = 5;
	}

	inline bool Item_Potion1::Cast(Monster* target) {
		assert(owner);
		target = owner.pointer;
		if (target->health == target->maxHealth) return false;
		target->health += int32_t(owner.pointer->maxHealth * cPercentage);
		if (target->health > target->maxHealth) {
			target->health = target->maxHealth;
		}
		return true;
	}

	inline bool Item_Staff1::Cast(Monster* target) {
		// todo
		return false;
	}

	inline bool Item_Sword1::Cast(Monster* target) {
		// todo
		return false;
	}

};
