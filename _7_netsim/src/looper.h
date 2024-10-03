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

	void BeforeUpdate();
	void Update();				// fixed update
	void Draw();
};

namespace server {
	struct Monster : xx::SpaceGridAB2Item<Monster> {
		virtual ~Monster();
		void Init();
		virtual bool Update();	// true: kill
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
	struct Monster : xx::SpaceGridAB2Item<Monster> {
		virtual ~Monster() {}
		void Init();
		virtual bool Update();	// true: kill
		virtual void Draw();
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
