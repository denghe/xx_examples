#pragma once

namespace Battle {

	inline void Monster::Init(XY const& pos_) {
		pos = pos_;
		if (pos.x < 0) pos.x = 0;
		else if (pos.x >= gLooper.mapSize.x) pos.x = gLooper.mapSize.x - 1;
		if (pos.y < 0) pos.y = 0;
		else if (pos.y >= gLooper.mapSize.y) pos.y = gLooper.mapSize.y - 1;
		radius = 32;
		scale = radius / cRadius;
		radians = 0;
		frame = gRes.monster_1;
		runawayMode = false;
		BlocksLimit();
	}

};
