#pragma once
#include "looper.h"

namespace Msgs {
	extern xx::SerdeInfo gSerdeInfo;
	void InitSerdeInfo();

	namespace Global {	// id = 1 ~ 99

		struct Player;
		struct Monster;
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
			/* C */ xx::Ref<xx::GLTexture> tex;
			/* C */ xx::Ref<xx::Frame> frame;

			void Init();
			/* C */ void InitForDraw();	// todo: recursive call all childs
			void Update();
			/* C */ void Draw();
			xx::Shared<Player> const& RefPlayer(int32_t clientId);
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

		struct MonsterData {
			FX64 x{}, y{}, radius{}, radians{}, frameIndex{};
			/* S */ void WriteTo(xx::Data& d) const;
			/* C */ int32_t ReadFrom(xx::Data_r& dr);
		};

		struct Monster : xx::SerdeBase, xx::Spacei32Item<Monster>, MonsterData {
			static constexpr uint16_t cTypeId{ 3 };
			static constexpr uint16_t cParentTypeId{ xx::SerdeBase::cTypeId };
			/* S */ void WriteTo(xx::Data& d) const override;
			/* C */ int32_t ReadFrom(xx::Data_r& dr) override;

			static constexpr FX64 cFrameIndexStep{ FX64{1} / FX64{10} };
			static constexpr FX64 cFrameIndexMax{ gRes._countof_monster_ };

			xx::Weak<Scene> scene;
			xx::Weak<Player> owner;

			virtual ~Monster();
			Monster* Init(Scene* scene_, xx::Shared<Player> const& owner_, xx::XYi const& bornPos);
			/* C */ Monster* Init(Scene* scene_, xx::Shared<Player> const& owner_, MonsterData const& md);
			virtual bool Update();	// true: kill
			/* C */ void Draw();
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

		// 1 to n
		struct Summon : xx::SerdeBase {
			static constexpr uint16_t cTypeId{ 203 };
			static constexpr uint16_t cParentTypeId{ xx::SerdeBase::cTypeId };
			/* S */ void WriteTo(xx::Data& d) const override;
			/* C */ int32_t ReadFrom(xx::Data_r& dr) override;

			int32_t clientId{};
			int64_t frameNumber{};
			Global::MonsterData data;
		};

	}
}
