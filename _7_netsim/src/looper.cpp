#include "pch.h"
#include "looper.h"

Looper gLooper;
ResTpFrames& gRes(gLooper.res);

#ifdef __EMSCRIPTEN__
int32_t main() {
	gLooper.EngineInit();
	emscripten_request_animation_frame_loop([](double ms, void*)->EM_BOOL {
		return gLooper.JsLoopCallback(ms);
		}, nullptr);
}
#else
int32_t main() {
	gLooper.showFps = true;
	gLooper.title = "xx_examples_7_netsim";
	gLooper.Init();
	gLooper.Run<true>();	// auto sleep
}
#endif


xx::Task<> Looper::MainTask() {
	co_await res.AsyncLoad("res/");

	s9cfg_btn.frame = res.button;
	s9cfg_btn.texScale = { 2, 2 };
	s9cfg_btn.center = { 2, 3, 2, 2 };
	s9cfg_btn.color = { 0x5f, 0x15, 0xd9, 0xff };

	ui.Emplace()->Init();

	gLooper.ui->MakeChildren<xx::Button>()->Init(1, gLooper.xy7m + XY{ 0, 0 }, gLooper.xy7a
		, gLooper.s9cfg_btn, U"server reset", [&]() {
			sScene.Emplace()->Init();
	});

	gLooper.ui->MakeChildren<xx::Button>()->Init(1, gLooper.xy7m + XY{ 0, -50 }, gLooper.xy7a
		, gLooper.s9cfg_btn, U"client1 add", [&]() {
			cScene1.Emplace()->Init({ -width_2 / 2, 0 });
	});

	gLooper.ui->MakeChildren<xx::Button>()->Init(1, gLooper.xy7m + XY{ 0, -100 }, gLooper.xy7a
		, gLooper.s9cfg_btn, U"client1 remove", [&]() {
			cScene1.Reset();
	});

	gLooper.ui->MakeChildren<xx::Button>()->Init(1, gLooper.xy7m + XY{ 0, -150 }, gLooper.xy7a
		, gLooper.s9cfg_btn, U"client2 add", [&]() {
			cScene2.Emplace()->Init({ width_2 / 2, 0 });
	});

	gLooper.ui->MakeChildren<xx::Button>()->Init(1, gLooper.xy7m + XY{ 0, -200 }, gLooper.xy7a
		, gLooper.s9cfg_btn, U"client2 remove", [&]() {
			cScene2.Reset();
	});

	clearColor = { 33, 33, 33, 255 };

	ok = true;
}

void Looper::BeforeUpdate() {
	if (!ok) return;
}

void Looper::Update() {
	if (!ok) return;

	if (sScene) sScene->Update();
	if (cScene1) cScene1->Update();
	if (cScene2) cScene2->Update();
}

void Looper::Draw() {
	if (!ok) return;

	if (cScene1) cScene1->Draw();
	if (cScene2) cScene2->Draw();

	xx::LineStrip ls;
	ls.FillBoxPoints({}, { 2, height }).Draw();

	gLooper.DrawNode(ui);
}

namespace server {

	void Scene::Init() {
		gLooper.msg.Clear();
		frameNumber = 1000;	// skip some cast delay
		// todo: init monsterGrid
	}

	void Scene::Update() {
		++frameNumber;

		for (int32_t i = monsters.len - 1; i >= 0; --i) {
			auto& m = monsters[i];
			if (m->Update()) {
				monsters.SwapRemoveAt(i);
			}
		}

		if (frameNumber % 180 == 0) {
			monsters.Emplace()->Init();
		}

		// todo: write data to msgFull 
	}

	Monster::~Monster() {
		if (_sgab) {
			_sgab->Remove(this);
			_sgab = {};
		}
	}

	void Monster::Init() {
	}

	bool Monster::Update() {
		return false;
	}

}

namespace client {
	void Scene::Init(XY centerPos_) {
		centerPos = centerPos_;
	}

	void Scene::Update() {
		// todo: restore all data from msg
	}

	void Scene::Draw() {
		for (int32_t i = 0, e = monsters.len; i < e; ++i) {
			auto& m = monsters[i];
			m->Draw();
		}

		xx::LineStrip ls;
		ls.FillCirclePoints(centerPos, 20).Draw();
	}

	void Monster::Init() {
	}

	bool Monster::Update() {
		return false;
	}

	void Monster::Draw() {

	}

}
