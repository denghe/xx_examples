#pragma once

namespace CollisionDetection {

	struct Scene;

	struct Item {
		static constexpr XYi cResSize{ 64, 64 };

		Scene* scene{};
		XYi pos{}, size{};		// pos: lef top pos
		xx::RGBA8 color{ xx::RGBA8_White };

		void Init(Scene* scene_, XYi const& pos_, XYi const& size_);
		void Draw();
	};

	struct Character : Item {
		XYi mouseOffset{};
		bool dragging{}, lastMBState{};

		Character& Init(Scene* scene_, XYi const& pos_ = {}, XYi const& size_ = cResSize);
		void Update();

		void HandleCollision();
		bool HasCross(XYi const& tarPos_) const;
	};

	struct Block : Item {
		Block& Init(Scene* scene_, XYi const& pos_ = {}, XYi const& size_ = cResSize);
		bool IsCross(XYi const& cPos, XYi const& cSize) const;
		bool IsCross(Character const& c) const;
		XYi PushOut(Character const& c) const;
	};

	struct Scene : xx::SceneBase {
		xx::Shared<Character> character;
		xx::Listi32<Block> blocks;

		void Init() override;
		void Update() override;
		void Draw() override;
	};

}
