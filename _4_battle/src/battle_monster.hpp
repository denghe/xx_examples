#pragma once

namespace Battle {

	inline void Monster::Init(Scene* scene_, XY const& pos_) {
		scene = scene_;
		auto radians = scene->rnd.Next<float>(-gPI, gPI);
		pos = pos_;
		movementDirection.x = std::cos(radians);
		movementDirection.y = std::sin(radians);
		auto n = ++scene->autoId;
		id = n;
		TryAddBaseActions();
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
			// ... more case
			}
		}

		// moved out of the screen? suicide
		if (pos.x < scene->screenAreaMin.x || pos.x > scene->screenAreaMax.x ||
			pos.y < scene->screenAreaMin.y || pos.y > scene->screenAreaMax.y) {
			return -1;
		}

		// try restore something
		TryAddBaseActions();

		// update space grid?
		if (posBak == pos) return 0;
		else return 1;
	}

};
