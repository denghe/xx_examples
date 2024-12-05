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

	sss.Emplace(sound.Load(LoadFileData<false>("res/1.ogg")));
	sss.Emplace(sound.Load(LoadFileData<false>("res/2.ogg")));
	sss.Emplace(sound.Load(LoadFileData<false>("res/button1.ogg"), true));
	sss.Emplace(sound.Load(LoadFileData<false>("res/gun1.ogg")));
	sss.Emplace(sound.Load(LoadFileData<false>("res/gun2.ogg")));
	sss.Emplace(sound.Load(LoadFileData<false>("res/gun3.ogg")));



	btnCfg.frame = res.button;
	btnCfg.texScale = { 0.99, 0.99 };
	btnCfg.center = { 2, 3, 2, 2 };
	btnCfg.color = { 0x5f, 0x15, 0xd9, 0xff };

	ui.Emplace()->Init();

	// todo: sound test buttons   play pause stop loop

	float x{ -50 }, y{ 200 };
	ui->MakeChildren<xx::Button>()->Init(1, xy5m + XY{ x, y }, xy7a, btnCfg, U"sound2 play 1.ogg", [this]() {
		sound2.Play(sss[0]);
	});

	y -= 50;
	ui->MakeChildren<xx::Button>()->Init(1, xy5m + XY{ x, y }, xy7a, btnCfg, U"sound1 play 2.ogg", [this]() {
		sound.Play(sss[1]);
	});

	y -= 50;
	ui->MakeChildren<xx::Button>()->Init(1, xy5m + XY{ x, y }, xy7a, btnCfg, U"sound1 loop play button1.ogg", [this]() {
		sound.Play(sss[2]);
	});

	y -= 50;
	ui->MakeChildren<xx::Button>()->Init(1, xy5m + XY{ x, y }, xy7a, btnCfg, U"sound1 play gun1.ogg", [this]() {
		sound.Play(sss[3]);
	});

	y -= 50;
	ui->MakeChildren<xx::Button>()->Init(1, xy5m + XY{ x, y }, xy7a, btnCfg, U"sound1 play gun2.ogg", [this]() {
		sound.Play(sss[4]);
	});

	y -= 50;
	ui->MakeChildren<xx::Button>()->Init(1, xy5m + XY{ x, y }, xy7a, btnCfg, U"sound1 play gun3.ogg", [this]() {
		sound.Play(sss[5]);
	});

	y -= 50;
	ui->MakeChildren<xx::Button>()->Init(1, xy5m + XY{ x, y }, xy7a, btnCfg, U"random play gun? 1 minutes", [this]() {
		soundTask = SoundTask();
	});

	y -= 50;
	ui->MakeChildren<xx::Button>()->Init(1, xy5m + XY{ x, y }, xy7a, btnCfg, U"sound1 pause all", [this]() {
		paused = true;
		sound.SetPauseAll(true);
	});

	y -= 50;
	ui->MakeChildren<xx::Button>()->Init(1, xy5m + XY{ x, y }, xy7a, btnCfg, U"sound1 resume all", [this]() {
		paused = false;
		sound.SetPauseAll(false);
	});

	y -= 50;
	ui->MakeChildren<xx::Button>()->Init(1, xy5m + XY{ x, y }, xy7a, btnCfg, U"sound* stop all", [this]() {
		soundTask.reset();
		sound.StopAll();
		sound2.StopAll();
	});

	// ...

	clearColor = { 33, 33, 33, 255 };
	
	camera.SetMaxFrameSize(64);
	//camera.SetOriginal(Msgs::Global::Scene::mapSize_2f);
	camera.SetScale(1.f);

	ok = true;
}

xx::Task<> Looper::SoundTask() {
	for (auto et = time + 60; time < et;) {
		//sound.Play(sss[rnd.Next(sss.Len() - 3) + 3]);
		sound.Play(sss[rnd.Next(sss.Len())]);
		co_yield 0;
	}
}


void Looper::Draw() {
	if (!ok) return;
	if (!paused) {
		if (soundTask.has_value()) {
			if ((*soundTask)()) {
				soundTask.reset();
			}
		}
	}
	DrawNode(ui);
}
