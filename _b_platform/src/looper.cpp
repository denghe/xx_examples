#include "pch.h"
#include "looper.h"

#include "scene_float.hpp"
#include "scene_int.hpp"
#include "scene_int2.hpp"
#include "scene_cd.hpp"
#include "scene_cd2.hpp"
#include "scene_ai.hpp"

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
	gLooper.Run<false>();
}
#endif

xx::Task<> Looper::MainTask() {
	co_await res.AsyncLoad("res/");

	btnCfg.frame = res.button;
	btnCfg.texScale = { 0.99, 0.99 };
	btnCfg.center = { 2, 3, 2, 2 };
	btnCfg.color = { 0x5f, 0x15, 0xd9, 0xff };

	ui.Emplace()->Init();

	float x{}, y{ 50 };

	y -= 50;
	ui->MakeChildren<xx::Button>()->Init(1, xy7m + XY{ x, y }, xy7a, btnCfg, U"restart", [&]() {
		scene.Emplace<AI::Scene>()->Init();
	});

	//y -= 50;
	//ui->MakeChildren<xx::Button>()->Init(1, xy7m + XY{ x, y }, xy7a, btnCfg, U"play int version", [&]() {
	//	scene.Emplace<IntVersion::Scene>()->Init();
	//});

	//y -= 50;
	//ui->MakeChildren<xx::Button>()->Init(1, xy7m + XY{ x, y }, xy7a, btnCfg, U"play", [&]() {
	//	scene.Emplace<IntVersion2::Scene>()->Init();
	//});

	//y -= 50;
	//ui->MakeChildren<xx::Button>()->Init(1, xy7m + XY{ x, y }, xy7a, btnCfg, U"pushout test", [&]() {
	//	scene.Emplace<CollisionDetection::Scene>()->Init();
	//});

	//y -= 50;
	//ui->MakeChildren<xx::Button>()->Init(1, xy7m + XY{ x, y }, xy7a, btnCfg, U"pushout2 test", [&]() {
	//	scene.Emplace<CollisionDetection2::Scene>()->Init();
	//});

	// ...

	clearColor = { 33, 33, 33, 255 };
	
	camera.SetMaxFrameSize(64);
	//camera.SetOriginal(Msgs::Global::Scene::mapSize_2f);
	camera.SetScale(1.f);

	scene.Emplace<AI::Scene>()->Init();
}

void Looper::Draw() {
	if (scene) {
		scene->Draw();
		DrawNode(ui);
	}
}
