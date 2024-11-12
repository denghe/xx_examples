#pragma once

namespace Msgs {
	namespace Global {

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

		using Stat_t = FX64;

		struct alignas(8) StatItem {
			StatTypes type{};
			Stat_t value{};
		};

		struct alignas(8) StatPanelBase {
			Stat_t health{};							// + life max, energy max
			Stat_t vitality{};							// + life regeneration, energy regeneration
			Stat_t strength{};							// + damage scale
			Stat_t dexterity{};							// + evasion, movement speed
			Stat_t defense{};							// + damage reduce scale
			Stat_t wisdom{};							// + experience scale
			Stat_t lucky{};								// + critical chance, critical bonus, improve drop rate

			void Clear();
			Stat_t& At(StatTypes t) const;
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

			void Clear();
			Stat_t& At(StatTypes t) const;
		};

		struct alignas(8) StatPanelExt : StatPanel {
			Stat_t attackSpeed1;
			Stat_t attackSpeed2;
			Stat_t attackSpeed3;
			// ...
			//Stat_t dummy[1000];

			void Clear();
			Stat_t& At(StatTypes t) const;
		};

	}
}

// todo: serde template
