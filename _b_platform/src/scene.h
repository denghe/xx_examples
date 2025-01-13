#pragma once

struct Scene;

// logic item base class
struct Item {
	Scene* owner{};
};

// player avatar
struct Character : Item {
	static constexpr float halfWidth{ 32.f };
	static constexpr float cMoveSpeed{ 3 };
	static constexpr float cGravity{ 1 };

	XY pos{};
	xx::KeyboardKeys lastKey{};
	float lastY{}, ySpeed{};
	bool jumping{};
	// todo: double jump support

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

// scene
struct Scene {
	xx::Shared<Character> character;
	xx::Listi32<Block> blocks;
	xx::Listi32<Platform> platforms;	// todo: space index

	void Init();
	void Update();
	void Draw();
};
