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
	xx::FrameBuffer fb;
	xx::Task<> MainTask();		// loading logic
	bool ok{};	// true: loading finished

	xx::Shared<server::Scene> sScene;
	xx::Shared<client::Scene> cScene1, cScene2;
	xx::DataShared msg;	// todo: msgSyncCmd

	void AfterInit();
	void BeforeUpdate();
	void Update();				// fixed update
	void Draw();
};

extern Looper gLooper;
extern ResTpFrames& gRes;
