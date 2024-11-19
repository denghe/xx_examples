#pragma once

namespace Msgs {
	namespace Global {

		inline void Config_Monster::WriteTo(xx::Data& d) const {
			d.Write(
				initHealth,
				initVitality,
				initStrength,
				initAgility,
				initDefense,
				initWisdom,
				initLucky,

				levelToHealthRatio,
				levelToVitalityRatio,
				levelToStrengthRatio,
				levelToAgilityRatio,
				levelToDefenseRatio,
				levelToWisdomRatio,
				levelToLuckyRatio,

				healthToLifeRatio,
				healthToEnergyRatio,
				vitalityToLifeRegenerationRatio,
				vitalityToEnergyRegenerationRatio,
				strengthToDamageScaleRatio,
				defenseFactor,
				evasionFactor,
				baseMovementSpeed,
				agilityToMovementSpeedRatio,
				wisdomToExperienceScaleRatio,
				luckyToCritialChanceScaleRatio,
				luckyToCritialBonusScaleRatio
			);
		}

		inline int32_t Config_Monster::ReadFrom(xx::Data_r& dr) {
			return dr.Read(
				initHealth,
				initVitality,
				initStrength,
				initAgility,
				initDefense,
				initWisdom,
				initLucky,

				levelToHealthRatio,
				levelToVitalityRatio,
				levelToStrengthRatio,
				levelToAgilityRatio,
				levelToDefenseRatio,
				levelToWisdomRatio,
				levelToLuckyRatio,

				healthToLifeRatio,
				healthToEnergyRatio,
				vitalityToLifeRegenerationRatio,
				vitalityToEnergyRegenerationRatio,
				strengthToDamageScaleRatio,
				defenseFactor,
				evasionFactor,
				baseMovementSpeed,
				agilityToMovementSpeedRatio,
				wisdomToExperienceScaleRatio,
				luckyToCritialChanceScaleRatio,
				luckyToCritialBonusScaleRatio
			);
		}

	}
}
