#pragma once

namespace Battle {

	struct Scene;
	struct Monster;

	struct Block {
		xx::FromTo<XY> aabb{};
		XY pos{}, halfSize{};
		xx::Math::BlockWayout wayout;
		bool isMapCorner{};

		void Init(float minX, float minY, float maxX, float maxY);
		void Init(XY const& centerPos, XY const& size);

		void FillWayout(Scene* scene);

		bool IntersectCircle(XY const& p, float radius);
		bool PushOut(Monster& m);

		void Draw();
	};

}
