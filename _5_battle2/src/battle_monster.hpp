#pragma once

namespace Battle {

	inline void Monster::Init(XY const& pos_) {
		auto radians = gScene->rnd.Next<float>(-gPI, gPI);
		pos = pos_;
		movementDirection.x = std::cos(radians);
		movementDirection.y = std::sin(radians);
		auto n = ++gScene->autoId;
		id = n;

		// init stat
		statInfoMax.health = 20;
		statInfoMax.mana = 50;
		statInfo.health = statInfoMax.health = gScene->rnd.Next<int32_t>(1, statInfoMax.health + 1);
		statInfo.mana = 50;
		// todo: more init

		// add some skills
		skills.Emplace().Emplace<Item_Sword1>()->Init(this);

		TryRestoreBornAbility();
		BlocksLimit();
	}

	/*********************************************************************************************/

	inline int32_t Monster::Update() {
		auto posBak = pos;

		// execute all actions
		for (int32_t i = actionsLen - 1; i >= 0; --i) {
			auto& b = actions[i];
			switch (b.type) {
			case Action_Stun::cType: Case_((Action_Stun&)b); break;
			case Action_SearchTarget::cType: Case_((Action_SearchTarget&)b); break;
			case Action_MoveToTarget::cType: Case_((Action_MoveToTarget&)b); break;
			case Action_HitTarget::cType: Case_((Action_HitTarget&)b); break;
			case Action_SetColor::cType: Case_((Action_SetColor&)b); break;
			// ...
			}
		}

		// handle blocks
		if (BlocksLimit()) return -1;

		// try restore something
		TryRestoreBornAbility();

		// update space grid?
		if (posBak == pos) return 0;
		else return 1;
	}

};
