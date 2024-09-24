#pragma once

namespace Battle {

	struct Scene {
		xx::SpaceGridRingDiffuseData srdd;
		xx::SpaceGridAB<Block> blocks;
		xx::SpaceGridEx<Monster> monsters;

		void Init();
		int32_t Update();
		void Draw();

		void GenMonsters(XY const& pos = gLooper.mapSize_2, int32_t count = 1000);
	};

}
