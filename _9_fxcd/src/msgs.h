#pragma once
#include "looper.h"

namespace Msgs {
	inline xx::SerdeInfo gSerdeInfo;
	void InitSerdeInfo();

	namespace Global {	// id = 1 ~ 99
		struct Scene;
		struct Player;
		struct Monster;
		struct Block;
		struct Config_Monster;
		struct Bullet_Base;
		// ...
	}

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

// todo: monster.items

#include "msgs_global_effect_text.h"
#include "msgs_global_stat.h"
#include "msgs_global_item.h"					// 1
#include "msgs_global_scene.h"					// 3
#include "msgs_global_config_monster.h"			// 2
#include "msgs_global_block.h"					// 4
#include "msgs_global_player.h"					// 5
#include "msgs_global_monster.h"				// 6
#include "msgs_global_projectile.h"				// 7
#include "msgs_global_bullet_base.h"			// 8
#include "msgs_global_bullet_sector.h"			// 9
#include "msgs_global_bullet_box.h"				// 10
#include "msgs_c2s.h"
#include "msgs_s2c.h"


#include "msgs.hpp"
#include "msgs_global_effect_text.hpp"
#include "msgs_global_stat.hpp"
#include "msgs_global_item.hpp"
#include "msgs_global_config_monster.hpp"
#include "msgs_global_scene.hpp"
#include "msgs_global_block.hpp"
#include "msgs_global_player.hpp"
#include "msgs_global_monster.hpp"

#include "msgs_global_projectile.hpp"
#include "msgs_global_bullet_base.hpp"
#include "msgs_global_bullet_sector.hpp"
#include "msgs_global_bullet_box.hpp"

#include "msgs_c2s.hpp"
#include "msgs_s2c.hpp"
