#pragma once

namespace Battle {

	XX_INLINE Skill::Skill(SkillTypes type, float castSpeed)
		: type(type)
		, castSpeed(castSpeed) {}

	XX_INLINE Skill_HealthAdd::Skill_HealthAdd(SkillTypes type, float castSpeed, float percentage) 
		: Skill(type, castSpeed)
		, percentage(percentage) {}

	inline bool Skill_HealthAdd::Cast(Monster* caster, Monster* target) {
		return false;
	}

	XX_INLINE Skill_Blade::Skill_Blade(SkillTypes type, float castSpeed, float distance, float radius, int32_t damage)
		: Skill(type, castSpeed)
		, distance(distance)
		, radius(radius)
		, damage(damage) {}

	inline bool Skill_Blade::Cast(Monster* caster, Monster* target) {
		return false;
	}

	XX_INLINE Skill_FireBall::Skill_FireBall(SkillTypes type, float castSpeed, float distance, float radius, float speed, float durationSeconds, int32_t damage)
		: Skill(type, castSpeed)
		, distance(distance)
		, radius(radius)
		, speed(speed)
		, durationSeconds(durationSeconds)
		, damage(damage) {}

	inline bool Skill_FireBall::Cast(Monster* caster, Monster* target) {
		return false;
	}

};
