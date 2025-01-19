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

	float x{}, y{};
	ui->MakeChildren<xx::Button>()->Init(1, xy7m + XY{ x, y }, xy7a, btnCfg, U"play float version", [&]() {
		scene.Emplace<FloatVersion::Scene>()->Init();
	});

	y -= 50;
	ui->MakeChildren<xx::Button>()->Init(1, xy7m + XY{ x, y }, xy7a, btnCfg, U"play int version", [&]() {
		scene.Emplace<IntVersion::Scene>()->Init();
	});

	y -= 50;
	ui->MakeChildren<xx::Button>()->Init(1, xy7m + XY{ x, y }, xy7a, btnCfg, U"collision detection test", [&]() {
		scene.Emplace<CollisionDetection::Scene>()->Init();
	});

	// ...

	clearColor = { 33, 33, 33, 255 };
	
	camera.SetMaxFrameSize(64);
	//camera.SetOriginal(Msgs::Global::Scene::mapSize_2f);
	camera.SetScale(1.f);

	scene.Emplace<IntVersion::Scene>()->Init();
}

void Looper::Draw() {
	if (!scene) return;

	scene->Draw();
	DrawNode(ui);

	// draw tips
	gLooper.ctcDefault.Draw({ 0, gLooper.windowSize_2.y - 5 }, "move: A / D     jump: SPACE      down jump: S+SPACE", xx::RGBA8_Green, { 0.5f, 1 });
}

#include "scene_float.hpp"
#include "scene_int.hpp"
#include "scene_cd.hpp"
