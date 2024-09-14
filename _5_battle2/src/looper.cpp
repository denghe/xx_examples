#include "pch.h"
#include "looper.h"
#include "battle.h"

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
	gLooper.title = "xx_examples_4_battle";
	gLooper.Init();
	gLooper.Run<false>();
}
#endif


xx::Task<> Looper::MainTask() {
	co_await res.AsyncLoad("res/");

	s9cfg_btn.frame = res.button;
	s9cfg_btn.texScale = { 2, 2 };
	s9cfg_btn.center = { 2, 3, 2, 2 };
	s9cfg_btn.color = { 0x5f, 0x15, 0xd9, 0xff };

	ui.Emplace()->Init();

	//ui->MakeChildren<xx::Button>()->Init(1, xy7m + XY{0, 0}, xy7a, s9cfg_btn, U"run battle test", [&]() {
	//	Battle::Test();
	//});

	camera.SetMaxFrameSize(maxItemSize);
	camera.SetOriginal(mapSize_2);
	camera.SetScale(1.f);

	scene.Emplace();
	::scene = scene.pointer;
	scene->Init();

	clearColor = { 33, 33, 33, 255 };

	ok = true;
}

void Looper::BeforeUpdate() {
	if (!ok) return;

	// scale control
	if (gLooper.KeyDownDelay(xx::KeyboardKeys::Z, 0.02f)) {
		camera.IncreaseScale(0.05f, 5);
	} else if (gLooper.KeyDownDelay(xx::KeyboardKeys::X, 0.02f)) {
		camera.DecreaseScale(0.05f, 0.45f);
	}

	camera.Calc();

	scene->BeforeUpdate();
}


void Looper::Update() {
	if (!ok) return;

	scene->Update();
}


void Looper::Draw() {
	if (!ok) return;

	scene->Draw();

	auto str = xx::ToString("monster count = ", scene->monsters.Count()
		, "  effect count = ", scene->bladeLights.Count() + scene->explosions.Count() + scene->effectTextManager.ens.Count()
	);
	gLooper.ctcDefault.Draw({ 0, gLooper.windowSize_2.y - 5 }, str, xx::RGBA8_Red, { 0.5f, 1 });
	gLooper.ctcDefault.Draw(XY{ 0, gLooper.windowSize_2.y - 5 } + XY{ 2, 2 }, str, xx::RGBA8_White, { 0.5f, 1 });

	// draw ui
	gLooper.DrawNode(ui);
}
