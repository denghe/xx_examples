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
	gLooper.title = "xx_examples_2_plane";
	gLooper.Init();
	gLooper.Run<false>();
}
#endif


xx::Task<> Looper::MainTask() {
	co_await res.AsyncLoad("res/");

	//camera.SetMaxFrameSize(maxItemSize);
	//camera.SetOriginal(mapSize_2);
	//camera.SetScale(1.f);

	srdd.Init(128, 32);

	//grid.Init(physNumRows, physNumCols, physCellSize);

	ok = true;
}


void Looper::BeforeUpdate() {
	if (!ok) return;

	// scale control
	if (gLooper.KeyDownDelay(xx::KeyboardKeys::Z, 0.02f)) {
		camera.IncreaseScale(0.1f, 5);
	} else if (gLooper.KeyDownDelay(xx::KeyboardKeys::X, 0.02f)) {
		camera.DecreaseScale(0.1f, 0.1f);
	}

	camera.Calc();
}


void Looper::Update() {
	if (!ok) return;

	// todo
}


void Looper::Draw() {
	if (!ok) return;

	// todo

	// draw tips
	gLooper.ctcDefault.Draw({ 0, gLooper.windowSize_2.y - 5 }, "zoom: Z / X   resize hit area: A / D    move map: mouse right button   hit bug: mouse left button", xx::RGBA8_Green, { 0.5f, 1 });

	//auto str = xx::ToString("total item count = ", grid.Count());
	//gLooper.ctcDefault.Draw({ 0, gLooper.windowSize_2.y - 50 }, str, xx::RGBA8_Green, { 0.5f, 1 });
}
