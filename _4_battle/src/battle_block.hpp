#pragma once

namespace Battle {

	inline void Block::Init(xx::FromTo<XY> const& aabb_) {
		aabb = aabb_;
	}

	inline void Block::Init(float minX, float minY, float maxX, float maxY) {
		aabb.from.x = minX;
		aabb.from.y = minY;
		aabb.to.x = maxX;
		aabb.to.y = maxY;
	}

};
