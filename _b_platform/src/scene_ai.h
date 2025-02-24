#pragma once

namespace AI {

	struct Scene;

	enum class PushOutWays : uint32_t {
		Unknown = 0, Up = 1, Right = 2, Down = 4, Left = 8
	};

	struct MapCfg {
		static constexpr XYi cellSize{ 64, 64 };
	};

	struct Item {
		Scene* scene{};
		XYi pos{};													// anchor point( bottom center )
		void Init(Scene* scene_, XYi const& pos_);
		virtual bool Update() { assert(false); return false; };		// return true: kill self
		virtual void Draw() { assert(false); };
		virtual ~Item() {};
	};

	struct Character : Item {
		static constexpr XYi cResSize{ 64, 64 };					// todo: generate from res
		static constexpr XY cAnchor{ 0.5, 0 };
		static constexpr float cScale{ 5 };
		static constexpr float cRadians{ 0 };
		static constexpr float cSpeed{ 400 / Cfg::fps };

		XYf _pos{};													// for update & draw

		Character& Init(Scene* scene_, XYi const& crIndex);			// crIndex: column row index in the map
		bool Update() override;
		void Draw() override;
	};

	struct Block : Item {
		static constexpr XYi cResSize{ 64, 64 };
		static constexpr XY cAnchor{ 0, 1 };
		static constexpr float cScale{ 5 };
		static constexpr float cRadians{ 0 };

		int32_t indexAtItems{ -1 }, indexAtCells{ -1 };	// for space index
		XYi size{};
		xx::RGBA8 color{ xx::RGBA8_White };
		xx::Math::BlockWayout wayout{};

		xx::Shared<Block> Init(Scene* scene_, XYi const& crIndex);
		bool IsCross(XYi const& cPos, XYi const& cSize) const;
		int32_t CalcCrossLenX(int32_t cPosX, int32_t cSizeX) const;
		void Draw();
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
