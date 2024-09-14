#pragma once

namespace Battle {

	struct Scene;
	struct Monster;

	struct BlockWayout {
		uint8_t up : 1;			// 1
		uint8_t right : 1;		// 2
		uint8_t down : 1;		// 4
		uint8_t left : 1;		// 8
	};

	struct Block {
		xx::FromTo<XY> aabb{};
		XY pos{}, halfSize{};
		BlockWayout wayout;

		void Init(float minX, float minY, float maxX, float maxY);
		void Init(xx::FromTo<XY> const& aabb_);
		void Init(XY const& centerPos, XY const& size);

		void FillWayout(Scene* scene);

		bool IntersectCircle(XY const& p, float radius);
		void PushOut(Monster& m);
	};

}
