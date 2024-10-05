#pragma once
#include "looper.h"

namespace client {
	extern xx::SerdeInfo gSerdeInfo;
	void InitSerdeInfo();

	struct Monster : xx::SerdeBase, SpaceItem<Monster> {
		static constexpr uint16_t cTypeId{ 1 };
		static constexpr uint16_t cParentTypeId{ xx::SerdeBase::cTypeId };
		virtual ~Monster();
		void Init();
		virtual bool Update();	// true: kill
		virtual void Draw();
		virtual int32_t ReadFrom(xx::Data_r& dr);
		Scene* scene{};
		FX64 x{}, y{}, radius{}, radians{};
	};

	struct Scene {
		int64_t frameNumber{};
		xx::Rnd rnd;
		Space<Monster> monsterSpace;
		xx::Listi32<xx::Shared<Monster>> monsters;
		XY centerPos;
		void Init(XY centerPos_);
		void Update();
		void Draw();
	};
}
