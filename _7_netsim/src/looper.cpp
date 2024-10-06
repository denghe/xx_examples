#include "pch.h"
#include "looper.h"
#include "server.h"
#include "client.h"

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
			cScene1.Emplace()->Init();
	});

	gLooper.ui->MakeChildren<xx::Button>()->Init(1, gLooper.xy7m + XY{ 0, -100 }, gLooper.xy7a
		, gLooper.s9cfg_btn, U"client1 remove", [&]() {
			cScene1.Reset();
	});

	gLooper.ui->MakeChildren<xx::Button>()->Init(1, gLooper.xy7m + XY{ 0, -150 }, gLooper.xy7a
		, gLooper.s9cfg_btn, U"client2 add", [&]() {
			cScene2.Emplace()->Init();
	});

	gLooper.ui->MakeChildren<xx::Button>()->Init(1, gLooper.xy7m + XY{ 0, -200 }, gLooper.xy7a
		, gLooper.s9cfg_btn, U"client2 remove", [&]() {
			cScene2.Reset();
	});

	clearColor = { 33, 33, 33, 255 };
	fb.Init();

	ok = true;
}

void Looper::AfterInit() {
	server::InitSerdeInfo();
	client::InitSerdeInfo();
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

	xx::Quad q;
	if (cScene1) {
		cScene1->Draw();
		q.SetFrame(cScene1->frame).SetPosition({ -width_2 / 2, 0 }).Draw();
	}
	if (cScene2) {
		cScene2->Draw();
		q.SetFrame(cScene2->frame).SetPosition({ width_2 / 2, 0 }).Draw();
	}

	xx::LineStrip ls;
	ls.FillBoxPoints({}, { 2, height }).SetColor(xx::RGBA8_Red).Draw();

	gLooper.DrawNode(ui);
}
