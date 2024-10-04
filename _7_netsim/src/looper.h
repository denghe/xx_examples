#pragma once
#include "pch.h"
#include "res_tp_frames.h"

namespace server {
	struct Scene;
}
namespace client {
	struct Scene;
}
struct Looper : xx::Engine<Looper>, xx::GDesign<1920, 1080, 60> {

	// all picture resource here
	ResTpFrames res;
	xx::Scale9SpriteConfig s9cfg_btn;
	xx::Shared<xx::Node> ui;
	xx::Task<> MainTask();		// loading logic
	bool ok{};	// true: loading finished

	xx::Shared<server::Scene> sScene;
	xx::Shared<client::Scene> cScene1, cScene2;
	xx::Data msg;	// todo: msgSyncCmd

	void AfterInit();
	void BeforeUpdate();
	void Update();				// fixed update
	void Draw();
};

namespace server {
	extern xx::SerdeInfo gSerdeInfo;
	void InitSerdeInfo();
	struct Monster : xx::SerdeBase, xx::SpaceGridAB2Item<Monster> {
		static constexpr uint16_t cTypeId{ 1 };
		static constexpr uint16_t cParentTypeId{ xx::SerdeBase::cTypeId };
		virtual ~Monster();
		void Init();
		virtual bool Update();	// true: kill
		virtual void WriteTo(xx::Data& d);
		Scene* scene;
		FX64 x{}, y{}, radians{}, radius{};
	};
	struct Scene {
		int64_t frameNumber{};
		xx::Rnd rnd;
		xx::SpaceGridAB2<Monster> monsterGrid;	// before monsters for life
		xx::Listi32<xx::Shared<Monster>> monsters;
		void Init();
		void Update();
	};
}

namespace client {
	extern xx::SerdeInfo gSerdeInfo;
	void InitSerdeInfo();
	struct Monster : xx::SerdeBase, xx::SpaceGridAB2Item<Monster> {
		static constexpr uint16_t cTypeId{ 1 };
		static constexpr uint16_t cParentTypeId{ xx::SerdeBase::cTypeId };
		virtual ~Monster();
		void Init();
		virtual bool Update();	// true: kill
		virtual void Draw();
		virtual int32_t ReadFrom(xx::Data_r& dr);
		Scene* scene;
		FX64 x{}, y{}, radians{}, radius{};
	};
	struct Scene {
		int64_t frameNumber{};
		xx::Rnd rnd;
		xx::SpaceGridAB2<Monster> monsterGrid;	// before monsters for life
		xx::Listi32<xx::Shared<Monster>> monsters;
		XY centerPos;
		void Init(XY centerPos_);
		void Update();
		void Draw();
	};
}

extern Looper gLooper;
extern ResTpFrames& gRes;
