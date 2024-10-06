#pragma once
#include "looper.h"

namespace server {
	extern xx::SerdeInfo gSerdeInfo;
	void InitSerdeInfo();

	struct Monster : xx::SerdeBase, SpaceItem<Monster> {
		static constexpr uint16_t cTypeId{ 1 };
		static constexpr uint16_t cParentTypeId{ xx::SerdeBase::cTypeId };
		static constexpr FX64 cFrameIndexStep{ FX64{1} / FX64{10} };
		static constexpr FX64 cFrameIndexMax{ gRes._countof_monster_ };
		virtual ~Monster();
		void Init(Scene* scene_);
		virtual bool Update();	// true: kill
		//virtual void Draw();
		virtual void WriteTo(xx::Data& d);
		Scene* scene{};
		FX64 x{}, y{}, radius{}, radians{}, frameIndex{};
	};

	struct Scene {
		int64_t frameNumber{};
		xx::Rnd rnd;
		Space<Monster> monsterSpace;
		xx::Listi32<xx::Shared<Monster>> monsters;
		//XY centerPos;
		void Init();
		void Update();
		//void Draw();
	};
}
