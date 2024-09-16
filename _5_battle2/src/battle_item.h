#pragma once

namespace Battle {

	enum class ItemEquipmentLocations : uint8_t {
		Bag,
        Helm,
        Amulet,
        Shoulder,
        Armor,
        Belt,
        Pants,
        Glove,
        Boots,
        Ring,
        Weapon,
        Shield,
        Accessory,
		// ...
	};

	enum class ItemQualities : uint8_t {
		Normal,
		Excellent,
		Rare,
		Epic,
		Legendary,
		Ancient,
		Archaic
		// ...
	};

	enum class ItemTypes : uint16_t {
		Potion1,
		Staff1,
		Sword1,
		// ...
	};

	// cXxxxx prefix c: const, config

	struct Monster;
	struct Item {
		Item(Item const&) = delete;
		Item& operator=(Item const&) = delete;
		virtual ~Item() {}
		virtual bool Cast(Monster* target) {};							// return true: success
		// ... more cast ?

		xx::SpaceWeak<Monster> owner;									// == null mean on floor
		XY pos;															// floor pos or owner's offset pos
		float radius;
		float radians;
		xx::Ref<StatPack> statPack;										// stats store
		ItemTypes cType;
		ItemQualities cQualitity;
		ItemEquipmentLocations cEquipmentLocation;
		int32_t cQuantityLimit;
		int32_t quantity;

		float cChargeSpeed;												// = cast speed( count per seconds ) / FPS
		/*
			chargePool += cChargeSpeed;
			int count = (int)chargePool;
			chargePool -= count;
			for i to count  cast
		*/
		float chargePool;

		// ...
	};

	struct Item_Potion1 : Item {
		float cPercentage;

		void Init(Monster* owner_);
		bool Cast(Monster* target) override;
	};

	struct Item_Staff1 : Item {
		float cShootDistance;
		float cBulletRadius;
		float cBulletSpeed;
		float cBulletLifeSeconds;
		int32_t cBulletDamage;

		void Init(Monster* owner_);
		bool Cast(Monster* target) override;
	};

	struct Item_Sword1 : Item {
		float cShootDistance;
		float cBulletRadius;
		int32_t cBulletDamage;

		void Init(Monster* owner_);
		bool Cast(Monster* target) override;
	};

}
