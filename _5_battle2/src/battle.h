#pragma once
#include "looper.h"
#include "battle_action.h"
#include "battle_monster.h"
#include "battle_bladelight.h"
#include "battle_explosion.h"
#include "battle_effect_text.h"
#include "battle_block.h"
#include "battle_scene.h"

// simulate monsters death match battle
// action == skill == behavior == buff

// todo:
// 
// move ui: button to looper
// refine: keep more than 1 scene
// 
// click button, begin new match
// 
// 1. random drop some weapons on floor ( knife, staff )
// 2. random generate monsters
// 3. monsters init AI is search & pickup weapon & equip
// 4. switch special AI for weapon
// 5. monsters use weapon to hit each others
// 
// can equip more than 1 weapons?
// group team?
// 
// drop item
// pickup
// projectile?
// block path enhance
// "we are warriors" simulate?
// knockback
// bag
// 
// Health Damage DamageReduction / Defense / Armor
// HealthRegeneration HealthOnHit ...
// Evasion
// AttackSpeed
// CriticalHitChance CriticalHitDamage

#include "battle_scene.hpp"
#include "battle_block.hpp"
#include "battle_bladelight.hpp"
#include "battle_effect_text.hpp"
#include "battle_explosion.hpp"
#include "battle_monster.hpp"
#include "battle_monster_action_add.hpp"
#include "battle_monster_action_case.hpp"
#include "battle_monster_action_utils.hpp"
#include "battle_monster_actions.hpp"
// ...
