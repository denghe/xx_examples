#pragma once

namespace IntVersion2 {

	struct Scene;

	struct Item {
		static constexpr XYi cResSize{ 64, 64 };

		Scene* scene{};
		XYi pos{}, size{};							// pos: left-top for store & calc		size.y == 1: platform
		XYp _pos{};									// for update & offset change
		xx::RGBA8 color{ xx::RGBA8_White };
		xx::Math::BlockWayout blockWayout{};
		xx::Listi32<xx::Weak<Item>> children;

		void AssignChildrenPosOffset(XYp const& offset);
		void Init(Scene* scene_, XYi const& pos_, XYi const& size_);
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
		int32_t lastXMoveDirection{};		// -1: left  0: stoped  1: right
		int32_t fallingFrameCount{};		// for coyote time
		int32_t bigJumpFrameCount{};
		bool lastJumpPressed{};
		bool jumping{};
		bool doubleJumped{};
		bool longJumpStoped{};

		Character& Init(Scene* scene_, XYi const& pos_ = {}, XYi const& size_ = cResSize);
		void Update();

		bool HasCross(XYi const& newPos_) const;
	};

	enum class PushOutWays : uint32_t {
		Unknown = 0, Up = 1, Right = 2, Down = 4, Left = 8
	};

	struct Block : Item {
		Block& Init(Scene* scene_, XYi const& pos_ = {}, XYi const& size_ = cResSize, xx::Math::BlockWayout blockWayout_ = {});
		bool IsCross(XYi const& cPos, XYi const& cSize) const;
		bool IsCross(Character const& c) const;
		std::tuple<XYi, PushOutWays, bool> PushOut(Character const& c) const;	// bool: is wayout
		void Update();
	};

	struct Platform : Item {
		int32_t lineNumber{};
		FX64 xOriginal{}, xOffset{};
		Platform& Init(Scene* scene_, XYi const& pos_ = {}, int32_t len_ = cResSize.x);
		void Update();			// todo: sync children
	};

	struct Scene : xx::SceneBase {
		xx::Shared<Character> character;
		xx::Listi32<Block> blocks;
		xx::Listi32<Platform> platforms;	// todo: space index

		void Init() override;
		void Update() override;
		void Draw() override;
	};

}
