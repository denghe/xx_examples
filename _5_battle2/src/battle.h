#pragma once
#include "looper.h"
#include "battle_enums.h"
#include "battle_drawable.h"
#include "battle_action.h"
#include "battle_stat.h"
#include "battle_item.h"
#include "battle_projectile.h"
#include "battle_projectile_fireball.h"
#include "battle_monster.h"
#include "battle_bladelight.h"
#include "battle_explosion.h"
#include "battle_effect_text.h"
#include "battle_block.h"
#include "battle_scene.h"

// simulate monsters death match battle

// todo
// 1. random drop some weapons on the ground ( sword, staff )
// 2. random generate monsters
// 3. monsters init AI is search & pickup weapon & equip
// 4. monsters use weapon to hit each others
// 5. when monster dead( has weapon ), drop weapon
// 
// support following stats: ( all double ? )
// Health Damage DamageReduction / Defense / Armor
// HealthRegeneration HealthOnHit ...
// Evasion
// AttackSpeed
// CriticalHitChance CriticalHitDamage
// Knockback, -Speed
// 
// 6. kill others, +EXP? or drop EXP stone? level up
// 
// monster group
// physics simulate
// simple path find
// "we are warriors" simulate?
// bag
// 
// can equip more than 1 weapons?
// AI blueprint?

#include "battle_drawable.hpp"
#include "battle_scene.hpp"
#include "battle_scene_init.hpp"
#include "battle_scene_update.hpp"
#include "battle_scene_draw.hpp"
#include "battle_stat.hpp"
#include "battle_projectile.hpp"
#include "battle_projectile_fireball.hpp"
#include "battle_item_init.hpp"
#include "battle_item_cast.hpp"
#include "battle_block.hpp"
#include "battle_bladelight.hpp"
#include "battle_effect_text.hpp"
#include "battle_explosion.hpp"
#include "battle_monster.hpp"
#include "battle_monster_action_add.hpp"
#include "battle_monster_action_case.hpp"
#include "battle_monster_action_utils.hpp"
#include "battle_monster_init.hpp"
#include "battle_monster_update.hpp"
#include "battle_monster_draw.hpp"
// ...
