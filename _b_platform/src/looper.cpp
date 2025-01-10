#include "pch.h"
#include "looper.h"

Looper gLooper;
ResTpFrames& gRes(gLooper.res);
bool gIsServer{};

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
	gLooper.title = "xx_examples_b_platform";
	gLooper.Init();
	gLooper.Run();
}
#endif

xx::Task<> Looper::MainTask() {
	co_await res.AsyncLoad("res/");

	btnCfg.frame = res.button;
	btnCfg.texScale = { 0.99, 0.99 };
	btnCfg.center = { 2, 3, 2, 2 };
	btnCfg.color = { 0x5f, 0x15, 0xd9, 0xff };

	ui.Emplace()->Init();

	float x{}, y{};
	ui->MakeChildren<xx::Button>()->Init(1, xy7m + XY{ x, y }, xy7a, btnCfg, U"reset", [&]() {
		logic.Emplace()->Init();
	});

	//x += 60;
	//ui->MakeChildren<xx::Button>()->Init(1, xy7m + XY{ x, y }, xy7a, btnCfg, U"s2", [&]() {
	//	});

	// ...

	clearColor = { 33, 33, 33, 255 };
	
	camera.SetMaxFrameSize(64);
	//camera.SetOriginal(Msgs::Global::Scene::mapSize_2f);
	camera.SetScale(1.f);

	ok = true;
}

void Looper::Update() {
	if (!ok) return;
	if (logic) {
		logic->Update();
	}
}

void Looper::Draw() {
	if (!ok) return;
	if (logic) {
		logic->Draw();
	}
	DrawNode(ui);
}

#include "logic.hpp"
