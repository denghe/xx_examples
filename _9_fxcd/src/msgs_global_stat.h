#pragma once

namespace Msgs {
	namespace Global {

		enum class StatTypes : uint32_t {
			health,
			vitality,
			strength,
			// intelligence,  // no need now
			agility,
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
			Stat_t agility{};							// + evasion, movement speed
			Stat_t defense{};							// + damage reduce scale
			Stat_t wisdom{};							// + experience scale, critical bonus
			Stat_t lucky{};								// + critical chance, improve drop rate

			void Clear();
			Stat_t& At(StatTypes t) const;
		};

		struct Config_Monster;
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
			void CalculatePoints(Config_Monster* cfg, Stat_t level);		// step1
			template<typename Container>
			void CalculateStatList(Container const& stats);					// step2: for all items
			void CalculateResults(Config_Monster* cfg);						// step3
			Stat_t& At(StatTypes t) const;
		};

		// todo
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

namespace xx {
	template<> struct IsPod<Msgs::Global::StatItem, void> : std::true_type {};
	template<> struct IsPod<Msgs::Global::StatPanelBase, void> : std::true_type {};
	template<> struct IsPod<Msgs::Global::StatPanel, void> : std::true_type {};
	template<> struct IsPod<Msgs::Global::StatPanelExt, void> : std::true_type {};

	template<typename T>
	struct DataFuncs<T, std::enable_if_t< std::is_same_v<Msgs::Global::StatItem, std::remove_cvref_t<T>> >> {
		template<bool needReserve = true>
		static inline void Write(Data& d, T const& in) {
			d.Write<needReserve>(in.type, in.value);
		}
		static inline int Read(Data_r& d, T& out) {
			return d.Read(out.type, out.value);
		}
	};

	template<typename T>
	struct DataFuncs<T, std::enable_if_t< std::is_same_v<Msgs::Global::StatPanelBase, std::remove_cvref_t<T>> >> {
		template<bool needReserve = true>
		static inline void Write(Data& d, T const& in) {
			d.Write<needReserve>(
				in.health,
				in.vitality,
				in.strength,
				in.agility,
				in.defense,
				in.wisdom,
				in.lucky
			);
		}
		static inline int Read(Data_r& d, T& out) {
			return d.Read(
				out.health,
				out.vitality,
				out.strength,
				out.agility,
				out.defense,
				out.wisdom,
				out.lucky
			);
		}
	};

	template<typename T>
	struct DataFuncs<T, std::enable_if_t< std::is_same_v<Msgs::Global::StatPanel, std::remove_cvref_t<T>> >> {
		template<bool needReserve = true>
		static inline void Write(Data& d, T const& in) {
			d.Write<needReserve>((Msgs::Global::StatPanelBase&)in);
			d.Write<needReserve>(
				in.life,
				in.lifeRegeneration,
				in.energy,
				in.energyRegeneration,
				in.damageScale,
				in.defenseScale,
				in.evasion,
				in.movementSpeed,
				in.experienceScale,
				in.criticalChance,
				in.criticalBonus
			);
		}
		static inline int Read(Data_r& d, T& out) {
			if (auto r = d.Read((Msgs::Global::StatPanelBase&)out); r) return r;
			return d.Read(
				out.life,
				out.lifeRegeneration,
				out.energy,
				out.energyRegeneration,
				out.damageScale,
				out.defenseScale,
				out.evasion,
				out.movementSpeed,
				out.experienceScale,
				out.criticalChance,
				out.criticalBonus
			);
		}
	};

	template<typename T>
	struct DataFuncs<T, std::enable_if_t< std::is_same_v<Msgs::Global::StatPanelExt, std::remove_cvref_t<T>> >> {
		template<bool needReserve = true>
		static inline void Write(Data& d, T const& in) {
			d.Write<needReserve>((Msgs::Global::StatPanel&)in);
			d.Write<needReserve>(
				in.attackSpeed1,
				in.attackSpeed2,
				in.attackSpeed3
			);
		}
		static inline int Read(Data_r& d, T& out) {
			if (auto r = d.Read((Msgs::Global::StatPanel&)out); r) return r;
			return d.Read(
				out.attackSpeed1,
				out.attackSpeed2,
				out.attackSpeed3
			);
		}
	};
}
