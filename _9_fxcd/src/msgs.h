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
		struct Bullet_Base;
		// ...

	}
}


#include "msgs_global_effect_text.h"
#include "msgs_global_stat.h"
#include "msgs_global_scene.h"
#include "msgs_global_block.h"
#include "msgs_global_player.h"
#include "msgs_global_monster.h"
// todo: monster.items
// todo: Item/Skill? Gun?
#include "msgs_global_projectile.h"
#include "msgs_global_bullet_base.h"
#include "msgs_global_bullet_sector.h"
#include "msgs_global_bullet_box.h"
#include "msgs_c2s.h"
#include "msgs_s2c.h"


#include "msgs.hpp"
#include "msgs_global_effect_text.hpp"
//#include "msgs_global_stat.hpp"
#include "msgs_global_scene.hpp"
#include "msgs_global_block.hpp"
#include "msgs_global_player.hpp"
#include "msgs_global_monster.hpp"

//#include "msgs_global_projectile.hpp"
//#include "msgs_global_bullet_base.hpp"
//#include "msgs_global_bullet_sector.hpp"
//#include "msgs_global_bullet_box.hpp"
#include "msgs_global_bullet.hpp"

#include "msgs_c2s.hpp"
#include "msgs_s2c.hpp"
