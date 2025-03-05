#pragma once

namespace AI {

	struct Scene;

	enum class PushOutWays : uint32_t {
		Unknown = 0, Up = 1, Right = 2, Down = 4, Left = 8
	};

	/*********************************************************************************************/

	struct MapCfg {
		static constexpr XYi cellSize{ 64, 64 };
	};

	/*********************************************************************************************/

	struct Item {
		Scene* scene{};
		XYi pos{};													// anchor point( bottom center )
		void Init(Scene* scene_, XYi const& pos_);
		virtual bool Update() { assert(false); return false; };		// return true: kill self
		virtual void Draw() { assert(false); };
		virtual ~Item() {};
	};

	/*********************************************************************************************/

	enum class ActionTypes : int32_t {
		Move,
		Jump,
		Fall,
		//Attack,
		//Defend,
		//Heal,
		//Count,
		//// ...
		MaxValue
	};
	static_assert((int32_t)ActionTypes::MaxValue <= 64);	// uint64_t actionFlags limit 

	/*********************************************************************************************/
	// base data struct

	struct alignas(8) Action {
		union {
			std::array<uint64_t, 2> _;	// resize it if need more space
			struct {
				ActionTypes type;
				int32_t __;
			};
		};
	};

	template<typename A>
	constexpr bool ActionStructCheck = alignof(Action) == alignof(A) && sizeof(A) <= sizeof(Action);

	/*********************************************************************************************/
	// Actions

	struct alignas(8) Action_Move {
		static constexpr ActionTypes cType{ ActionTypes::Move };
		ActionTypes type;	// need fill by Init
		//int32_t timeoutFrameNumber;
		XYi tarCRIndex;
		// todo: more fields here
	};
	static_assert(ActionStructCheck<Action_Move>);

	struct alignas(8) Action_Jump {
		static constexpr ActionTypes cType{ ActionTypes::Jump };
		ActionTypes type;	// need fill by Init
		//int32_t timeoutFrameNumber;
		XYi tarCRIndex;
		// todo: more fields here
	};
	static_assert(ActionStructCheck<Action_Jump>);

	struct alignas(8) Action_Fall {
		static constexpr ActionTypes cType{ ActionTypes::Fall };
		ActionTypes type;	// need fill by Init
		//int32_t timeoutFrameNumber;
		XYi tarCRIndex;
		// todo: more fields here
	};
	static_assert(ActionStructCheck<Action_Fall>);

	/*********************************************************************************************/

	struct Character : Item {
		static constexpr XYi cResSize{ 64, 64 };					// todo: generate from res
		static constexpr XY cAnchor{ 0.5, 0 };
		static constexpr float cScale{ 1 };
		static constexpr float cRadians{ 0 };
		static constexpr float cSpeed{ 40 / Cfg::fps };

		xx::Queue<Action> actions;
		XYf _pos{};													// for update & draw
		int32_t moveLineNumber{};									// for move logic

		XYi CRIndexToPos(XYi const& crIndex) const;
		Character& Init(Scene* scene_, XYi const& crIndex);			// crIndex: column row index in the map
		bool Update() override;
		bool Move(XYi const& tarPos);
		void Draw() override;
	};

	/*********************************************************************************************/

	struct BlockGroup;
	struct Block : Item {
		static constexpr XYi cResSize{ 64, 64 };
		static constexpr XY cAnchor{ 0, 1 };
		static constexpr float cScale{ 5 };
		static constexpr float cRadians{ 0 };

		int32_t indexAtItems{ -1 }, indexAtCells{ -1 };	// for space index
		XYi size{};
		xx::RGBA8 color{ xx::RGBA8_White };
		xx::Math::BlockWayout wayout{};

		xx::Weak<BlockGroup> blockGroup;	// fill after Init

		xx::Shared<Block> Init(Scene* scene_, XYi const& crIndex);
		bool IsCross(XYi const& cPos, XYi const& cSize) const;
		int32_t CalcCrossLenX(int32_t cPosX, int32_t cSizeX) const;
		void Draw();
		void FillWayout();
		std::pair<XYi, PushOutWays> PushOut(XYi const& cPos, XYi const& cSize) const;
	};

	/*********************************************************************************************/

	struct BlockGroup {
		int32_t index{};
		xx::List<xx::Weak<Block>> blocks;
		xx::Listi32<int32_t> navigationTips;
	};

	/*********************************************************************************************/

	struct Scene : xx::SceneBase {
		xx::Shared<Character> character;
		xx::SpaceIndexBox<Block, false> blocks;
		xx::Listi32<xx::Shared<BlockGroup>> blockGroups;

		void Init() override;
		void Update() override;
		void Draw() override;
	};

}
