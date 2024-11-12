#pragma once
#include <looper.h>

namespace Code4 {

	// simulate Char + Item + Stat

#define STAT_ENABLE_CONSOLE_DUMP

	enum class StatTypes : uint32_t {
		health,
		vitality,
		strength,
		dexterity,
		defense,
		wisdom,
		lucky, __POINTS_END__ = lucky,

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
		criticalBonus, __RESULTS_END__ = criticalBonus,

		attackSpeed1,
		attackSpeed2,
		attackSpeed3,
		// ...

		__MAX_VALUE__
	};

	static constexpr std::array<const char*, (uint32_t)StatTypes::__MAX_VALUE__> strings_StatTypes{
		"health",
		"vitality",
		"strength",
		"dexterity",
		"defense",
		"wisdom",
		"lucky",

		"life",
		"lifeRegeneration",
		"energy",
		"energyRegeneration",
		"damageScale",
		"defenseScale",
		"evasion",
		"movementSpeed",
		"experienceScale",
		"criticalChance",
		"criticalBonus",

		"attackSpeed1",
		"attackSpeed2",
		"attackSpeed3",
		// ...
	};

	using Stat_t = float;
	//using Stat_t = double;

	struct alignas(8) StatItem {
		StatTypes type{};
		Stat_t value{};
	};

	struct alignas(8) StatPanelBase {
		Stat_t health{};						// + life max, energy max
		Stat_t vitality{};						// + life regeneration, energy regeneration
		Stat_t strength{};						// + damage scale
		Stat_t dexterity{};						// + evasion, movement speed
		Stat_t defense{};						// + damage reduce scale
		Stat_t wisdom{};						// + experience scale
		Stat_t lucky{};							// + critical chance, critical bonus, improve drop rate

		void Clear() {
			memset(this, 0, sizeof(StatPanelBase));
		}

		XX_INLINE Stat_t& At(StatTypes t) const {
			assert((uint32_t)t <= (uint32_t)StatTypes::__POINTS_END__);
			return ((Stat_t*)this)[(uint32_t)t];
		}

		void Dump() {
			for (uint32_t i = 0; i <= (uint32_t)StatTypes::__POINTS_END__; ++i) {
				xx::CoutN(strings_StatTypes[i], " = ", At((StatTypes)i));
			}
		}
	};

	struct alignas(8) StatPanel : StatPanelBase {
		Stat_t life{};
		Stat_t lifeRegeneration{};
		Stat_t energy{};
		Stat_t energyRegeneration{};
		Stat_t damageScale{};
		Stat_t defenseScale{};
		Stat_t evasion{};
		Stat_t movementSpeed{};
		Stat_t experienceScale{};
		Stat_t criticalChance{};
		Stat_t criticalBonus{};

		void Clear() {
			memset(this, 0, sizeof(StatPanel));
		}

		XX_INLINE Stat_t& At(StatTypes t) const {
			assert((uint32_t)t <= (uint32_t)StatTypes::__RESULTS_END__);
			return ((Stat_t*)this)[(uint32_t)t];
		}

		void Dump() {
			for (uint32_t i = 0; i <= (uint32_t)StatTypes::__RESULTS_END__; ++i) {
				xx::CoutN(strings_StatTypes[i], " = ", At((StatTypes)i));
			}
		}
	};

	struct alignas(8) StatPanelExt : StatPanel {
		Stat_t attackSpeed1;
		Stat_t attackSpeed2;
		Stat_t attackSpeed3;
		// ...
		//Stat_t dummy[1000];

		void Clear() {
			memset(this, 0, sizeof(StatPanelExt));
		}

		XX_INLINE Stat_t& At(StatTypes t) const {
			assert((uint32_t)t <= (uint32_t)StatTypes::__MAX_VALUE__);
			return ((Stat_t*)this)[(uint32_t)t];
		}

		void Dump() {
			for (uint32_t i = 0; i <= (uint32_t)StatTypes::__MAX_VALUE__; ++i) {
				xx::CoutN(strings_StatTypes[i], " = ", At((StatTypes)i));
			}
		}
	};

	struct Char;
	struct Equipment {
		virtual ~Equipment() {}
		xx::Weak<Char> owner;
		xx::TinyList<StatItem> stats;
		// ...
	};

	struct CharConfigs {
		Stat_t initHealth{10};
		Stat_t initVitality{10};
		Stat_t initStrength{10};
		Stat_t initDexterity{10};
		Stat_t initDefense{10};
		Stat_t initWisdom{10};
		Stat_t initLucky{10};

		Stat_t levelToHealthRatio{2};
		Stat_t levelToVitalityRatio{2};
		Stat_t levelToStrengthRatio{2};
		Stat_t levelToDexterityRatio{2};
		Stat_t levelToDefenseRatio{2};
		Stat_t levelToWisdomRatio{2};
		Stat_t levelToLuckyRatio{2};

		Stat_t healthToLifeRatio{2};
		Stat_t healthToEnergyRatio{5};
		Stat_t vitalityToLifeRegenerationRatio{0.2};
		Stat_t vitalityToEnergyRegenerationRatio{0.5};
		Stat_t strengthToDamageScaleRatio{0.1};
		Stat_t defenseFactor{100};
		Stat_t evasionFactor{100};
		Stat_t baseMovementSpeed{300};
		Stat_t dexterityToMovementSpeedRatio{0.1};
		Stat_t wisdomToExperienceScaleRatio{0.1};
		Stat_t luckyToCritialChanceScaleRatio{0.01};
		Stat_t luckyToCritialBonusScaleRatio{0.1};
	};

	struct Char {
		// ...

		xx::Ref<CharConfigs> cfg;
		xx::TinyList<xx::Shared<Equipment>> equipments;
		Stat_t level{}, experience{};
		Stat_t life{}, energy{};

		bool spDirty{ true };
		StatPanel sp;

		void Init() {
			level = 1;
			experience = 0;
			UpdateSP();
			life = sp.life;
			energy = sp.energy;
		}

		void UpdateSP() {
			if (spDirty) {
				spDirty = false;
				sp.Clear();
				// calculate points by level
				sp.health = cfg->initHealth + (this->level - 1) * cfg->levelToHealthRatio;
				sp.vitality = cfg->initVitality + (this->level - 1) * cfg->levelToVitalityRatio;
				sp.strength = cfg->initStrength + (this->level - 1) * cfg->levelToStrengthRatio;
				sp.dexterity = cfg->initDexterity + (this->level - 1) * cfg->levelToDexterityRatio;
				sp.defense = cfg->initDefense + (this->level - 1) * cfg->levelToDefenseRatio;
				sp.wisdom = cfg->initWisdom + (this->level - 1) * cfg->levelToWisdomRatio;
				sp.lucky = cfg->initLucky + (this->level - 1) * cfg->levelToLuckyRatio;
				// gather points & results from items
				if (auto equipmentsCount = equipments.Count(); equipmentsCount > 0) {
					for (int32_t ei = 0; ei < equipmentsCount; ++ei) {
						auto& e = equipments[ei];
						if (auto statsCount = e->stats.Count(); statsCount > 0) {
							for (int32_t si = 0; si < statsCount; ++si) {
								auto& s = e->stats[si];
								sp.At(s.type) += s.value;
							}
						}
					}
				}
				// calculate final results
				sp.life += sp.health * cfg->healthToLifeRatio;
				sp.energy += sp.health * cfg->healthToEnergyRatio;
				sp.lifeRegeneration += sp.vitality * cfg->vitalityToLifeRegenerationRatio;
				sp.energyRegeneration += sp.vitality * cfg->vitalityToEnergyRegenerationRatio;
				sp.damageScale += sp.strength * cfg->strengthToDamageScaleRatio;
				sp.defenseScale += cfg->defenseFactor / (cfg->defenseFactor + sp.defense);
				sp.evasion += cfg->evasionFactor / (cfg->evasionFactor + sp.dexterity);
				sp.movementSpeed += cfg->baseMovementSpeed + sp.dexterity * cfg->dexterityToMovementSpeedRatio;
				sp.experienceScale += sp.wisdom * cfg->wisdomToExperienceScaleRatio;
				sp.criticalChance += sp.lucky * cfg->luckyToCritialChanceScaleRatio;
				sp.criticalBonus += sp.lucky * cfg->luckyToCritialBonusScaleRatio;
				// handle value range
				if (sp.criticalChance > 1) sp.criticalBonus += sp.criticalChance - 1;
				if (sp.movementSpeed >= 1000) sp.movementSpeed = 1000;
				if (sp.defenseScale >= 1) sp.defenseScale = 0.99999;
				if (sp.evasion >= 1) sp.evasion = 0.99999;
			}
			// handle regenerations
			if (life < sp.life) {
				life += sp.lifeRegeneration;
				if (life > sp.life) life = sp.life;
			}
			if (energy < sp.energy) {
				energy += sp.energyRegeneration;
				if (energy > sp.energy) energy = sp.energy;
			}
		}

		void Update() {
			UpdateSP();	// need update all monster sp first
			// ...
		}
	};

	struct Equipment1 : Equipment {
		Stat_t damage{};
		Stat_t attackSpeed{};
		Equipment1(xx::Shared<Char> const& owner_) {
			owner = owner_;
			damage = 1;
			attackSpeed = 2;
			stats.Emplace(StatTypes::health, 5);
			stats.Emplace(StatTypes::vitality, 6);
		}
	};

	struct Equipment2 : Equipment {
		Equipment2(xx::Shared<Char> const& owner_) {
			owner = owner_;
			stats.Emplace(StatTypes::movementSpeed, 100);
		}
	};

	struct Projectile {
		xx::Weak<Char> owner;
		xx::Weak<Equipment> emitter;
		virtual ~Projectile() {};
		virtual bool Hurt(Char* tar) { return false; }
	};

	struct Bullet1 : Projectile {
		Stat_t damage{};											// emitter's props copy here
		Stat_t damageScale{}, criticalChance{}, criticalBonus{};	// owner's props copy here

		void Init(Equipment1* emitter_) {
			assert(emitter_);
			owner = emitter_->owner;
			emitter = xx::WeakFromThis(emitter_);
			auto& sp = emitter_->owner->sp;
			// copy props
			damage = emitter_->damage;
			damageScale = sp.damageScale;
			criticalChance = sp.criticalChance;
			criticalBonus = sp.criticalBonus;
		}

		// tar->life -= d ( tar->life == 0  mean  is dead )
		bool Hurt(Char* tar) override {
			auto d = damage * damageScale;
			auto crit = gLooper.rnd.Next<float>() < criticalChance;
			if (crit) {
				d += d * criticalBonus;
			}
			tar->life -= d;
			if (tar->life <= 0) {
				tar->life = 0;
				// todo: emitter + exp ? update: check exp & level up ?
			}
			return crit;
		}
	};


	/*********************************************************************************************/
	/*********************************************************************************************/

	inline void Test() {

		xx::Listi32<xx::Shared<Char>> chars;
		for (int j = 0; j < 100000; j++) {
			auto c = chars.Emplace().Emplace();
			c->cfg.Emplace();
			c->equipments.Emplace( xx::MakeShared<Equipment1>(c) );
			c->equipments.Emplace( xx::MakeShared<Equipment1>(c) );
			c->equipments.Emplace( xx::MakeShared<Equipment1>(c) );
			c->equipments.Emplace( xx::MakeShared<Equipment2>(c) );
			c->equipments.Emplace( xx::MakeShared<Equipment2>(c) );
			c->equipments.Emplace( xx::MakeShared<Equipment2>(c) );
			c->Init();
		}
		auto secs = xx::NowEpochSeconds();
		for (int i = 0; i < 1000; ++i) {
			for (int j = 0; j < 100000; j++) {
				chars[j]->Update();
			}
		}
		xx::CoutN("secs = ", xx::NowEpochSeconds(secs));

		{
			auto c = chars[0];
			xx::CoutN("after Update()");
			c->sp.Dump();
			xx::CoutN("simulate hurt");
			c->life -= 10;
			xx::CoutN("life = ", c->life);
			c->Update();
			xx::CoutN("life = ", c->life);
			c->Update();
			xx::CoutN("life = ", c->life);
			c->level++;
			c->Update();
			xx::CoutN("level = ", c->level);
			xx::CoutN("life = ", c->life);
			c->sp.Dump();
			xx::CoutN("bullet hurt test");
			auto b = xx::MakeShared<Bullet1>();
			b->Init(dynamic_cast<Equipment1*>(c->equipments[0].pointer));
			auto r = b->Hurt(c);
			xx::CoutN("r = ", r, " life = ", c->life);
			r = b->Hurt(c);
			xx::CoutN("r = ", r, " life = ", c->life);
			r = b->Hurt(c);
			xx::CoutN("r = ", r, " life = ", c->life);
			r = b->Hurt(c);
			xx::CoutN("r = ", r, " life = ", c->life);
			r = b->Hurt(c);
			xx::CoutN("r = ", r, " life = ", c->life);
			r = b->Hurt(c);
			xx::CoutN("r = ", r, " life = ", c->life);
		}
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