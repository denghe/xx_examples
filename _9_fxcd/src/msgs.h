#pragma once
#include "looper.h"

namespace Msgs {
	extern xx::SerdeInfo gSerdeInfo;
	void InitSerdeInfo();

	namespace Global {	// id = 1 ~ 99
		struct Scene;
		struct EffectText {
			constexpr static float cCharPixelWidth{ 9.f };
			constexpr static float cScale{ 3.f };

			constexpr static int64_t cMoveDurationSecondsFrames{ int64_t(0.5 * Looper::fps) };
			constexpr static float cMoveSpeedMin{ 20 / gLooper.fps };
			constexpr static float cMoveSpeedMax{ 50 / gLooper.fps };

			constexpr static float cFadeOutDurationSeconds{ 0.2f };
			constexpr static float cFadeOutStep = 1.f / (cFadeOutDurationSeconds / gLooper.frameDelay);

			xx::Weak<Scene> scene;
			std::array<char, 12> buf{};		// value to string cache
			int32_t len{};					// buf's len

			int32_t lineNumber{};
			xx::RGBA8 color{};
			XY pos{}, inc{};
			int64_t timeout{};
			float alpha{};

			void Init(Scene* scene_, XY const& pos_, XY const& direction_, xx::RGBA8 color_, int32_t value_);

			int32_t UpdateCore();
			bool Update();

			void Draw();
		};
	}
}

namespace xx {
	template<>
	struct IsPod<Msgs::Global::EffectText, void> : std::true_type {};

	template<typename T>
	struct DataFuncs<T, std::enable_if_t< std::is_base_of_v<Msgs::Global::EffectText, T> >> {
		template<bool needReserve = true>
		static inline void Write(Data& d, T const& in) {
			xx::BufLenRef blr{ in.buf.data(), &in.len };
			d.Write(in.scene, blr, in.lineNumber, (float&)in.color, in.pos, in.inc, in.timeout, in.alpha);
		}
		static inline int Read(Data_r& d, T& out) {
			xx::BufLenRef blr{ out.buf.data(), &out.len };
			return d.Read(out.scene, blr , out.lineNumber, (float&)out.color, out.pos, out.inc, out.timeout, out.alpha);
		}
	};
}

namespace Msgs {
	namespace Global {

		struct Player;
		struct Monster;
		struct Block;
		struct Bullet_Base;

		struct Scene : xx::SerdeBase {
			static constexpr uint16_t cTypeId{ 1 };	/////////////////////////////////////////////////////////////////////////////
			static constexpr uint16_t cParentTypeId{ xx::SerdeBase::cTypeId };
			/* S */ void WriteTo(xx::Data& d) const override;
			/* C */ int32_t ReadFrom(xx::Data_r& dr) override;

			// int max value == 0x7FFF FFFF. sqrt == 46340. / 96 ~= 482		/ 64 = 724
			static constexpr FX64 maxDistance{ 46340 };
			static constexpr int32_t cellSize{ 64 };
			static constexpr int32_t numRows{ (int32_t)Looper::height / cellSize };
			static constexpr int32_t numCols{ (int32_t)Looper::width / cellSize };
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
			xx::Queue<EffectText> effectTexts;
			/* T */ bool disposing{};	// ~Scene() == true

			void Init(int32_t sid);
			/* C */ void InitForDraw();	// todo: recursive call all childs
			void Update();
			/* C */ void Draw();
			xx::Shared<Player> const& RefPlayer(int32_t clientId);
			void RemovePlayer(int32_t clientId);
			void PosLimitByMapSize(XYp& p);
			void MakeEffectText(xx::XY const& pos_, xx::XY const& dist_, xx::RGBA8 color_, int32_t value_);
			~Scene();
		};

	}
}

namespace Msgs {
	namespace Global {

		struct Player : xx::SerdeBase {
			static constexpr uint16_t cTypeId{ 2 }; /////////////////////////////////////////////////////////////////////////////
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

	}
}

namespace Msgs {
	namespace Global {

		// todo: change color

		struct Bullet_Base;
		struct Monster : xx::SerdeBase, xx::Spacei32Item<Monster> {
			static constexpr uint16_t cTypeId{ 3 }; /////////////////////////////////////////////////////////////////////////////
			static constexpr uint16_t cParentTypeId{ xx::SerdeBase::cTypeId };
			/* S */ void WriteTo(xx::Data& d) const override;
			/* C */ int32_t ReadFrom(xx::Data_r& dr) override;

			static constexpr FX64 cResRadius{ 32 };
			static constexpr FX64 cFrameIndexStep{ FX64{0.1} / Scene::fps60ratio };
			static constexpr FX64 cFrameIndexMax{ ResTpFrames::_countof_monster_ };
			static constexpr FX64 cMovementSpeed{ FX64{5} / Scene::fps60ratio };

			static constexpr float cColorPlusDefault{ 1 };
			static constexpr float cColorPlusWhite{ 100000 };
			static constexpr int32_t cColorPlusChangeDuration{ int32_t(0.1 * Looper::fps) };

			static constexpr int32_t cMaxHP{ 100 };

			xx::Weak<Scene> scene;
			xx::Weak<Player> owner;

			XYp pos, tarPos;
			FX64 radius{}, radians{}, frameIndex{};
			int64_t changeColorToWhiteElapsedTime{};
			int32_t indexAtContainer{ -1 };
			/* T */ XYp inc{}, newPos{};

			int32_t hp{};


			virtual ~Monster();
			Monster* Init(Scene* scene_, xx::Shared<Player> const& owner_, xx::XYi const& bornPos);
			virtual void Update1();	// concurrent support
			virtual int32_t Update2();	// non zero: kill
			/* C */ void Draw();
			/* C */ void DrawBars();
			bool FillCrossInc(XYp const& pos_);
			int32_t BlocksLimit(XYp& pos_);
			void Kill(/* todo: killer */);	// remove from scene.monsters & destroy
			bool Hurt(Bullet_Base* bullet_);
		};

	}
}

namespace Msgs {
	namespace Global {

		struct Block : xx::SerdeBase, xx::SpaceABi32Item<Block> {
			static constexpr uint16_t cTypeId{ 4 }; /////////////////////////////////////////////////////////////////////////////
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

	}
}

namespace Msgs {
	namespace Global {

		struct Projectile : xx::SerdeBase {
			static constexpr uint16_t cTypeId{ 5 }; /////////////////////////////////////////////////////////////////////////////
			static constexpr uint16_t cParentTypeId{ xx::SerdeBase::cTypeId };
			/* S */ void WriteTo(xx::Data& d) const override;
			/* C */ int32_t ReadFrom(xx::Data_r& dr) override;

			xx::Weak<Scene> scene;
			XYp pos{};
			FX64 radians{};
			/* T */ XYp direction{};	// { cos(radians), sin(radians) }

			void FillDirectionByRadians();
			virtual int32_t Update() { assert(false); return 0; };
			virtual void Draw() { assert(false); };
		};

		struct Bullet_Base : Projectile {
			using Base = Projectile;
			static constexpr uint16_t cTypeId{ 6 }; /////////////////////////////////////////////////////////////////////////////
			static constexpr uint16_t cParentTypeId{ xx::SerdeBase::cTypeId };
			/* S */ void WriteTo(xx::Data& d) const override;
			/* C */ int32_t ReadFrom(xx::Data_r& dr) override;

			static constexpr int32_t cPierceDelay{ uint32_t(Looper::fps) >> 3 };

			int32_t damage{};
			xx::Listi32<std::pair<xx::Weak<Monster>, int64_t>> hitBlackList;

			void Init(Scene* scene_, XYp const& pos_, FX64 radians_, int32_t damage_);
			void HitBlackListClear(int32_t pierceDelay_);
			bool HitBlackListTryAdd(int32_t pierceDelay_, Monster* m);
			// todo
		};

		struct Bullet_Sector : Bullet_Base {
			using Base = Bullet_Base;
			static constexpr uint16_t cTypeId{ 7 }; /////////////////////////////////////////////////////////////////////////////
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
			static constexpr uint16_t cTypeId{ 8 }; /////////////////////////////////////////////////////////////////////////////
			static constexpr uint16_t cParentTypeId{ Bullet_Base::cTypeId };
			/* S */ void WriteTo(xx::Data& d) const override;
			/* C */ int32_t ReadFrom(xx::Data_r& dr) override;

			static constexpr FX64 cRotateStep{ FX64{0.05} / Scene::fps60ratio };

			XYp size{};

			Bullet_Box& Init(Scene* scene_, XYp const& pos_, FX64 radians_, XYp const& size_);
			int32_t Update() override;	// non zero: kill
			void Draw() override;
		};

	}
}

namespace Msgs {
	namespace C2S {	// id == 100 ~ 199

		// todo: list/array data length limit protect

		struct Join : xx::SerdeBase {
			static constexpr uint16_t cTypeId{ 101 }; /////////////////////////////////////////////////////////////////////////////
			static constexpr uint16_t cParentTypeId{ xx::SerdeBase::cTypeId };
			/* S */ void WriteTo(xx::Data& d) const override;
			/* C */ int32_t ReadFrom(xx::Data_r& dr) override;
		};

		struct Summon : xx::SerdeBase {
			static constexpr uint16_t cTypeId{ 102 }; /////////////////////////////////////////////////////////////////////////////
			static constexpr uint16_t cParentTypeId{ xx::SerdeBase::cTypeId };
			/* S */ void WriteTo(xx::Data& d) const override;
			/* C */ int32_t ReadFrom(xx::Data_r& dr) override;

			xx::XYi bornPos;
		};

	}
}

namespace Msgs {
	namespace S2C {	// id == 200 ~ 299

		// 1 to 1
		struct Join_r : xx::SerdeBase {
			static constexpr uint16_t cTypeId{ 201 }; /////////////////////////////////////////////////////////////////////////////
			static constexpr uint16_t cParentTypeId{ xx::SerdeBase::cTypeId };
			/* S */ void WriteTo(xx::Data& d) const override;
			/* C */ int32_t ReadFrom(xx::Data_r& dr) override;

			int32_t clientId{};
			xx::Shared<Global::Scene> scene;
		};

		// 1 to n ( after Join_r )
		struct PlayerJoin : xx::SerdeBase {
			static constexpr uint16_t cTypeId{ 202 }; /////////////////////////////////////////////////////////////////////////////
			static constexpr uint16_t cParentTypeId{ xx::SerdeBase::cTypeId };
			/* S */ void WriteTo(xx::Data& d) const override;
			/* C */ int32_t ReadFrom(xx::Data_r& dr) override;

			int32_t clientId{};
			int64_t frameNumber{};
		};

		// 1 to n ( after player disconnect )
		struct PlayerLeave : xx::SerdeBase {
			static constexpr uint16_t cTypeId{ 203 }; /////////////////////////////////////////////////////////////////////////////
			static constexpr uint16_t cParentTypeId{ xx::SerdeBase::cTypeId }; 
			/* S */ void WriteTo(xx::Data& d) const override;
			/* C */ int32_t ReadFrom(xx::Data_r& dr) override;

			int32_t clientId{};
			int64_t frameNumber{};
		};

		// 1 to n
		struct Summon : xx::SerdeBase {
			static constexpr uint16_t cTypeId{ 204 }; /////////////////////////////////////////////////////////////////////////////
			static constexpr uint16_t cParentTypeId{ xx::SerdeBase::cTypeId };
			/* S */ void WriteTo(xx::Data& d) const override;
			/* C */ int32_t ReadFrom(xx::Data_r& dr) override;

			int32_t clientId{};
			int64_t frameNumber{};
			xx::XYi bornPos;
		};

	}
}
