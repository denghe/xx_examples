#pragma once

namespace Battle {

	inline Item Item::Generate(ItemTypes type) {
		Item r;
		r.type = type;
		r.quantity = 1;

		switch (type) {
		case ItemTypes::Potion1:
			r.qualitity = ItemQualities::Normal;
			r.equipmentLocation = ItemEquipmentLocations::Bag;
			r.quantityLimit = 99;
			// Skill_HealthAdd(SkillTypes type, float castSpeed, float percentage);
			r.skill.Emplace<Skill_HealthAdd>(SkillTypes::HealthAdd, 5.f, 0.3f);
			break;

		case ItemTypes::Staff1:
			r.qualitity = ItemQualities::Normal;
			r.equipmentLocation = ItemEquipmentLocations::Weapon;
			r.quantityLimit = 0;
			// Skill_Blade(SkillTypes type, float castSpeed, float distance, float radius, int32_t damage);
			r.skill.Emplace<Skill_Blade>(SkillTypes::Blade, 1.0f,20.f, 32.f, 5);
			break;

		case ItemTypes::Sword1:
			r.qualitity = ItemQualities::Normal;
			r.equipmentLocation = ItemEquipmentLocations::Weapon;
			r.quantityLimit = 0;
			// Skill_FireBall(SkillTypes type, float castSpeed, float distance, float radius, float speed, float durationSeconds, int32_t damage);
			r.skill.Emplace<Skill_FireBall>(SkillTypes::FireBall, 0.5f, 20.f, 16.f, 300.f, 3.f, 10);
			break;

			// ...
		}
		return r;
	}

};
