#pragma once

namespace Battle {


	// int array's index
	enum class StatTypes : uint32_t {
		Health,
		Mana,
		Damage,
		// ...
	};

	/**********************************************************************************************/
	/**********************************************************************************************/

	enum class ActionTypes : int32_t {
		Stun,
		Search,
		MoveToItem,
		MoveToTarget,
		HitTarget,
		SetColor,
		// ...
		MaxValue
	};
	static_assert((int32_t)ActionTypes::MaxValue <= 64);	// uint64_t actionFlags limit 

	/**********************************************************************************************/
	/**********************************************************************************************/

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
		Body,		// is not a item
		Potion1,
		Potion2,
		Staff1,
		Sword1,
		// ...
	};

}
