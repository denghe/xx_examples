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

	auto s_1 = sound.Load(LoadFileData<false>("res/1.ogg"));
	auto s_2 = sound.Load(LoadFileData<false>("res/2.ogg"));
	auto s_button1 = sound.Load(LoadFileData<false>("res/button1.ogg"));
	auto s_gun1 = sound.Load(LoadFileData<false>("res/gun1.ogg"));
	auto s_gun2 = sound.Load(LoadFileData<false>("res/gun2.ogg"));
	auto s_gun3 = sound.Load(LoadFileData<false>("res/gun3.ogg"));



	btnCfg.frame = res.button;
	btnCfg.texScale = { 0.99, 0.99 };
	btnCfg.center = { 2, 3, 2, 2 };
	btnCfg.color = { 0x5f, 0x15, 0xd9, 0xff };

	ui.Emplace()->Init();

	// todo: sound test buttons   play pause stop loop

	float x{ -50 }, y{ 100 };
	ui->MakeChildren<xx::Button>()->Init(1, xy5m + XY{ x, y }, xy7a, btnCfg, U"play 1.ogg", [this, s_1]() {
		sound.Play(s_1);
	});

	y -= 50;
	ui->MakeChildren<xx::Button>()->Init(1, xy5m + XY{ x, y }, xy7a, btnCfg, U"play 2.ogg", [this, s_2]() {
		sound.Play(s_2);
	});

	y -= 50;
	ui->MakeChildren<xx::Button>()->Init(1, xy5m + XY{ x, y }, xy7a, btnCfg, U"play button1.ogg", [this, s_button1]() {
		sound.Play(s_button1);
	});

	y -= 50;
	ui->MakeChildren<xx::Button>()->Init(1, xy5m + XY{ x, y }, xy7a, btnCfg, U"play gun1.ogg", [this, s_gun1]() {
		sound.Play(s_gun1);
	});

	y -= 50;
	ui->MakeChildren<xx::Button>()->Init(1, xy5m + XY{ x, y }, xy7a, btnCfg, U"play gun1.ogg", [this, s_gun2]() {
		sound.Play(s_gun2);
	});

	y -= 50;
	ui->MakeChildren<xx::Button>()->Init(1, xy5m + XY{ x, y }, xy7a, btnCfg, U"play gun1.ogg", [this, s_gun3]() {
		sound.Play(s_gun3);
	});

	y -= 50;
	ui->MakeChildren<xx::Button>()->Init(1, xy5m + XY{ x, y }, xy7a, btnCfg, U"stop all", [this]() {
		sound.soloud.stopAll();
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
