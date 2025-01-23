#pragma once
#include "xx_space_box.h"

namespace CollisionDetection2 {
	struct Scene;

	struct Item {
		static constexpr XYi cResSize{ 64, 64 };

		Scene* scene{};
		XYi pos{}, size{};							// pos: left-top
		xx::RGBA8 color{ xx::RGBA8_White };
		xx::Math::BlockWayout wayout{};

		void Init(Scene* scene_, XYi const& pos_, XYi const& size_);
		void Draw();
	};

	struct Character : Item {
		XYi mouseOffset{}, lastPos{};
		bool dragging{}, lastMBState{};

		Character& Init(Scene* scene_, XYi const& pos_ = {}, XYi const& size_ = cResSize);
		void Update();
		void PosAreaLimit();
		void HandleCollision();
	};

	enum class PushOutWays : uint32_t {
		Unknown = 0, Up = 1, Right = 2, Down = 4, Left = 8
	};

	struct Block : Item {
		//Block* prev{}, * next{};						// for space index
		int32_t indexAtItems{ -1 }, indexAtCells{ -1 };	// for space index

		Block& Init(Scene* scene_, XYi const& pos_ = {}, XYi const& size_ = cResSize);
		bool IsCross(XYi const& cPos, XYi const& cSize) const;
		void FillWayout();
		std::pair<XYi, PushOutWays> PushOut(XYi const& cPos, XYi const& cSize) const;
	};

	struct Scene : xx::SceneBase {
		xx::Shared<Character> character;
		xx::SpaceIndexBox<Block, false> blocks;

		void Init() override;
		void Update() override;
		void Draw() override;
	};

}
