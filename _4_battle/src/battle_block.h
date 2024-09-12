#pragma once

namespace Battle {

	struct Block {
		xx::FromTo<XY> aabb{};
		void Init(xx::FromTo<XY> const& aabb_);
		void Init(float minX, float minY, float maxX, float maxY);
	};

}
