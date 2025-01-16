#pragma once

namespace IntVersion {

	struct Scene;

	// logic item base class
	struct Item {
		Scene* owner{};
	};

	// player avatar
	struct Character : Item {
		static constexpr int32_t halfWidth{ 32 };
		static_assert(Cfg::fps == 30.f || Cfg::fps == 60.f || Cfg::fps == 120.f);
		static constexpr FX64 cGravity{ Cfg::fps == 120.f ? 0.4 : Cfg::fps == 60.f ? 1.2012 : 3.4205 };
		static constexpr FX64 cYSpeedInit{ -650 / Cfg::fps };
		static constexpr FX64 cYSpeedMax{ 1000 / Cfg::fps };
		static constexpr FX64 cXSpeed{ 400 / Cfg::fps };
		static constexpr FX64 cDownJumpYOffset{ 1 };
		static constexpr int32_t cCoyoteNumFrames{ int32_t(0.1f / Cfg::frameDelay) };
		static constexpr int32_t cBigJumpNumFrames{ int32_t(0.2f / Cfg::frameDelay) };
		static constexpr bool cEnableStrictJumpMode{ true };

		XYp _pos{};							// for update
		XYi pos{};							// for check & draw
		FX64 ySpeed{};
		int32_t lastXMoveDirection{};		// -1: left  0: stoped  1: right
		int32_t fallingFrameCount{};		// for coyote time
		int32_t bigJumpFrameCount{};
		bool lastJumpPressed{};
		bool jumping{};
		bool doubleJumped{};
		bool longJumpStoped{};

		Character& Init(Scene* owner_);
		void Update();
		void Draw();
	};

	// 4 way
	struct Block {
		xx::FromTo<XYi> aabb{};
		void Draw();
	};

	// 1 way ( up )
	struct Platform {
		xx::FromTo<int32_t> x{};
		int32_t y{};
		void Draw();
	};

	// todo: moving platform? trap?

	// scene
	struct Scene {
		xx::Shared<Character> character;
		xx::Listi32<Block> blocks;
		xx::Listi32<Platform> platforms;	// todo: space index

		void Init();
		void Update();
		void Draw();
	};

}
