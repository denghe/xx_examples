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

	gLooper.ui->MakeChildren<xx::Button>()->Init(1, gLooper.xy7m + XY{ 0, 0 }, gLooper.xy7a
		, gLooper.s9cfg_btn, U"NewGame", [&]() {
			scene->NewGame();
		});

	gLooper.ui->MakeChildren<xx::Button>()->Init(1, gLooper.xy7m + XY{ 0, -50 }, gLooper.xy7a
		, gLooper.s9cfg_btn, U"Speed: 1x", [&]() {
			this->updateSpeed = 1;
		});

	gLooper.ui->MakeChildren<xx::Button>()->Init(1, gLooper.xy7m + XY{ 0, -100 }, gLooper.xy7a
		, gLooper.s9cfg_btn, U"Speed: 10x", [&]() {
			this->updateSpeed = 10;
		});

	gLooper.ui->MakeChildren<xx::Button>()->Init(1, gLooper.xy7m + XY{ 0, -150 }, gLooper.xy7a
		, gLooper.s9cfg_btn, U"Speed: 100x", [&]() {
			this->updateSpeed = 100;
		});

	gLooper.ui->MakeChildren<xx::Button>()->Init(1, gLooper.xy7m + XY{ 0, -200 }, gLooper.xy7a
		, gLooper.s9cfg_btn, U"Speed: 1000x", [&]() {
			this->updateSpeed = 1000;
		});

	camera.SetMaxFrameSize(maxItemSize);
	camera.SetOriginal(mapSize_2);
	camera.SetScale(1.f);

	scene.Emplace();
	gScene = scene.pointer;
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

	// move control
	if (!gLooper.mouseEventHandler) {			// not in ui
		auto& m = gLooper.mouse;
		auto mbs = m.PressedMBRight();

		auto mp = m.pos / camera.scale;
		mp.x = -mp.x;

		if (lastMBState != mbs) {
			lastMBState = mbs;
			if (mbs) {							// mouse down
				mouseOffset = mp - camera.original;
				lastMousePos = m.pos;
			} else {							// mouse up
				if (dragging) {					// dragging end
					dragging = false;
				} else {						// click
					//MouseHit();
				}
			}
		}
		if (mbs && xx::Calc::DistancePow2(lastMousePos, m.pos) > 16) {		// mouse down + moved == dragging
			dragging = true;
		}
		if (dragging) {
			camera.original = mp - mouseOffset;
		}

	}


	camera.Calc();
}


void Looper::Update() {
	if (!ok) return;

	//// drop monster
	//if (mouse.PressedMBLeft()) {
	//	auto p = camera.ToLogicPos(mouse.pos);
	//	scene->TryMakeMonster(p);
	//}

	for (int32_t i = 0; i < updateSpeed; ++i) {
		scene->Update();
	}
}


void Looper::Draw() {
	if (!ok) return;

	scene->Draw();

	auto str = xx::ToString("Z, X zoom, mouse RightButton drag map "
		, "monster count = ", scene->monsters.Count()
		, "  effect count = ", scene->bladeLights.Count() + scene->explosions.Count() + scene->effectTextManager.ens.Count()
	);
	gLooper.ctcDefault.Draw({ 0, gLooper.windowSize_2.y - 5 }, str, xx::RGBA8_Red, { 0.5f, 1 });
	gLooper.ctcDefault.Draw(XY{ 0, gLooper.windowSize_2.y - 5 } + XY{ 2, 2 }, str, xx::RGBA8_White, { 0.5f, 1 });

	// draw ui
	gLooper.DrawNode(ui);
}
