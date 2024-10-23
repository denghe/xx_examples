#pragma once
#include <looper.h>

// coroutine 2x slowly than switch/case goto  ( cpu: 9900k )

//#define UPDATE_USE_COROUTINE

struct BugBody {
	static constexpr int cTypeId{ 0 };

	static constexpr float cScale{ 1 };
	static constexpr float cRadius{ 16 };
	static constexpr XY cAnchor{ 0.5f, 0.5f };
	static constexpr float cSpeed{ 300.f / Cfg::fps };
	static constexpr float cDistance{ 10.f };
	static constexpr float cMinRadians{ xx::g2PI / 32.f };

	XY pos, tarPos;
	bool isTail{};
	float radians{};
	xx::SpaceGridWeak<BugBody> head, prev;

#ifdef UPDATE_USE_COROUTINE
#else
	int32_t lineNumber{};
#endif

	XY GenRndPos(float radius, float safeRadius);

	void Init(XY const& pos_, xx::SpaceGridWeak<BugBody> head_, xx::SpaceGridWeak<BugBody> prev_, bool isTail_ = false);

	bool Update();
#ifdef UPDATE_USE_COROUTINE
	Task<> UpdateTask = UpdateTask_();
	Task<> UpdateTask_();
#else
	int UpdateCore();
#endif

	void Draw();
};
