#pragma once
#include "looper.h"

namespace Msgs {
	extern xx::SerdeInfo gSerdeInfo;
	void InitSerdeInfo();

	namespace Global {	// id = 1 ~ 99

		struct Player;
		struct Monster;
		struct Block;

		struct Scene : xx::SerdeBase {
			static constexpr uint16_t cTypeId{ 1 };
			static constexpr uint16_t cParentTypeId{ xx::SerdeBase::cTypeId };
			/* S */ void WriteTo(xx::Data& d) const override;
			/* C */ int32_t ReadFrom(xx::Data_r& dr) override;

			int64_t frameNumber{};
			xx::Rnd rnd;
			xx::Spacei32<Monster> monsterSpace;
			xx::Listi32<xx::Shared<Monster>> monsters;
			xx::Listi32<xx::Shared<Player>> players;
			xx::SpaceABi32<Block> blockSpace;
			xx::Listi32<xx::Shared<Block>> blocks;

			/* C */ xx::Ref<xx::GLTexture> tex;
			/* C */ xx::Ref<xx::Frame> frame;

			void Init();
			/* C */ void InitForDraw();	// todo: recursive call all childs
			void Update();
			/* C */ void Draw();
			xx::Shared<Player> const& RefPlayer(int32_t clientId);
			void RemovePlayer(int32_t clientId);
		};

		struct Player : xx::SerdeBase {
			static constexpr uint16_t cTypeId{ 2 };
			static constexpr uint16_t cParentTypeId{ xx::SerdeBase::cTypeId };
			/* S */ void WriteTo(xx::Data& d) const override;
			/* C */ int32_t ReadFrom(xx::Data_r& dr) override;

			xx::Weak<Scene> scene;
			int32_t clientId{};
			int32_t score{};

			/* S */ void Init(Scene* scene_, int32_t clientId_);	// auto add to scene.players
			void Update();
			/* C */ void Draw();
		};

		struct Monster : xx::SerdeBase, xx::Spacei32Item<Monster> {
			static constexpr uint16_t cTypeId{ 3 };
			static constexpr uint16_t cParentTypeId{ xx::SerdeBase::cTypeId };
			/* S */ void WriteTo(xx::Data& d) const override;
			/* C */ int32_t ReadFrom(xx::Data_r& dr) override;

			static constexpr int32_t c314159{ 314159 };
			static constexpr FX64 c1_100000{ FX64{1} / FX64{100000} };
			static constexpr FX64 c0_1{ 0.1 };
			static constexpr FX64 c0_01{ 0.01 };
			static constexpr FX64 c0_001{ 0.001 };

			static constexpr int32_t cNeighbourMaxCount{ 7 };
			static constexpr int32_t cTimeout{ (int32_t)gLooper.fps * 2 };

			static constexpr FX64 cResRadius{ 32 };
			static constexpr FX64 cFrameIndexStep{ 0.1 };
			static constexpr FX64 cFrameIndexMax{ ResTpFrames::_countof_monster_ };
			static constexpr FX64 cMovementSpeed{ 20 };
			static constexpr FX64 cMovementSpeed2{ cMovementSpeed * 2 };
			static constexpr FX64 cMovementSpeed3{ cMovementSpeed * 3 };
			static constexpr FX64 cMovementSpeedPow2{ cMovementSpeed * cMovementSpeed };

			xx::Weak<Scene> scene;
			xx::Weak<Player> owner;
			FX64 x{}, y{}, radius{}, radians{}, frameIndex{};
			FX64 incX{}, incY{};
			int64_t timeout{};
			bool runawayMode{};

			virtual ~Monster();
			Monster* Init(Scene* scene_, xx::Shared<Player> const& owner_, xx::XYi const& bornPos);
			virtual bool Update();	// true: kill
			/* C */ void Draw();
			bool FillCrossInc();
		};


		struct BlockWayout {
			uint8_t up : 1;			// 1
			uint8_t right : 1;		// 2
			uint8_t down : 1;		// 4
			uint8_t left : 1;		// 8
		};

		struct Block : xx::SerdeBase, xx::SpaceABi32Item<Block> {
			static constexpr uint16_t cTypeId{ 4 };
			static constexpr uint16_t cParentTypeId{ xx::SerdeBase::cTypeId };
			/* S */ void WriteTo(xx::Data& d) const override;
			/* C */ int32_t ReadFrom(xx::Data_r& dr) override;

			xx::Weak<Scene> scene;
			XYi pos{}, halfSize{};
			BlockWayout wayout;

			virtual ~Block();
			void Init(Scene* scene_, int32_t minX, int32_t minY, int32_t maxX, int32_t maxY);

			void FillWayout();

			bool IntersectCircle(XYi const& p, int32_t radius);
			void PushOut(Monster* m);

			void Draw();
		};
	}

	namespace C2S {	// id == 100 ~ 199

		// todo: list/array data length limit protect

		struct Join : xx::SerdeBase {
			static constexpr uint16_t cTypeId{ 101 };
			static constexpr uint16_t cParentTypeId{ xx::SerdeBase::cTypeId };
			/* S */ void WriteTo(xx::Data& d) const override;
			/* C */ int32_t ReadFrom(xx::Data_r& dr) override;
		};

		struct Summon : xx::SerdeBase {
			static constexpr uint16_t cTypeId{ 102 };
			static constexpr uint16_t cParentTypeId{ xx::SerdeBase::cTypeId };
			/* S */ void WriteTo(xx::Data& d) const override;
			/* C */ int32_t ReadFrom(xx::Data_r& dr) override;

			xx::XYi bornPos;
		};

	}

	namespace S2C {	// id == 200 ~ 299

		// 1 to 1
		struct Join_r : xx::SerdeBase {
			static constexpr uint16_t cTypeId{ 201 };
			static constexpr uint16_t cParentTypeId{ xx::SerdeBase::cTypeId };
			/* S */ void WriteTo(xx::Data& d) const override;
			/* C */ int32_t ReadFrom(xx::Data_r& dr) override;

			int32_t clientId{};
			xx::Shared<Global::Scene> scene;
		};

		// 1 to n ( after Join_r )
		struct PlayerJoin : xx::SerdeBase {
			static constexpr uint16_t cTypeId{ 202 };
			static constexpr uint16_t cParentTypeId{ xx::SerdeBase::cTypeId };
			/* S */ void WriteTo(xx::Data& d) const override;
			/* C */ int32_t ReadFrom(xx::Data_r& dr) override;

			int32_t clientId{};
			int64_t frameNumber{};
		};

		// 1 to n ( after player disconnect )
		struct PlayerLeave : xx::SerdeBase {
			static constexpr uint16_t cTypeId{ 203 };
			static constexpr uint16_t cParentTypeId{ xx::SerdeBase::cTypeId };
			/* S */ void WriteTo(xx::Data& d) const override;
			/* C */ int32_t ReadFrom(xx::Data_r& dr) override;

			int32_t clientId{};
			int64_t frameNumber{};
		};

		// 1 to n
		struct Summon : xx::SerdeBase {
			static constexpr uint16_t cTypeId{ 204 };
			static constexpr uint16_t cParentTypeId{ xx::SerdeBase::cTypeId };
			/* S */ void WriteTo(xx::Data& d) const override;
			/* C */ int32_t ReadFrom(xx::Data_r& dr) override;

			int32_t clientId{};
			int64_t frameNumber{};
			xx::XYi bornPos;
		};

	}
}
