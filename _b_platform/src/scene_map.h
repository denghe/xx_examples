#pragma once
#include "xx_map.h"

namespace Map {

	struct Scene;

	/*********************************************************************************************/

	struct MapItem {
		bool isBlock{};
		bool bottomNeighborIsBlock{};	// cache
		int16_t groupId{};
	};

	/*********************************************************************************************/

	struct Scene : xx::SceneBase {
		xx::Map2d<MapItem> map;

		XYi beginPos{}, endPos{};
		void InitMap(std::u32string_view mapText);
		void DumpMap();

		void Init() override;
		void Update() override;
		void Draw() override;
	};

}
