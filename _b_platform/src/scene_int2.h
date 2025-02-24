#pragma once

namespace IntVersion2 {

	struct Scene;
	struct Platform;

	struct Item {
		static constexpr XYi cResSize{ 64, 64 };

		Scene* scene{};
		XYi pos{}, size{};							// pos: left-top for store & calc		size.y == 1: platform
		XYp _pos{};									// for update & offset change
		xx::RGBA8 color{ xx::RGBA8_White };
		xx::Math::BlockWayout wayout{};

		virtual ~Item();
		void Init(Scene* scene_, XYi const& pos_, XYi const& size_, xx::RGBA8 color_);
		bool IsCross(XYi const& cPos, XYi const& cSize) const;
		int32_t CalcCrossLenX(int32_t cPosX, int32_t cSizeX) const;
		virtual bool Update();
		void Draw();
	};

	struct Character : Item {
		static_assert(Cfg::fps == 30.f || Cfg::fps == 60.f || Cfg::fps == 120.f);
		static constexpr FX64 cGravity{ Cfg::fps == 120.f ? 0.4 : Cfg::fps == 60.f ? 1.2012 : 3.4205 };
		static constexpr FX64 cYSpeedInit{ -650 / Cfg::fps };
		static constexpr FX64 cYSpeedMax{ 1000 / Cfg::fps };
		static constexpr FX64 cXSpeed{ 400 / Cfg::fps };
		static constexpr FX64 cDownJumpYOffset{ 1 };
		static constexpr int32_t cCoyoteNumFrames{ int32_t(0.1f / Cfg::frameDelay) };
		static constexpr int32_t cBigJumpNumFrames{ int32_t(0.2f / Cfg::frameDelay) };
		static constexpr bool cEnableStrictJumpMode{ true };

		FX64 ySpeed{};
		xx::Weak<Platform> attachedPlatform;
		xx::Weak<Platform> needAttachPlatform;
		int32_t needAttachPlatformIntersectLength{};
		int32_t attachedPlatformIndex{ -1 };
		int32_t lastXMoveDirection{};		// -1: left  0: stoped  1: right
		int32_t fallingFrameCount{};		// for coyote time
		int32_t bigJumpFrameCount{};
		bool lastJumpPressed{};
		bool jumping{};
		bool doubleJumped{};
		bool longJumpStoped{};

		Character& Init(Scene* scene_, XYi const& pos_ = {}, XYi const& size_ = cResSize);
		bool Update() override;
		void AttachPlatform(xx::Weak<Platform> platform);
	};

	enum class PushOutWays : uint32_t {
		Unknown = 0, Up = 1, Right = 2, Down = 4, Left = 8
	};

	struct Block : Item {
		//Block* prev{}, * next{};						// for space index
		int32_t indexAtItems{ -1 }, indexAtCells{ -1 };	// for space index

		xx::Shared<Block> Init(Scene* scene_, XYi const& pos_ = {}, XYi const& size_ = cResSize);
		void FillWayout();
		std::pair<XYi, PushOutWays> PushOut(XYi const& cPos, XYi const& cSize) const;
	};

	struct Platform : Item {
		xx::Listi32<xx::Weak<Character>> attachedCharacters;
		Platform& Init(Scene* scene_, XYi const& pos_, int32_t len_);
		void AssignOffset(XYp const& offset);
	};

	struct Platform_Slide : Platform {
		int32_t lineNumber{}, idleFrames{}, i{}, moveFrames{};
		XYi posFrom{}, posTo{};
		XYp posInc{};
		Platform_Slide& Init(Scene* scene_, XYi const& posFrom_, XYi const& posTo_
			, int32_t len_, int32_t moveDurationMS_, int32_t idleDurationMS_);
		bool Update() override;
	};

	struct Platform_Swing : Platform {
		int32_t lineNumber{}, idleFrames{}, i{}, moveFrames{};
		XYi posFrom{}, posTo{};
		XYp centerPos{};
		FX64 angle{}, angleSpeed{};
		Platform_Swing& Init(Scene* scene_, XYi const& posFrom_, XYi const& posTo_
		, int32_t len_, int32_t moveDurationMS, int32_t idleDurationMS);
		bool Update() override;
	};

	struct BornPlace : Item {
		bool touched{};
		BornPlace& Init(Scene* scene_, XYi const& pos_);
		bool Update() override;
	};

	struct Scene : xx::SceneBase {
		XYi bornPos{}, lastCharacterPos{};
		xx::Shared<Character> character;
		xx::SpaceIndexBox<Block, false> blocks;
		xx::Listi32<xx::Shared<Platform>> platforms;	// todo: space index
		xx::Listi32<BornPlace> bornPlaces;				// todo: space index

		void Init() override;
		void Update() override;
		void Draw() override;
	};

}
