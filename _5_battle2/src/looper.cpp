﻿#include "pch.h"
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
	s9cfg_btn.texScale = { 0.9999, 0.9999 };
	s9cfg_btn.center = { 2, 3, 2, 2 };
	s9cfg_btn.color = { 0x5f, 0x15, 0xd9, 0xff };

	ui.Emplace()->Init();

	float offsetY = 0;
	gLooper.ui->MakeChildren<xx::Button>()->Init(1, gLooper.xy7m + XY{ 0, offsetY }, gLooper.xy7a
		, gLooper.s9cfg_btn, U"NewGame 10k", [&]() {
			scene->NewGame(10000);
		});

	offsetY -= 50;
	gLooper.ui->MakeChildren<xx::Button>()->Init(1, gLooper.xy7m + XY{ 0, offsetY }, gLooper.xy7a
		, gLooper.s9cfg_btn, U"NewGame 20k", [&]() {
			scene->NewGame(20000);
		});

	offsetY -= 50;
	gLooper.ui->MakeChildren<xx::Button>()->Init(1, gLooper.xy7m + XY{ 0, offsetY }, gLooper.xy7a
		, gLooper.s9cfg_btn, U"NewGame 50k", [&]() {
			scene->NewGame(50000);
		});

	offsetY -= 50;
	gLooper.ui->MakeChildren<xx::Button>()->Init(1, gLooper.xy7m + XY{ 0, offsetY }, gLooper.xy7a
		, gLooper.s9cfg_btn, U"NewGame 100k", [&]() {
			scene->NewGame(100000);
		});

	offsetY -= 50;
	gLooper.ui->MakeChildren<xx::Button>()->Init(1, gLooper.xy7m + XY{ 0, offsetY }, gLooper.xy7a
		, gLooper.s9cfg_btn, U"Drop 1k Weapons", [&]() {
			for (int32_t i = 0; i < 1000; ++i) {
				auto t = gLooper.rnd.Next<bool>() ? Battle::ItemTypes::Staff1 : Battle::ItemTypes::Sword1;
				scene->DropItem(t);
			}
		});

	offsetY -= 50;
	gLooper.ui->MakeChildren<xx::Button>()->Init(1, gLooper.xy7m + XY{ 0, offsetY }, gLooper.xy7a
		, gLooper.s9cfg_btn, U"Speed: 1x", [&]() {
			this->updateSpeed = 1;
		});

	offsetY -= 50;
	gLooper.ui->MakeChildren<xx::Button>()->Init(1, gLooper.xy7m + XY{ 0, offsetY }, gLooper.xy7a
		, gLooper.s9cfg_btn, U"Speed: 5x", [&]() {
			this->updateSpeed = 5;
		});

	offsetY -= 50;
	gLooper.ui->MakeChildren<xx::Button>()->Init(1, gLooper.xy7m + XY{ 0, offsetY }, gLooper.xy7a
		, gLooper.s9cfg_btn, U"Speed: 10x", [&]() {
			this->updateSpeed = 10;
		});

	offsetY -= 50;
	gLooper.ui->MakeChildren<xx::Button>()->Init(1, gLooper.xy7m + XY{ 0, offsetY }, gLooper.xy7a
		, gLooper.s9cfg_btn, U"Speed: 20x", [&]() {
			this->updateSpeed = 20;
		});

	offsetY -= 50;
	gLooper.ui->MakeChildren<xx::Button>()->Init(1, gLooper.xy7m + XY{ 0, offsetY }, gLooper.xy7a
		, gLooper.s9cfg_btn, U"Speed: 50x", [&]() {
			this->updateSpeed = 50;
		});

	offsetY -= 50;
	gLooper.ui->MakeChildren<xx::Button>()->Init(1, gLooper.xy7m + XY{ 0, offsetY }, gLooper.xy7a
		, gLooper.s9cfg_btn, U"Speed: 100x", [&]() {
			this->updateSpeed = 100;
		});

	offsetY -= 50;
	gLooper.ui->MakeChildren<xx::Button>()->Init(1, gLooper.xy7m + XY{ 0, offsetY }, gLooper.xy7a
		, gLooper.s9cfg_btn, U"Zoom: show all", [&]() {
			camera.SetScale(cameraMinScale);
		});

	offsetY -= 50;
	gLooper.ui->MakeChildren<xx::Button>()->Init(1, gLooper.xy7m + XY{ 0, offsetY }, gLooper.xy7a
		, gLooper.s9cfg_btn, U"Zoom: 1x", [&]() {
			camera.SetScale(1.f);
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
		camera.DecreaseScale(0.05f, cameraMinScale);
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

	for (int32_t i = 0; i < updateSpeed; ++i) {
		scene->Update();
	}
}

void Looper::Draw() {
	if (!ok) return;

	scene->Draw();

	auto str = xx::ToString("Z, X zoom, mouse RightButton drag map "
		, "monster count = ", scene->monsters.Count()
	);
	gLooper.ctcDefault.Draw({ 0, gLooper.windowSize_2.y - 5 }, str, xx::RGBA8_Red, { 0.5f, 1 });
	gLooper.ctcDefault.Draw(XY{ 0, gLooper.windowSize_2.y - 5 } + XY{ 2, 2 }, str, xx::RGBA8_White, { 0.5f, 1 });

	// draw ui
	gLooper.DrawNode(ui);
}
