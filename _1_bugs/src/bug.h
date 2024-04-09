#pragma once
#include <looper.h>

struct BugBody {
	static constexpr int cTypeId{ 0 };

	static constexpr float cScale{ 1 };
	static constexpr XY cAnchor{ 0.5f, 0.5f };
	static constexpr float cSpeed{ 300.f / gDesign.fps };
	static constexpr float cDistance{ 10.f };
	static constexpr float cMinRadians{ g2PI / 32.f };

	XY pos;
	XY tarPos;
	int32_t lineNumber{};
	bool isTail{};
	float radians{};
	SpaceWeak<BugBody> head, prev;

	XY GenRndPos(float radius, float safeRadius);

	void Init(XY const& pos_, SpaceWeak<BugBody> head_, SpaceWeak<BugBody> prev_, bool isTail_ = false);
	bool Update();
	int UpdateCore();
	void Draw();
};
