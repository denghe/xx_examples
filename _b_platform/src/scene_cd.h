#pragma once

namespace CollisionDetection {

	struct Scene;

	// logic item base class
	struct Item {
		Scene* owner{};
	};

	// player avatar
	struct Character : Item {
		static constexpr int32_t halfWidth{ 32 };

		XYi pos{};							// for check & draw

		Character& Init(Scene* owner_);
		void Update();
		void Draw();
	};

	// 4 way
	struct Block {
		xx::FromTo<XYi> aabb{};
		void Draw();
	};

	// scene
	struct Scene : xx::SceneBase {
		xx::Shared<Character> character;
		xx::Listi32<Block> blocks;

		void Init() override;
		void Update() override;
		void Draw() override;
	};

}
