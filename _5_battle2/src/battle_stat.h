#pragma once

namespace Battle {

	// int array's index
	enum class StatTypes : uint32_t {
		Health,
		Mana,
		Damage,
		// ...
	};

	struct alignas(8) Stat {
		StatTypes type;
		int32_t value;
	};

	// int array
	struct alignas(8) StatInfo {
		int32_t health{};
		int32_t mana{};
		int32_t damage{};
		// ...

		void Append(Stat const& sp);
	};

}
/*
* todo: 

// char's stats

enum class StatTypes : uint32_t {
	Health,
	HealthRegeneration,		// + value for every seconds

	Mana,
	ManaRegeneration,

	Damage,
	CriticalHitChance,		// 0 ~ 1. > 1: convert to + CriticalHitDamage
	CriticalHitDamage,		// 1 ~ n
	AttackSpeed,

	Defense,				// 1 - D / ( V + D )  ==>  V / ( V + D )      ( V default == 100 )
	Evasion,				// 0 ~ 
	MoveSpeed,
	// ...
};

// todo: item's stats

struct alignas(8) Stat {
	ItemTypes type;
	int32_t index;
	double value;
};

struct alignas(8) StatInfo {
	double health{};
	double healthRegeneration{};

	double mana{};
	double manaRegeneration{};

	double damage{};
	double criticalHitChance{};
	double criticalHitDamage{};
	double attackSpeed{};

	double defense{};
	double evasion{};
	double moveSpeed{};
	// ...

	void Append(Stat const& sp);
};
*/