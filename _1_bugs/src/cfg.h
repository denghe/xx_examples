#pragma once
#include "pch.h"

struct Cfg : GDesign<1280, 800, 60> {
	static constexpr float maxItemSize{ 32 };
	static constexpr float maxItemSize_2{ maxItemSize / 2 };

	static constexpr int32_t physCellSize{ 32 };	// need >= max item size
	static constexpr int32_t physNumRows{ 1024 };
	static constexpr int32_t physNumCols{ int32_t(physNumRows * w_h) };

	static constexpr XY mapSize{ float(physNumCols * physCellSize), float(physNumRows * physCellSize) };
	static constexpr XY mapSize_2{ mapSize.x / 2, mapSize.y / 2 };

	static constexpr XY mapEdgeMin{ maxItemSize * 5, maxItemSize * 5 };
	static constexpr XY mapEdgeMax{ mapSize - mapEdgeMin };

	static constexpr FromTo<float> mouseHitRadius{ 20, 500 };
};
