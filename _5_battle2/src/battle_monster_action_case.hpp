#pragma once

namespace Battle {

	XX_INLINE void Monster::Case_(Action_Stun& o) {
		if (!ActionExists<Action_SetColor>()) {
			frame = gRes.monster_1;
		}
		// timeout
		if (gScene->time > o.timeout) {
			ActionRemove(o);	// suicide
		}
	}

	XX_INLINE void Monster::Case_(Action_Search& o) {
		if (!ActionExists<Action_SetColor>()) {
			frame = gRes.monster_2;
		}
		// delay
		if (gScene->time < o.timeout) return;
		// search. if no skill, search item on the ground
		if (skills.Empty()) {
			if (auto item = gScene->SearchItem(pos, o.searchRange)) {
				targetItem = xx::WeakFromThis(item);
				ActionRemove(o);	// suicide
				// next step
				Add_Action_MoveToItem(3);
				return;
			}
		} else {
			if (auto m = gScene->monsters.FindNearestByRange<true>(gScene->srdd, pos.x, pos.y, o.searchRange, this)) {
				targetMonster = m;
				ActionRemove(o);	// suicide
				// next step
				Add_Action_MoveToTarget(2, 10);
				return;
			}
		}
		// todo: random move ?
		ActionRemove(o);	// suicide
	}

	XX_INLINE void Monster::Case_(Action_MoveToItem& o) {
		assert(!skills.len);
		if (!ActionExists<Action_SetColor>()) {
			frame = gRes.monster_3;
		}
		// lost target? has been picked up?
		if (!targetItem || targetItem->owner) {
			ActionRemove(o);	// suicide
			return;
		}

		// compare distance
		auto t = targetItem.GetPointer();	// ref
		auto d = t->pos - pos;
		auto r = /*t.radius + */radius;
		auto mag2 = d.x * d.x + d.y * d.y;
		// reached
		if (mag2 <= r * r) {
			// pick it up			// todo: refine
			for (int32_t i = 0, e = gScene->items.len; i < e; ++i) {
				auto& item = gScene->items[i];
				if (item == t) {
					gScene->itemsSG.Remove(item);
					item->owner = this;
					skills.Add(item);
					gScene->items.SwapRemoveAt(i);
					break;
				}
			}
			ActionRemove(o);	// suicide
			// next step: search
		} else {
			auto mag = std::sqrt(mag2);
			if (mag > 0) {
				// move to
				pos += (d / mag) * o.movementSpeed;
				// block
				if (BlocksLimit()) {
					isDead = true;	// kill owner
				}
			}
		}
	}

	XX_INLINE void Monster::Case_(Action_MoveToTarget& o) {
		assert(skills.len);
		if (!ActionExists<Action_SetColor>()) {
			frame = gRes.monster_3;
		}
		// lost target? timeout?
		if (!targetMonster.Exists() || gScene->time > o.timeout) {
			ActionRemove(o);	// suicide
			return;
		}

		// compare distance
		auto& t = targetMonster();	// ref
		auto d = t.pos - pos;
		auto r = t.radius + radius + skills[0]->cCastMinDistance;
		auto mag2 = d.x * d.x + d.y * d.y;
		// reached
		if (mag2 <= r * r) {
			ActionRemove(o);	// suicide
			// next step
			Add_Action_HitTarget(0.5);		// todo: get args from cfg?
		} else {
			auto mag = std::sqrt(mag2);
			if (mag > 0) {
				// move to
				pos += (d / mag) * o.movementSpeed;
				// block
				if (BlocksLimit()) {
					isDead = true;	// kill owner
				}
			}
		}
	}

	XX_INLINE void Monster::Case_(Action_HitTarget& o) {
		if (!ActionExists<Action_SetColor>()) {
			frame = gRes.monster_3;
		}
		// lost target?
		if (!targetMonster.Exists()) {
			ActionRemove(o);	// suicide
			return;
		}
		
		// ensure distance
		auto& t = targetMonster();	// ref
		auto d = t.pos - pos;
		auto r = t.radius + radius + skills[0]->cCastMinDistance;
		auto mag2 = d.x * d.x + d.y * d.y;
		if (mag2 <= r * r) {	// reached
			// cast skills?
			for (auto& s : skills) {
				s->Cast(&t);		// todo: if not success( waiting cd ... ), suicide ?
			}
		} else {	// not reached: suicide this action, search again
			ActionRemove(o);
		}
	}

	XX_INLINE void Monster::Case_(Action_SetColor& o) {
		if (gScene->time > o.timeout) {
			ActionRemove(o);	// suicide
			return;
		}
		color = o.color;
		frame = gRes.monster_4;
	}
	// ...
};
