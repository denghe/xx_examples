#pragma once
#include <looper.h>

namespace Code4 {

	// simulate Char + Item + Stat

#define STAT_ENABLE_CONSOLE_DUMP

	/**********************************************************************************************/
	/**********************************************************************************************/

	struct alignas(8) StatPanelPoints {
		double health;					// + life max, energy max
		double vitality;				// + life regeneration, energy regeneration
		double strength;				// + damage scale
		double dexterity;				// + evasion, movement speed
		double defense;					// + damage reduce scale
		double wisdom;					// + experience scale
		double lucky;					// + critical chance, critical bonus, improve drop rate
		// ...

		void Dump() {
#ifdef STAT_ENABLE_CONSOLE_DUMP
			xx::CoutFormat(R"(StatPanelPoints {{
    health = {0},
    vitality = {1},
    strength = {2},
    dexterity = {3},
    defense = {4},
    wisdom = {5},
    lucky = {6},
})",
				health,
				vitality,
				strength,
				dexterity,
				defense,
				wisdom,
				lucky
			);
#endif;
		}
	};

	struct alignas(8) StatPanelResults {
		double life;
		double lifeRegeneration;
		double energy;
		double energyRegeneration;
		double damageScale;
		double defenseScale;
		double evasion;
		double movementSpeed;
		double experienceScale;
		double criticalChance;
		double criticalBonus;
		// ...

		void ClearResults() {
			memset(this, 0, sizeof(StatPanelResults));
		}

		void Dump() {
#ifdef STAT_ENABLE_CONSOLE_DUMP
			xx::CoutFormat(R"(StatPanelResults {{
    life = {0},
    lifeRegeneration = {1},
    energy = {2},
    energyRegeneration = {3},
    damageScale = {4},
    defenseScale = {5},
    evasion = {6},
    movementSpeed = {7},
    experienceScale = {8},
    criticalChance = {9},
    criticalBonus = {10}
})",
				life,
				lifeRegeneration,
				energy,
				energyRegeneration,
				damageScale,
				defenseScale,
				evasion,
				movementSpeed,
				experienceScale,
				criticalChance,
				criticalBonus
			);
#endif
		}
	};

	struct alignas(8) StatPanel : StatPanelPoints, StatPanelResults {
		void Dump() {
			StatPanelPoints::Dump();
			StatPanelResults::Dump();
		}
	};

	// todo: more StatTypes for  ??? = level * n.  map to calculate function

	enum class StatTypes : uint32_t {
		health,					__POINTS_BEGIN__ = health,
		vitality,
		strength,
		dexterity,
		defense,
		wisdom,
		lucky,					__POINTS_END__ = lucky,
		life,					__RESULTS_BEGIN__ = life,
		lifeRegeneration,
		energy,
		energyRegeneration,
		damageScale,
		defenseScale,
		evasion,
		movementSpeed,
		experienceScale,
		criticalChance,
		criticalBonus,			__RESULTS_END__ = criticalBonus
		// ... exts
	};

	struct StatItem {
		StatTypes type;
		double value;
	};

	struct Equipment {
		// ...
		xx::TinyList<StatItem> stats;
		// ...
	};

	struct CharConfigs {
		double initHealth = 10;
		double initVitality = 10;
		double initStrength = 10;
		double initDexterity = 10;
		double initDefense = 10;
		double initWisdom = 10;
		double initLucky = 10;

		double levelToHealthRatio = 2;
		double levelToVitalityRatio = 2;
		double levelToStrengthRatio = 2;
		double levelToDexterityRatio = 2;
		double levelToDefenseRatio = 2;
		double levelToWisdomRatio = 2;
		double levelToLuckyRatio = 2;

		double healthToLifeRatio = 2;
		double healthToEnergyRatio = 5;
		double vitalityToLifeRegenerationRatio = 0.2;
		double vitalityToEnergyRegenerationRatio = 0.5;
		double strengthToDamageScaleRatio = 0.1;
		double defenseFactor = 100;
		double evasionFactor = 100;
		double baseMovementSpeed = 300;
		double dexterityToMovementSpeedRatio = 0.1;
		double wisdomToExperienceScaleRatio = 0.1;
		double luckyToCritialChanceScaleRatio = 0.01;
		double luckyToCritialBonusScaleRatio = 0.1;
	};

	struct Char {
		// ...

		double level;
		double experience;
		xx::TinyList<Equipment> equipments;
		CharConfigs cfg;
		StatPanel sp;			// max value
		double life, energy;	// current

		void Init() {
			level = 1;
			experience = 0;
			CalculateSP();
			life = sp.life;
			energy = sp.energy;
		}

		XX_INLINE void CalculateSP() {
			// calculate points by level
			sp.health = cfg.initHealth + (this->level - 1) * cfg.levelToHealthRatio;
			sp.vitality = cfg.initVitality + (this->level - 1) * cfg.levelToVitalityRatio;
			sp.strength = cfg.initStrength + (this->level - 1) * cfg.levelToStrengthRatio;
			sp.dexterity = cfg.initDexterity + (this->level - 1) * cfg.levelToDexterityRatio;
			sp.defense = cfg.initDefense + (this->level - 1) * cfg.levelToDefenseRatio;
			sp.wisdom = cfg.initWisdom + (this->level - 1) * cfg.levelToWisdomRatio;
			sp.lucky = cfg.initLucky + (this->level - 1) * cfg.levelToLuckyRatio;
			// gather points & results from items
			sp.ClearResults();
			if (auto equipmentsCount = equipments.Count(); equipmentsCount > 0) {
				for (int32_t ei = 0; ei < equipmentsCount; ++ei) {
					auto& e = equipments[ei];
					if (auto statsCount = e.stats.Count(); statsCount > 0) {
						for (int32_t si = 0; si < statsCount; ++si) {
							auto& s = e.stats[si];
							if ((uint32_t)s.type <= (uint32_t)StatTypes::__RESULTS_END__) {
								((double*)&sp)[(uint32_t)s.type] += s.value;
							} else {
								// todo: call mapped handle func
							}
						}
					}
				}
			}
			// calculate final results
			sp.life += sp.health * cfg.healthToLifeRatio;
			sp.energy += sp.health * cfg.healthToEnergyRatio;
			sp.lifeRegeneration += sp.vitality * cfg.vitalityToLifeRegenerationRatio;
			sp.energyRegeneration += sp.vitality * cfg.vitalityToEnergyRegenerationRatio;
			sp.damageScale += sp.strength * cfg.strengthToDamageScaleRatio;
			sp.defenseScale += cfg.defenseFactor / (cfg.defenseFactor + sp.defense);
			sp.evasion += cfg.evasionFactor / (cfg.evasionFactor + sp.dexterity);
			sp.movementSpeed += cfg.baseMovementSpeed + sp.dexterity * cfg.dexterityToMovementSpeedRatio;
			sp.experienceScale += sp.wisdom * cfg.wisdomToExperienceScaleRatio;
			sp.criticalChance += sp.lucky * cfg.luckyToCritialChanceScaleRatio;
			sp.criticalBonus += sp.lucky * cfg.luckyToCritialBonusScaleRatio;
			// handle regenerations
			if (life < sp.life) life += sp.lifeRegeneration;
			if (energy < sp.energy) energy += sp.energyRegeneration;
			// handle value range
			if (life > sp.life) life = sp.life;
			if (energy > sp.energy) energy = sp.energy;
			if (sp.criticalChance > 1) sp.criticalBonus += sp.criticalChance - 1;
			if (sp.movementSpeed >= 1000) sp.movementSpeed = 1000;
			if (sp.defenseScale >= 1) sp.defenseScale = 0.99999;
			if (sp.evasion >= 1) sp.evasion = 0.99999;
		}

		void Update() {
			CalculateSP();
			// ...
		}
	};

	/*********************************************************************************************/
	/*********************************************************************************************/

	inline void Test() {
		Char c;
		// add some item for test
		{
			auto& e = c.equipments.Emplace();
			e.stats.Emplace(StatTypes::health, 5);
			e.stats.Emplace(StatTypes::vitality, 6);
		}
		{
			auto& e = c.equipments.Emplace();
			e.stats.Emplace(StatTypes::movementSpeed, 100);
		}
		c.Init();
		xx::CoutN("after Init()");
		c.sp.Dump();
		xx::CoutN("after Update()");
		c.Update();
		c.sp.Dump();
		xx::CoutN("simulate hurt");
		c.life -= 10;
		xx::CoutN("life = ", c.life);
		c.Update();
		xx::CoutN("life = ", c.life);
		c.Update();
		xx::CoutN("life = ", c.life);
		c.sp.Dump();
	}

};








//enum class StatTypes_Body : uint32_t {
//	Life,
//	LifeRegeneration,		// 0 ~ n ( percentage )

//	Energy,
//	EnergyRegeneration,

//	//Damage,					// 1 ~ n ( percentage )
//	//CritChance,				// 0 ~ n. n > 1: CritDamage += CritChance - 1
//	//CritDamage,				// 1 ~ n ( percentage )
//	////CoolDown/AttackSpeed	// 0 ~ 0.99999.. ( percentage )

//	Defense,				// 0 ~ n. 1 - D / ( V + D )  ==>  V / ( V + D )      ( V default == 100 )
//	Evasion,				// 0 ~ n. V / ( V + E )      ( V default == 100 )
//	MovementSpeed,
//	// ...

//	// health		->  Life, LifeRegeneration, Energy, EnergyRegeneration
//	// strength		->	DamageScale, DefenseScale
//	// dexterity	->	Evasion, MovementSpeed
//	// wisdom		->  ExperienceScale

//	// DamageScale
//	// HealingScale
//	// ExperienceScale
//};

//enum class StatTypes_Potion1 : uint32_t {
//	HealthRestore			// body.Health += HealthRestore
//};

//enum class StatTypes_Potion2 : uint32_t {
//	EnergyRestore			// body.Energy += EnergyRestore
//};

//enum class StatTypes_Staff1 : uint32_t {
//	BaseDamage,				// final damage = BaseDamage * body.Damage
//	AimRange,
//	FireballRadius,
//	AttackSpeed,
//	EnergyCost,
//};

//enum class StatTypes_Sword1 : uint32_t {
//	BaseDamage,
//	SwordLength,
//	AttackSpeed,
//	EnergyCost,
//};

// ...