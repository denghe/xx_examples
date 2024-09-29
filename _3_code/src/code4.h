#pragma once
#include <looper.h>

namespace Code4 {

	// simulate Char + Item + Stat

	/**********************************************************************************************/
	/**********************************************************************************************/

	struct StatPanel {
		double health;					// + life max, energy max
		double vitality;				// + life regeneration, energy regeneration
		double strength;				// + damage scale
		double dexterity;				// + evasion, movement speed
		double defense;					// + damage reduce scale
		double wisdom;					// + experience scale
		double lucky;					// + critical chance, critical bonus, improve drop rate
		// ...
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

		void Dump() {
			xx::CoutFormat(R"(StatPanel {{
    health = {0},
    vitality = {1},
    strength = {2},
    dexterity = {3},
    defense = {4},
    wisdom = {5},
    lucky = {6},
    life = {7},
    lifeRegeneration = {8},
    energy = {9},
    energyRegeneration = {10},
    damageScale = {11},
    defenseScale = {12},
    evasion = {13},
    movementSpeed = {14},
    experienceScale = {15},
    criticalChance = {16},
    criticalBonus = {17}
})",
				health,
				vitality,
				strength,
				dexterity,
				defense,
				wisdom,
				lucky,
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
		}
	};

	enum class StatIndexs : uint32_t {
		health = 0,
		vitality,
		strength,
		dexterity,
		defense,
		wisdom,
		lucky,
		MIDDLE_VALUE = 7,		// == next one value
		life = 7,
		lifeRegeneration,
		energy,
		energyRegeneration,
		damageScale,
		defenseScale,
		evasion,
		movementSpeed,
		experienceScale,
		criticalChance,
		criticalBonus,
		MAX_VALUE
	};

	struct StatItem {
		StatIndexs index;
		double value;
	};

	struct Equipment {
		xx::TinyList<StatItem> stats1;		// 0 ~ 6
		xx::TinyList<StatItem> stats2;		// 7 ~ 17
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
		double level;
		double experience;
		CharConfigs cfg;
		StatPanel base;
		StatPanel curr;
		xx::TinyList<Equipment> equipments;

		void Init() {
			level = 1;
			experience = 0;
			BaseCalc1();
			BaseCalc2();
			curr = base;
		}

		void BaseCalc1() {
			base.health = cfg.initHealth + (this->level - 1) * cfg.levelToHealthRatio;
			base.vitality = cfg.initVitality + (this->level - 1) * cfg.levelToVitalityRatio;
			base.strength = cfg.initStrength + (this->level - 1) * cfg.levelToStrengthRatio;
			base.dexterity = cfg.initDexterity + (this->level - 1) * cfg.levelToDexterityRatio;
			base.defense = cfg.initDefense + (this->level - 1) * cfg.levelToDefenseRatio;
			base.wisdom = cfg.initWisdom + (this->level - 1) * cfg.levelToWisdomRatio;
			base.lucky = cfg.initLucky + (this->level - 1) * cfg.levelToLuckyRatio;

			if (auto equipmentsCount = equipments.Count()) {
				for (int32_t ei = 0; ei < equipmentsCount; ++ei) {
					auto& e = equipments[ei];
					if (auto statsCount = e.stats1.Count()) {
						for (int32_t si = 0; si < statsCount; ++si) {
							auto& s = e.stats1[si];
							assert((uint32_t)s.index < (uint32_t)StatIndexs::MIDDLE_VALUE);
							((double*)&base)[(uint32_t)s.index] += s.value;
						}
					}
				}
			}
		}

		void BaseCalc2() {
			base.life = base.health * cfg.healthToLifeRatio;
			base.energy = base.health * cfg.healthToEnergyRatio;
			base.lifeRegeneration = base.vitality * cfg.vitalityToLifeRegenerationRatio;
			base.energyRegeneration = base.vitality * cfg.vitalityToEnergyRegenerationRatio;
			base.damageScale = base.strength * cfg.strengthToDamageScaleRatio;
			base.defenseScale = cfg.defenseFactor / (cfg.defenseFactor + base.defense);
			base.evasion = cfg.evasionFactor / (cfg.evasionFactor + base.dexterity);
			base.movementSpeed = cfg.baseMovementSpeed + base.dexterity * cfg.dexterityToMovementSpeedRatio;
			base.experienceScale = base.wisdom * cfg.wisdomToExperienceScaleRatio;
			base.criticalChance = base.lucky * cfg.luckyToCritialChanceScaleRatio;
			base.criticalBonus = base.lucky * cfg.luckyToCritialBonusScaleRatio;

			if (auto equipmentsCount = equipments.Count()) {
				for (int32_t ei = 0; ei < equipmentsCount; ++ei) {
					auto& e = equipments[ei];
					if (auto statsCount = e.stats2.Count()) {
						for (int32_t si = 0; si < statsCount; ++si) {
							auto& s = e.stats2[si];
							assert((uint32_t)s.index >= (uint32_t)StatIndexs::MIDDLE_VALUE);
							assert((uint32_t)s.index < (uint32_t)StatIndexs::MAX_VALUE);
							((double*)&base)[(uint32_t)s.index] += s.value;
						}
					}
				}
			}
		}

		void BaseToCurr() {
			auto bak_life = curr.life;
			auto bak_energy = curr.energy;
			curr = base;
			curr.life = bak_life;
			curr.energy = bak_energy;
		}

		void Update() {
			BaseCalc1();
			BaseCalc2();
			BaseToCurr();
		}

		void Dump() {
			xx::Cout("base = ");
			base.Dump();
			//xx::Cout("\ncurr = ");
			//curr.Dump();
			xx::CoutN();
		}
	};

	/*********************************************************************************************/
	/*********************************************************************************************/

	inline void Test() {
		Char c;
		// add some item for test
		{
			auto& e = c.equipments.Emplace();
			e.stats1.Emplace(StatIndexs::health, 5);
			e.stats1.Emplace(StatIndexs::vitality, 6);
		}
		{
			auto& e = c.equipments.Emplace();
			e.stats2.Emplace(StatIndexs::movementSpeed, 100);
		}
		c.Init();
		xx::CoutN("after Init()");
		c.Dump();
		xx::CoutN("after Update()");
		c.Update();
		c.Dump();
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