#pragma once
#include "pch.h"
#include "res_tp_frames.h"

namespace Battle {
	struct Scene;
}

struct Looper : xx::Engine<Looper>, xx::GDesign<1920, 1080, 60> {
	static constexpr float maxItemSize{ 64 };
	static constexpr float maxItemSize_2{ maxItemSize / 2 };

	static constexpr int32_t physCellSize{ (int32_t)maxItemSize };
	static constexpr int32_t physNumRows{ int32_t(height * 10 / physCellSize) };
	static constexpr int32_t physNumCols{ int32_t(width * 10 / physCellSize) };

	static constexpr XY mapSize{ float(physNumCols * physCellSize), float(physNumRows * physCellSize) };
	static constexpr XY mapSize_2{ mapSize.x / 2, mapSize.y / 2 };
	static constexpr float cameraMinScale{ width / mapSize.x };

	static constexpr XY mapEdgeMin{ maxItemSize * 2, maxItemSize * 2 };
	static constexpr XY mapEdgeMax{ mapSize - mapEdgeMin };

	static constexpr XY screenSafeEdgeMin{ width_2 - maxItemSize_2, height_2 - maxItemSize_2 };
	static constexpr XY screenSafeEdgeMax{ width_2 + maxItemSize_2, height_2 + maxItemSize_2 };

	static constexpr xx::FromTo<float> mouseHitRadius{ 20, 500 };

	// all picture resource here
	ResTpFrames res;

	// button config
	xx::Scale9SpriteConfig s9cfg_btn;
	xx::Shared<xx::Node> ui;

	xx::Camera camera;
	xx::Shared<Battle::Scene> scene;
	int32_t updateSpeed{ 1 };

	// mouse states
	bool dragging{}, lastMBState{};
	XY lastMousePos{}, mouseOffset{};

	// true: loading finished
	bool ok{};

	xx::Task<> MainTask();		// loading logic
	void BeforeUpdate();
	void Update();			// fixed update
	void Draw();
};

extern Looper gLooper;
extern ResTpFrames& gRes;
inline Battle::Scene* gScene{};	// ref to Looper.scene for easy use
