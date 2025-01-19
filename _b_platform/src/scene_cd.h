#pragma once

namespace CollisionDetection {

	struct Scene;

	struct Item {
		static constexpr XYi cResSize{ 64, 64 };

		Scene* owner{};
		XYi pos{}, size{};
		xx::FromTo<XYi> aabb{};
		xx::RGBA8 color{ xx::RGBA8_White };

		void Init(Scene* owner_, XYi const& pos_, XYi const& size_);
		void SyncAABB();
		void Draw();
	};

	struct Character : Item {
		XYi mouseOffset{};
		bool dragging{}, lastMBState{};

		Character& Init(Scene* owner_, XYi const& pos_ = {}, XYi const& size_ = cResSize);
		void Update();

		void HandleCollision();
	};

	struct Block : Item {
		Block& Init(Scene* owner_, XYi const& pos_ = {}, XYi const& size_ = cResSize);
	};

	struct Scene : xx::SceneBase {
		xx::Shared<Character> character;
		xx::Listi32<Block> blocks;

		void Init() override;
		void Update() override;
		void Draw() override;
	};

}
