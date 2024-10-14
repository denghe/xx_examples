#pragma once

namespace Battle {

	XX_INLINE int32_t Monster::BlocksLimit() {
		auto& sg = gScene->blocks;
		xx::FromTo<xx::XY> aabb{ pos - cRadius, pos + cRadius };	// pos to aabb
		if (!sg.TryLimitAABB(aabb)) {
			return -1;	// bug?
		}
		sg.ClearResults();
		sg.ForeachAABB(aabb);	// search
		for (auto b : sg.results) {
			b->PushOut(*this);
		}
		if (pos.IsOutOfEdge(gLooper.mapSize)) {
			return -2;	// bug?
		}
		return 0;
	}

};
