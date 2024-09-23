#pragma once

namespace Battle {

	inline int32_t Monster::Update() {
		auto posBak = pos;

		// execute all actions
		for (int32_t i = actionsLen - 1; i >= 0; --i) {
			auto& b = actions[i];
			switch (b.type) {
			case Action_Stun::cType: Case_((Action_Stun&)b); break;
			case Action_Search::cType: Case_((Action_Search&)b); break;
			case Action_MoveToItem::cType: Case_((Action_MoveToItem&)b); break;
			case Action_MoveToTarget::cType: Case_((Action_MoveToTarget&)b); break;
			case Action_HitTarget::cType: Case_((Action_HitTarget&)b); break;
			case Action_SetColor::cType: Case_((Action_SetColor&)b); break;
			// ...
			}
		}

		// suicide check
		if (isDead) return -1;

		// try restore something
		TryRestoreBornAbility();

		// update space grid?
		if (posBak == pos) return 0;
		else return 1;
	}

};
