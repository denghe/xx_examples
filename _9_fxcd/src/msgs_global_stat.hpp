#pragma once

namespace Msgs {
	namespace Global {

		XX_INLINE void StatPanelBase::Clear() {
			memset(this, 0, sizeof(StatPanelBase));
		}

		XX_INLINE Stat_t& StatPanelBase::At(StatTypes t) const {
			assert((uint32_t)t <= (uint32_t)StatTypes::__POINTS_END__);
			return ((Stat_t*)this)[(uint32_t)t];
		}

		XX_INLINE void StatPanel::Clear() {
			memset(this, 0, sizeof(StatPanel));
		}

		XX_INLINE Stat_t& StatPanel::At(StatTypes t) const {
			assert((uint32_t)t <= (uint32_t)StatTypes::__RESULTS_END__);
			return ((Stat_t*)this)[(uint32_t)t];
		}

		XX_INLINE void StatPanel::CalculatePoints(Config_Monster* cfg, Stat_t level) {
			Clear();
			// calculate points by level
			health = cfg->initHealth + (level - 1) * cfg->levelToHealthRatio;
			vitality = cfg->initVitality + (level - 1) * cfg->levelToVitalityRatio;
			strength = cfg->initStrength + (level - 1) * cfg->levelToStrengthRatio;
			agility = cfg->initAgility + (level - 1) * cfg->levelToAgilityRatio;
			defense = cfg->initDefense + (level - 1) * cfg->levelToDefenseRatio;
			wisdom = cfg->initWisdom + (level - 1) * cfg->levelToWisdomRatio;
			lucky = cfg->initLucky + (level - 1) * cfg->levelToLuckyRatio;
		}

		template<typename Container>
		XX_INLINE void StatPanel::CalculateStatList(Container const& stats) {
			if (auto c = stats.Count(); c > 0) {
				for (int32_t i = 0; i < c; ++i) {
					auto& s = stats[i];
					At(s.type) += s.value;
				}
			}
		}

		XX_INLINE void StatPanel::CalculateResults(Config_Monster* cfg) {
			// calculate final results
			life += health * cfg->healthToLifeRatio;
			energy += health * cfg->healthToEnergyRatio;
			lifeRegeneration += vitality * cfg->vitalityToLifeRegenerationRatio;
			energyRegeneration += vitality * cfg->vitalityToEnergyRegenerationRatio;
			damageScale += strength * cfg->strengthToDamageScaleRatio;
			defenseScale += cfg->defenseFactor / (cfg->defenseFactor + defense);
			evasion += cfg->evasionFactor / (cfg->evasionFactor + agility);
			movementSpeed += cfg->baseMovementSpeed + agility * cfg->agilityToMovementSpeedRatio;
			experienceScale += wisdom * cfg->wisdomToExperienceScaleRatio;
			criticalChance += lucky * cfg->luckyToCritialChanceScaleRatio;
			criticalBonus += lucky * cfg->luckyToCritialBonusScaleRatio;
			// handle value range
			if (criticalChance > 1) criticalBonus += criticalChance - 1;
			if (movementSpeed >= 1000) movementSpeed = 1000;
			if (defenseScale >= 1) defenseScale = 0.99999;
			if (evasion >= 1) evasion = 0.99999;
		}

		XX_INLINE void StatPanelExt::Clear() {
			memset(this, 0, sizeof(StatPanelExt));
		}

		XX_INLINE Stat_t& StatPanelExt::At(StatTypes t) const {
			assert((uint32_t)t <= (uint32_t)StatTypes::__MAX_VALUE__);
			return ((Stat_t*)this)[(uint32_t)t];
		}

	}
}
