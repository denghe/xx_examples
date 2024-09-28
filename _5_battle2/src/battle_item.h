#pragma once

namespace Battle {

	struct Monster;
	struct Item : Drawable, xx::SpaceGrid2Item<Item> {
		Item() = default;
		Item(Item const&) = delete;
		Item& operator=(Item const&) = delete;
		virtual ~Item() {}
		virtual bool Cast(Monster* target) { return false; };			// return true: success
		// ... more cast ?

		xx::SpaceGridWeak<Monster> owner;								// empty mean on floor
		xx::TinyList<Stat> stats;
		ItemTypes cType;
		ItemQualities cQualitity;
		ItemEquipmentLocations cEquipmentLocation;
		int32_t cQuantityLimit;
		int32_t quantity;

		float cCastDelay;
		float lastCastTime;
		float cCastMinDistance;
		// ...
	};

	struct Item_Potion1 : Item {
		float cPercentage;

		Item_Potion1() = default;
		void Init(Monster* owner_);
		bool Cast(Monster* target) override;
	};

	struct Item_Staff1 : Item {
		float cShootDistance;
		float cBulletRadius;
		float cBulletSpeed;
		float cBulletLifeSeconds;
		int32_t cBulletDamage;

		Item_Staff1() = default;
		void Init(Monster* owner_);
		bool Cast(Monster* target) override;
	};

	struct Item_Sword1 : Item {
		float cShootDistance;
		float cBulletRadius;
		int32_t cBulletDamage;

		Item_Sword1() = default;
		void Init(Monster* owner_);
		bool Cast(Monster* target) override;
	};

}
