#pragma once
#include "cfg.h"

struct EffectNumber  {
	constexpr static float cCharPixelWidth{ 8 };
	constexpr static float cScale{ 2.f };

	constexpr static float cMoveDurationSeconds{ 0.5f };
	constexpr static float cMoveSpeedMin { 20 / gCfg.fps };
	constexpr static float cMoveSpeedMax { 50 / gCfg.fps };

	constexpr static float cFadeOutDurationSeconds{ 0.2f };
	constexpr static float cFadeOutInc = 255.f / (cFadeOutDurationSeconds / gCfg.frameDelay);

	GLuint texId{};					// number texture's gl id
	std::array<char, 12> buf{};		// value to string cache
	int len{};						// buf's len

	int lineNumber{};
	xx::RGBA8 color{};
	xx::XY pos{}, inc{};
	float f{};

	void FillBuf(int i);
	void Init(xx::XY const& pos_, xx::XY const& vec_, xx::RGBA8 color_, int value_);
	int UpdateCore();
	bool Update();
	void Draw();
};

namespace xx {
	template<>
	struct IsPod<EffectNumber, void> : std::true_type {};
}

struct EffectNumberManager {
	xx::Queue<EffectNumber> ens;

	void Init(size_t cap);
	void Add(xx::XY const& pos_, xx::XY const& vec_, xx::RGBA8 color_, int value_);
	bool Update();
	void Draw();
};
