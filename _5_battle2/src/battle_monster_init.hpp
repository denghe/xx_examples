#pragma once

namespace Battle {

	inline void Monster::Init(XY const& pos_) {
		pos = pos_;
		radius = cRadius;
		scale = radius / cRadius;
		radians = 0;
		frame = gRes.monster_1;

#if 0
		if (gScene->rnd.Next<bool>()) {
			auto& skill = *skills.Emplace().Emplace<Item_Sword1>();
			skill.Init(this);
			statInfoMax.health = 100;
			statInfoMax.mana = 0;
		} else {
			auto& skill = *skills.Emplace().Emplace<Item_Staff1>();
			skill.Init(this);
			statInfoMax.health = 20;
			statInfoMax.mana = 50;
		}
#else
		statInfoMax.health = 20;
		statInfoMax.mana = 0;
#endif
		// todo: more init

		statInfo = statInfoMax;

		TryRestoreBornAbility();
		BlocksLimit();
	}

};
