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
	gLooper.title = "xx_examples_a_sound";
	gLooper.Init();
	gLooper.Run();
}
#endif

xx::Task<> Looper::MainTask() {
	co_await res.AsyncLoad("res/");

	// todo: async download ogg files & preload	( maybe generate res code? )
	sound.Init();

	btnCfg.frame = res.button;
	btnCfg.texScale = { 0.99, 0.99 };
	btnCfg.center = { 2, 3, 2, 2 };
	btnCfg.color = { 0x5f, 0x15, 0xd9, 0xff };

	ui.Emplace()->Init();

	// todo: sound test buttons   play pause stop loop

	float x{ -50 }, y{ 100 };
	ui->MakeChildren<xx::Button>()->Init(1, xy5m + XY{ x, y }, xy7a, btnCfg, U"play1", [&]() {
		//id = sound.Play( ??? )  & store
	});

	y -= 50;
	ui->MakeChildren<xx::Button>()->Init(1, xy5m + XY{ x, y }, xy7a, btnCfg, U"stop1", [&]() {
		//sound.Stop( id )
	});

	y -= 50;
	ui->MakeChildren<xx::Button>()->Init(1, xy5m + XY{ x, y }, xy7a, btnCfg, U"play2", [&]() {
	});

	y -= 50;
	ui->MakeChildren<xx::Button>()->Init(1, xy5m + XY{ x, y }, xy7a, btnCfg, U"stop2", [&]() {
	});

	// ...

	clearColor = { 33, 33, 33, 255 };
	
	camera.SetMaxFrameSize(64);
	//camera.SetOriginal(Msgs::Global::Scene::mapSize_2f);
	camera.SetScale(1.f);

	ok = true;
}

void Looper::Draw() {
	if (!ok) return;

	DrawNode(ui);
}
