#pragma once
#include "xx_map.h"

namespace Map {

	struct Scene : xx::SceneBase {
		xx::Map map;
		XYi beginPos{}, endPos{};

		void Init() override;
		void Update() override;
		void Draw() override;
	};

}
