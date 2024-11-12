#pragma once

namespace Battle {
	struct EffectText {
		constexpr static float cCharPixelWidth{ 9.f };
		constexpr static float cScale{ 3.f };

		constexpr static float cMoveDurationSeconds{ 0.5f };
		constexpr static float cMoveSpeedMin{ 20 / gLooper.fps };
		constexpr static float cMoveSpeedMax{ 50 / gLooper.fps };

		constexpr static float cFadeOutDurationSeconds{ 0.2f };
		constexpr static float cFadeOutStep = 1.f / (cFadeOutDurationSeconds / gLooper.frameDelay);

		std::array<char, 12> buf{};		// value to string cache
		int32_t len{};						// buf's len

		int32_t lineNumber{};
		xx::RGBA8 color{};
		XY pos{}, inc{};
		float timeout{};
		float alpha{};

		// pos: original position,  dist: determine move direction
		void Init(XY const& pos_, XY const& vec_, xx::RGBA8 color_, int32_t value_);

		int32_t UpdateCore();
		bool Update();

		void Draw();
	};
}

namespace xx {
	template<>
	struct IsPod<Battle::EffectText, void> : std::true_type {};
}

namespace Battle {
	struct EffectTextManager {
		xx::Queue<EffectText> ens;

		void Init(int32_t cap);
		void Add(XY const& pos_, XY const& vec_, xx::RGBA8 color_, int32_t value_);
		bool Update();
		void Draw();
	};
}
