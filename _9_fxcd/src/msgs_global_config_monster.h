#pragma once

namespace Msgs {
	namespace Global {

		struct Config_Monster : xx::SerdeBase {
			static constexpr uint16_t cTypeId{ 2 }; /////////////////////////////////////////////////////////////////////////////
			static constexpr uint16_t cParentTypeId{ xx::SerdeBase::cTypeId };
			/* S */ void WriteTo(xx::Data& d) const override;
			/* C */ int32_t ReadFrom(xx::Data_r& dr) override;

			Stat_t initHealth{ 10 };
			Stat_t initVitality{ 10 };
			Stat_t initStrength{ 10 };
			Stat_t initAgility{ 10 };
			Stat_t initDefense{ 10 };
			Stat_t initWisdom{ 10 };
			Stat_t initLucky{ 10 };

			Stat_t levelToHealthRatio{ 2 };
			Stat_t levelToVitalityRatio{ 2 };
			Stat_t levelToStrengthRatio{ 2 };
			Stat_t levelToAgilityRatio{ 2 };
			Stat_t levelToDefenseRatio{ 2 };
			Stat_t levelToWisdomRatio{ 2 };
			Stat_t levelToLuckyRatio{ 2 };

			Stat_t healthToLifeRatio{ 2 };
			Stat_t healthToEnergyRatio{ 5 };
			Stat_t vitalityToLifeRegenerationRatio{ FX64{1} / Scene::fps };
			Stat_t vitalityToEnergyRegenerationRatio{ FX64{2} / Scene::fps };
			Stat_t strengthToDamageScaleRatio{ 0.1 };
			Stat_t defenseFactor{ 100 };
			Stat_t evasionFactor{ 100 };
			Stat_t baseMovementSpeed{ 300 };
			Stat_t agilityToMovementSpeedRatio{ 0.1 };
			Stat_t wisdomToExperienceScaleRatio{ 0.1 };
			Stat_t luckyToCritialChanceScaleRatio{ 0.01 };
			Stat_t luckyToCritialBonusScaleRatio{ 0.1 };
		};

	}
}
