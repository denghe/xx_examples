#pragma once

namespace Battle {

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


	// todo

	struct alignas(8) Stat2 {
		ItemTypes type;
		int32_t index;
		double value;
	};

	struct alignas(8) StatInfo2 {
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

		void Append(Stat2 const& sp);
	};

}
