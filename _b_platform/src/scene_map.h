#pragma once
#include "xx_map.h"

namespace Map {

	struct Scene : xx::SceneBase {
		xx::Map map;
		xx::Listi32<xx::MapCellGroup> spaceGroups;
		xx::AStarGrid asg;

		XYi beginPos{}, endPos{};
		void DumpMap();
		void InitSpaceGroups();
		void InitAStarGrid();
		void FillNavTips();

		void Init() override;
		void Update() override;
		void Draw() override;
	};

}
