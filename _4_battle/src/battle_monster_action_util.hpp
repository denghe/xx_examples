#pragma once
#include "pch.h"
#include "battle.h"

namespace Battle {

	XX_INLINE bool Monster::ActionExists(ActionTypes bt) {
		return (actionFlags & (1llu << (int32_t)bt)) > 0;
	}

	XX_INLINE void Monster::ActionSetFlag(ActionTypes bt) {
		assert(!ActionExists(bt));
		actionFlags |= (1llu << (int32_t)bt);
	}

	XX_INLINE void Monster::ActionClearFlag(ActionTypes bt) {
		assert(ActionExists(bt));
		actionFlags &= ~(1llu << (int32_t)bt);
	}

	// return -1 mean not found
	XX_INLINE int32_t Monster::ActionFind(ActionTypes bt) {
		if (!ActionExists(bt)) return -1;
		for (int32_t i = actionsLen - 1; i >= 0; --i) {
			if (actions[i].type == bt) return i;
		}
		assert(false);
		return -1;
	}

	// call after ActionFind
	XX_INLINE void Monster::ActionRemove(ActionTypes bt, int32_t index) {
		assert(ActionExists(bt));
		assert(index != -1);
		assert(index < _countof(actions));
		assert(actions[index].type == bt);
		actionFlags &= ~(1llu << (int32_t)bt);
		actions[index] = actions[--actionsLen];
	}

	// return -1 mean not found
	XX_INLINE bool Monster::ActionRemove(ActionTypes bt) {
		if (!ActionExists(bt)) return false;
		for (int32_t index = actionsLen - 1; index >= 0; --index) {
			if (actions[index].type == bt) {
				ActionRemove(bt, index);
				return true;
			}
		}
		assert(false);
		return false;
	}

};
