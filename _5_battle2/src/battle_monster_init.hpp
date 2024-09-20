#pragma once

namespace Battle {

	inline void Monster::Init(XY const& pos_) {
		auto radians = gScene->rnd.Next<float>(-gPI, gPI);
		pos = pos_;
		radius = cRadius;
		scale = radius / cRadius;
		radians = 0;
		frame = gRes.monster_1;
		movementDirection.x = std::cos(radians);
		movementDirection.y = std::sin(radians);

		if (gScene->rnd.Next<bool>()) {
			auto& skill = skills.Emplace().Emplace<Item_Sword1>();
			skill->Init(this);
			attackMinDistance = 20;
			statInfoMax.health = 80;
			statInfoMax.mana = 0;
		} else {
			auto& skill = skills.Emplace().Emplace<Item_Staff1>();
			skill->Init(this);
			attackMinDistance = skill->cBulletSpeed * skill->cBulletLifeSeconds * 0.75f;
			statInfoMax.health = 20;
			statInfoMax.mana = 50;
		}
		statInfo.health = statInfoMax.health = gScene->rnd.Next<int32_t>(1, statInfoMax.health + 1);
		statInfo.mana = 50;
		// todo: more init

		TryRestoreBornAbility();
		BlocksLimit();
	}

};
