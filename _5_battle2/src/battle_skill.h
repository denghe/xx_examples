#pragma once

namespace Battle {

	enum class SkillTypes : uint32_t {
		Unknown,
		HealthAdd,
		Blade,
		FireBall,
		// ...
	};

	struct alignas(8) Skill {
		SkillTypes type;
		float castSpeed;

		Skill(SkillTypes type, float castSpeed);
		Skill(Skill const&) = delete;
		Skill& operator=(Skill const&) = delete;
		virtual bool Cast(Monster* caster, Monster* target) { return false; };
		virtual ~Skill() {};
	};

	struct Skill_HealthAdd : Skill {
		float percentage;

		Skill_HealthAdd(SkillTypes type, float castSpeed, float percentage);
		bool Cast(Monster* caster, Monster* target) override;
	};

	struct Skill_Blade : Skill {
		float distance;
		float radius;
		int32_t damage;

		Skill_Blade(SkillTypes type, float castSpeed, float distance, float radius, int32_t damage);
		bool Cast(Monster* caster, Monster* target) override;
	};

	struct Skill_FireBall : Skill {
		float distance;
		float radius;
		float speed;
		float durationSeconds;
		int32_t damage;

		Skill_FireBall(SkillTypes type, float castSpeed, float distance, float radius, float speed, float durationSeconds, int32_t damage);
		bool Cast(Monster* caster, Monster* target) override;
	};

	// ...

}
