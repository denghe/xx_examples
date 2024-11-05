#pragma once
#include "looper.h"

namespace Msgs {
	extern xx::SerdeInfo gSerdeInfo;
	void InitSerdeInfo();

	namespace Global {	// id = 1 ~ 99

		struct Player;
		struct Monster;
		struct Block;
		struct Bullet_Base;

		struct Scene : xx::SerdeBase {
			static constexpr uint16_t cTypeId{ 1 };
			static constexpr uint16_t cParentTypeId{ xx::SerdeBase::cTypeId };
			/* S */ void WriteTo(xx::Data& d) const override;
			/* C */ int32_t ReadFrom(xx::Data_r& dr) override;

			// int max value == 0x7FFF FFFF. sqrt == 46340. / 96 ~= 482		/ 64 = 724
			static constexpr FX64 maxDistance{ 46340 };
			static constexpr int32_t numRows{ 23 };
			static constexpr int32_t numCols{ 35 };
			static constexpr int32_t cellSize{ 64 };
			static constexpr int32_t cellSize_2{ cellSize / 2 };
			static constexpr XYi mapSize{ numCols * cellSize, numRows * cellSize };
			static constexpr XYi mapSize_2{ mapSize / 2 };
			static constexpr XY mapSize_2f{ mapSize_2 };
			static constexpr XYp mapSize_2p{ mapSize_2 };
			static constexpr int32_t itemSize{ 64 };
			static constexpr XYi mapEdgeMin{ itemSize * 2 - 1 };
			static constexpr XYi mapEdgeMax{ mapSize - mapEdgeMin };
			static constexpr XYp mapSizep{ mapSize };

			static constexpr FX64 fps{ (int)Looper::fps };
			static constexpr FX64 fps60ratio{ fps / FX64{60} };
			static constexpr FX64 _1_fps60ratio{ FX64{1} / fps60ratio };

			int64_t frameNumber{};
			xx::Rnd rnd;
			xx::Spacei32<Monster> monsterSpace;
			xx::Listi32<xx::Shared<Monster>> monsters;
			xx::Listi32<xx::Shared<Player>> players;
			xx::SpaceABi32<Block> blockSpace;
			xx::Listi32<xx::Shared<Block>> blocks;
			xx::Listi32<xx::Shared<Bullet_Base>> bullets;

			void Init();
			/* C */ void InitForDraw();	// todo: recursive call all childs
			void Update();
			/* C */ void Draw();
			xx::Shared<Player> const& RefPlayer(int32_t clientId);
			void RemovePlayer(int32_t clientId);
			void PosLimitByMapSize(XYp& p);
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

			static constexpr int32_t cNeighbourMaxCount{ 7 };
			static constexpr int32_t cTimeout{ (int32_t)gLooper.fps >> 1 };

			static constexpr FX64 cResRadius{ 32 };
			static constexpr FX64 cFrameIndexStep{ FX64{0.1} / Scene::fps60ratio };
			static constexpr FX64 cFrameIndexMax{ ResTpFrames::_countof_monster_ };
			static constexpr FX64 cMovementSpeed{ FX64{5} / Scene::fps60ratio };

			xx::Weak<Scene> scene;
			xx::Weak<Player> owner;
			XYp pos, tarPos;
			FX64 radius{}, radians{}, frameIndex{};
			/* T */ XYp inc{}, newPos{};

			virtual ~Monster();
			Monster* Init(Scene* scene_, xx::Shared<Player> const& owner_, xx::XYi const& bornPos);
			virtual int32_t Update1();	// non zero: kill
			virtual int32_t Update2();	// non zero: kill
			/* C */ void Draw();
			bool FillCrossInc(XYp const& pos_);
			int32_t BlocksLimit(XYp& pos_);
		};

		struct Block : xx::SerdeBase, xx::SpaceABi32Item<Block> {
			static constexpr uint16_t cTypeId{ 4 };
			static constexpr uint16_t cParentTypeId{ xx::SerdeBase::cTypeId };
			/* S */ void WriteTo(xx::Data& d) const override;
			/* C */ int32_t ReadFrom(xx::Data_r& dr) override;

			xx::Weak<Scene> scene;
			XYi pos{}, halfSize{};
			xx::Math::BlockWayout wayout;
			bool isMapCorner{};

			virtual ~Block();
			Block& Init(Scene* scene_, int32_t minX, int32_t minY, int32_t maxX, int32_t maxY);
			Block& Init(Scene* scene_, XYi const& pos_, XYi const& siz_);

			void FillWayout();
			void AuthWayout(xx::Math::BlockWayout bw);

			bool IntersectCircle(XYi const& p, int32_t radius);
			bool PushOut(Monster* m, XYi& mp);

			void Draw();
		};

		struct Bullet_Base : xx::SerdeBase {
			static constexpr uint16_t cTypeId{ 5 };
			static constexpr uint16_t cParentTypeId{ xx::SerdeBase::cTypeId };
			/* S */ void WriteTo(xx::Data& d) const override { assert(false); }
			/* C */ int32_t ReadFrom(xx::Data_r& dr) override { assert(false); return 0; };

			xx::Weak<Scene> scene;
			XYp pos{};
			FX64 radians{};

			void Init(Scene* scene_, XYp const& pos_, FX64 radians_);
			virtual int32_t Update() { assert(false); return 0; };
			virtual void Draw() { assert(false); };
		};

		struct Bullet_Sector : Bullet_Base {
			using Base = Bullet_Base;
			static constexpr uint16_t cTypeId{ 6 };
			static constexpr uint16_t cParentTypeId{ Bullet_Base::cTypeId };
			/* S */ void WriteTo(xx::Data& d) const override;
			/* C */ int32_t ReadFrom(xx::Data_r& dr) override;

			static constexpr FX64 cRotateStep{ FX64{0.05} / Scene::fps60ratio };

			FX64 radius{}, theta{};

			Bullet_Sector& Init(Scene* scene_, XYp const& pos_, FX64 radians_, FX64 radius_, FX64 theta_);
			int32_t Update() override;	// non zero: kill
			void Draw() override;
		};

		struct Bullet_Box : Bullet_Base {
			using Base = Bullet_Base;
			static constexpr uint16_t cTypeId{ 7 };
			static constexpr uint16_t cParentTypeId{ Bullet_Base::cTypeId };
			/* S */ void WriteTo(xx::Data& d) const override;
			/* C */ int32_t ReadFrom(xx::Data_r& dr) override;

			static constexpr FX64 cRotateStep{ FX64{0.05} / Scene::fps60ratio };

			XYp size{};
			/* T */ XYp direction{};	// { cos(radians), sin(radians) }

			void FillDirectionByRadians();
			Bullet_Box& Init(Scene* scene_, XYp const& pos_, FX64 radians_, XYp const& size_);
			int32_t Update() override;	// non zero: kill
			void Draw() override;
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
