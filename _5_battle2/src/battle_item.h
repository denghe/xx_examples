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

	struct Monster;
	struct Item {
		xx::SpaceWeak<Monster> owner;			// == null mean on floor
		XY pos;									// floor pos or owner's offset pos
		float radians;							// for display
		int32_t quantity;

		// fill by Generate
		ItemTypes type;
		ItemQualities qualitity;
		ItemEquipmentLocations equipmentLocation;
		int32_t quantityLimit;
		xx::Shared<Skill> skill;
		xx::TinyList<Stat> stats;
		// todo: stats for skill ?

		static Item Generate(ItemTypes type);
	};

}
