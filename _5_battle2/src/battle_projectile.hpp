#pragma once

namespace Battle {

	XX_INLINE bool Projectile::IsBlockIntersect() {
		auto& sg = gScene->blocks;
		xx::FromTo<xx::XY> aabb{ pos - radius, pos + radius };	// pos to aabb
		if (!sg.TryLimitAABB(aabb)) {
			return true;	// bug?
		}
		sg.ClearResults();
		sg.ForeachAABB(aabb);	// search
		for (auto b : sg.results) {
			if (b->IntersectCircle(pos, radius)) return true;
		}
		if (pos.IsOutOfEdge(gLooper.mapSize)) {
			return true;	// bug?
		}
		return false;
	}

};
