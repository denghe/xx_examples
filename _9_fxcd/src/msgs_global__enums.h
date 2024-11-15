#pragma once
#include "looper.h"

namespace Msgs {
	namespace Global {

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
			Unknown,		// is not a item?
			Sword1,
			// ...
		};

	}
}
