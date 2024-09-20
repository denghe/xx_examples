#pragma once

namespace Battle {

	// todo: get data from config file?

	inline void Item_Potion1::Init(Monster* owner_) {
		if (owner_) {
			owner = owner_;
		}
		//pos
		//frame
		radius = 16;
		radians = 0;
		//stats
		cType = ItemTypes::Potion1;
		cQualitity = ItemQualities::Normal;
		cEquipmentLocation = ItemEquipmentLocations::Bag;
		cQuantityLimit = 99;
		quantity = 1;
		cCastDelay = 10.f;
		lastCastTime = 0;
		cPercentage = 0.3f;
	}

	inline void Item_Staff1::Init(Monster* owner_) {
		if (owner_) {
			owner = owner_;
		}
		//pos
		frame = gRes.staff_5;
		radius = 32;
		radians = 0;
		stats.Reserve(1);
		stats.Emplace(StatTypes::Mana, 123);
		cType = ItemTypes::Staff1;
		cQualitity = ItemQualities::Normal;
		cEquipmentLocation = ItemEquipmentLocations::Weapon;
		cQuantityLimit = 0;
		quantity = 0;
		cCastDelay = 0.5f;
		lastCastTime = 0;
		cShootDistance = 20;
		cBulletRadius = 16;
		cBulletSpeed = 300;
		cBulletLifeSeconds = 5;
		cBulletDamage = 2;
	}

	inline void Item_Sword1::Init(Monster* owner_) {
		if (owner_) {
			owner = owner_;
		}
		//pos
		frame = gRes.sword_16;
		radius = 32;
		radians = 0;
		stats.Reserve(1);
		stats.Emplace(StatTypes::Health, 123);
		cType = ItemTypes::Sword1;
		cQualitity = ItemQualities::Normal;
		cEquipmentLocation = ItemEquipmentLocations::Weapon;
		cQuantityLimit = 0;
		quantity = 0;
		cCastDelay = 0.5f;
		lastCastTime = 0;
		cShootDistance = 30;
		cBulletRadius = 32;
		cBulletDamage = 3;
	}

};
