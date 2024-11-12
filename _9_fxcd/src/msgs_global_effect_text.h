#pragma once

namespace Msgs {
	namespace Global {

		struct EffectText {
			constexpr static float cCharPixelWidth{ 9.f };
			constexpr static float cScale{ 3.f };

			constexpr static int64_t cMoveDurationSecondsFrames{ int64_t(0.5 * Looper::fps) };
			constexpr static float cMoveSpeedMin{ 20 / gLooper.fps };
			constexpr static float cMoveSpeedMax{ 50 / gLooper.fps };

			constexpr static float cFadeOutDurationSeconds{ 0.2f };
			constexpr static float cFadeOutStep = 1.f / (cFadeOutDurationSeconds / gLooper.frameDelay);

			xx::Weak<Scene> scene;
			std::array<char, 12> buf{};		// value to string cache
			int32_t len{};					// buf's len

			int32_t lineNumber{};
			xx::RGBA8 color{};
			XY pos{}, inc{};
			int64_t timeout{};
			float alpha{};

			void Init(Scene* scene_, XY const& pos_, XY const& direction_, xx::RGBA8 color_, int32_t value_);

			int32_t UpdateCore();
			bool Update();

			void Draw();
		};

	}
}

namespace xx {
	template<>
	struct IsPod<Msgs::Global::EffectText, void> : std::true_type {};

	template<typename T>
	struct DataFuncs<T, std::enable_if_t< std::is_base_of_v<Msgs::Global::EffectText, T> >> {
		template<bool needReserve = true>
		static inline void Write(Data& d, T const& in) {
			xx::BufLenRef blr{ in.buf.data(), &in.len };
			d.Write<needReserve>(in.scene, blr, in.lineNumber, (float&)in.color, in.pos, in.inc, in.timeout, in.alpha);
		}
		static inline int Read(Data_r& d, T& out) {
			xx::BufLenRef blr{ out.buf.data(), &out.len };
			return d.Read(out.scene, blr , out.lineNumber, (float&)out.color, out.pos, out.inc, out.timeout, out.alpha);
		}
	};
}
