#pragma once

namespace Msgs {
	namespace Global {

		struct Config_Monster {
			Stat_t initHealth{ 10 };
			Stat_t initVitality{ 10 };
			Stat_t initStrength{ 10 };
			Stat_t initDexterity{ 10 };
			Stat_t initDefense{ 10 };
			Stat_t initWisdom{ 10 };
			Stat_t initLucky{ 10 };

			Stat_t levelToHealthRatio{ 2 };
			Stat_t levelToVitalityRatio{ 2 };
			Stat_t levelToStrengthRatio{ 2 };
			Stat_t levelToDexterityRatio{ 2 };
			Stat_t levelToDefenseRatio{ 2 };
			Stat_t levelToWisdomRatio{ 2 };
			Stat_t levelToLuckyRatio{ 2 };

			Stat_t healthToLifeRatio{ 2 };
			Stat_t healthToEnergyRatio{ 5 };
			Stat_t vitalityToLifeRegenerationRatio{ 0.2 };
			Stat_t vitalityToEnergyRegenerationRatio{ 0.5 };
			Stat_t strengthToDamageScaleRatio{ 0.1 };
			Stat_t defenseFactor{ 100 };
			Stat_t evasionFactor{ 100 };
			Stat_t baseMovementSpeed{ 300 };
			Stat_t dexterityToMovementSpeedRatio{ 0.1 };
			Stat_t wisdomToExperienceScaleRatio{ 0.1 };
			Stat_t luckyToCritialChanceScaleRatio{ 0.01 };
			Stat_t luckyToCritialBonusScaleRatio{ 0.1 };
		};

	}
}

namespace xx {
	template<>
	struct IsPod<Msgs::Global::Config_Monster, void> : std::true_type {};

	template<typename T>
	struct DataFuncs<T, std::enable_if_t< std::is_base_of_v<Msgs::Global::Config_Monster, T> >> {
		template<bool needReserve = true>
		static inline void Write(Data& d, T const& in) {
			d.Write<needReserve>(
				in.initHealth,
				in.initVitality,
				in.initStrength,
				in.initDexterity,
				in.initDefense,
				in.initWisdom,
				in.initLucky,

				in.levelToHealthRatio,
				in.levelToVitalityRatio,
				in.levelToStrengthRatio,
				in.levelToDexterityRatio,
				in.levelToDefenseRatio,
				in.levelToWisdomRatio,
				in.levelToLuckyRatio,

				in.healthToLifeRatio,
				in.healthToEnergyRatio,
				in.vitalityToLifeRegenerationRatio,
				in.vitalityToEnergyRegenerationRatio,
				in.strengthToDamageScaleRatio,
				in.defenseFactor,
				in.evasionFactor,
				in.baseMovementSpeed,
				in.dexterityToMovementSpeedRatio,
				in.wisdomToExperienceScaleRatio,
				in.luckyToCritialChanceScaleRatio,
				in.luckyToCritialBonusScaleRatio
			);
		}
		static inline int Read(Data_r& d, T& out) {
			return d.Read(
				out.initHealth,
				out.initVitality,
				out.initStrength,
				out.initDexterity,
				out.initDefense,
				out.initWisdom,
				out.initLucky,

				out.levelToHealthRatio,
				out.levelToVitalityRatio,
				out.levelToStrengthRatio,
				out.levelToDexterityRatio,
				out.levelToDefenseRatio,
				out.levelToWisdomRatio,
				out.levelToLuckyRatio,

				out.healthToLifeRatio,
				out.healthToEnergyRatio,
				out.vitalityToLifeRegenerationRatio,
				out.vitalityToEnergyRegenerationRatio,
				out.strengthToDamageScaleRatio,
				out.defenseFactor,
				out.evasionFactor,
				out.baseMovementSpeed,
				out.dexterityToMovementSpeedRatio,
				out.wisdomToExperienceScaleRatio,
				out.luckyToCritialChanceScaleRatio,
				out.luckyToCritialBonusScaleRatio
			);
		}
	};
}
