#pragma once

struct Scene;

// logic item base class
struct Item {
	Scene* owner{};
};

// player avatar
struct Character : Item {
	static constexpr float halfWidth{ 32.f };
	static_assert(Cfg::fps == 60.f || Cfg::fps == 120.f);
	static constexpr float cGravity{ Cfg::fps == 120.f ? 0.4 : 1.2012 };
	static constexpr float cYSpeedInit{ -650 / Cfg::fps };
	static constexpr float cYSpeedMax{ 1000 / Cfg::fps };
	static constexpr float cXSpeed{ 500 / Cfg::fps };
	static constexpr float cCoyoteTimespan{ 0.1f };
	static constexpr int32_t cCoyoteNumFrames{ int32_t(cCoyoteTimespan / Cfg::frameDelay) };
	static constexpr float cBigJumpTimespan{ 0.2f };
	static constexpr int32_t cBigJumpNumFrames{ int32_t(cBigJumpTimespan / Cfg::frameDelay) };
	static constexpr bool cEnableStrictJumpMode{ true };

	XY pos{};
	float lastY{}, ySpeed{};
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
	xx::FromTo<XY> aabb{};
	void Draw();
};

// 1 way ( up )
struct Platform {
	xx::FromTo<float> x{};
	float y{};
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
