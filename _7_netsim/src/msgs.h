﻿#pragma once
#include "looper.h"

namespace Msgs {
	extern xx::SerdeInfo gSerdeInfo;
	void InitSerdeInfo();

	namespace Global {	// id = 1 ~ 999

		struct Player;
		struct Monster;
		struct Scene : xx::SerdeBase {
			static constexpr uint16_t cTypeId{ 1 };
			static constexpr uint16_t cParentTypeId{ xx::SerdeBase::cTypeId };

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
			/* S */ void WriteTo(xx::Data& d) const override;
			/* C */ int32_t ReadFrom(xx::Data_r& dr) override;
		};

		struct Player : xx::SerdeBase {
			static constexpr uint16_t cTypeId{ 2 };
			static constexpr uint16_t cParentTypeId{ xx::SerdeBase::cTypeId };

			xx::Weak<Scene> scene;
			int32_t clientId{};
			int32_t score{};

			void Init(Scene* scene_, int32_t clientId_);
			void Update();
			/* C */ void Draw();
			/* S */ void WriteTo(xx::Data& d) const override;
			/* C */ int32_t ReadFrom(xx::Data_r& dr) override;
		};

		struct MonsterData {
			FX64 x{}, y{}, radius{}, radians{}, frameIndex{};
			/* S */ void WriteTo(xx::Data& d) const;
			/* C */ int32_t ReadFrom(xx::Data_r& dr);
		};

		struct Monster : xx::SerdeBase, xx::Spacei32Item<Monster>, MonsterData {
			static constexpr uint16_t cTypeId{ 3 };
			static constexpr uint16_t cParentTypeId{ xx::SerdeBase::cTypeId };

			static constexpr FX64 cFrameIndexStep{ FX64{1} / FX64{10} };
			static constexpr FX64 cFrameIndexMax{ gRes._countof_monster_ };

			xx::Weak<Scene> scene;
			xx::Weak<Player> player;	// owner

			virtual ~Monster();
			void Init(Scene* scene_);
			/* C */ void Init(Scene* scene_, MonsterData const& md);
			virtual bool Update();	// true: kill
			/* C */ void Draw();
			/* S */ void WriteTo(xx::Data& d) const override;
			/* C */ int32_t ReadFrom(xx::Data_r& dr) override;
		};

	}

	namespace C2S {	// id == 1000 ~ 1999

		// todo: list/array data length limit protect

		struct Join : xx::SerdeBase {
			static constexpr uint16_t cTypeId{ 1001 };
			static constexpr uint16_t cParentTypeId{ xx::SerdeBase::cTypeId };
			/* S */ void WriteTo(xx::Data& d) const override;
			/* C */ int32_t ReadFrom(xx::Data_r& dr) override;
		};

		struct Summon : xx::SerdeBase {
			static constexpr uint16_t cTypeId{ 1002 };
			static constexpr uint16_t cParentTypeId{ xx::SerdeBase::cTypeId };
			/* S */ void WriteTo(xx::Data& d) const override;
			/* C */ int32_t ReadFrom(xx::Data_r& dr) override;
		};

	}

	namespace S2C {	// id == 2000 ~ 2999

		struct Join_r : xx::SerdeBase {
			static constexpr uint16_t cTypeId{ 2001 };
			static constexpr uint16_t cParentTypeId{ xx::SerdeBase::cTypeId };
			int32_t clientId{};
			xx::Shared<Global::Scene> scene;
			/* S */ void WriteTo(xx::Data& d) const override;
			/* C */ int32_t ReadFrom(xx::Data_r& dr) override;
		};

		struct Summon_r : xx::SerdeBase {
			static constexpr uint16_t cTypeId{ 1002 };
			static constexpr uint16_t cParentTypeId{ xx::SerdeBase::cTypeId };
			Global::MonsterData data;
			/* S */ void WriteTo(xx::Data& d) const override;
			/* C */ int32_t ReadFrom(xx::Data_r& dr) override;
		};

	}
}
