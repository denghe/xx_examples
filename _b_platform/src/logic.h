#pragma once

struct Item {
	static constexpr float cGravity{ 2 };
	static constexpr float cBaseVelocity{ 1 };
	static constexpr float cMaxVelocity{ 5 };

	XY pos{}, size{1,1};
	float ySpeed{};
	// todo: more stat members

	void Update();
	void Draw();
};

struct Character : Item {
	xx::KeyboardKeys lastKey{};
	float jumpAccel{};
	bool jumping{};
	// todo: double jump support

	void Update();
	void Init();
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

struct Logic {
	xx::Shared<Character> character;
	xx::Listi32<Block> blocks;
	xx::Listi32<Platform> platforms;

	void Init();
	void Update();
	void Draw();
};
