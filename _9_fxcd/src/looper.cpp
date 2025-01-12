﻿#include "pch.h"
#include "looper.h"
#include "server.h"
#include "client.h"
#include "msgs.h"

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
	gLooper.title = "xx_examples_9_fxcd";
	gLooper.Init();
	//gLooper.Run<false>();	// no sleep
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
	ui->MakeChildren<xx::Button>()->Init(1, xy7m + XY{ x, y }, xy7a, btnCfg, U"s1", [&]() {
		server.Emplace()->Init(1);
	});

	x += 60;
	ui->MakeChildren<xx::Button>()->Init(1, xy7m + XY{ x, y }, xy7a, btnCfg, U"s2", [&]() {
		server.Emplace()->Init(2);
	});

	x += 60;
	ui->MakeChildren<xx::Button>()->Init(1, xy7m + XY{ x, y }, xy7a, btnCfg, U"s3", [&]() {
		server.Emplace()->Init(3);
	});

	x += 60;
	ui->MakeChildren<xx::Button>()->Init(1, xy7m + XY{ x, y }, xy7a, btnCfg, U"s4", [&]() {
		server.Emplace()->Init(4);
	});

	// ...

	x = 0;
	y -= 50;
	ui->MakeChildren<xx::Button>()->Init(1, xy7m + XY{ x, y }, xy7a, btnCfg, U"client1 reset", [&]() {
		client1.Emplace()->Init();
	});

	clearColor = { 33, 33, 33, 255 };
	
	sgrdd.Init(200, 64);

	camera.SetMaxFrameSize(64);
	camera.SetOriginal(Msgs::Global::Scene::mapSize_2f);
	camera.SetScale(1.f);

	ok = true;
}

void Looper::AfterInit() {
	Msgs::InitSerdeInfo();
}

void Looper::BeforeUpdate() {
	if (!ok) return;

	// scale control
	if (gLooper.KeyDownDelay(xx::KeyboardKeys::Z, 0.01f)) {
		camera.IncreaseScale(0.01f, 5);
	} else if (gLooper.KeyDownDelay(xx::KeyboardKeys::X, 0.01f)) {
		camera.DecreaseScale(0.01f,  gLooper.height_2 / float(Msgs::Global::Scene::mapSize.y));
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
}

void Looper::Update() {
	if (!ok) return;

#ifndef NDEDBUG
	xx::DataShared ds, ds1, ds2, ds3, ds4;
#endif
	if (server) {
		server->Update();
#ifndef NDEDBUG
		ds = Msgs::gSerdeInfo.MakeDataShared(server->scene);
#endif
	}

	if (client1) {
		client1->Update();
#ifndef NDEDBUG
		ds1 = Msgs::gSerdeInfo.MakeDataShared(client1->scene);
#endif
	}

#ifndef NDEDBUG
	// verify data
	if (server && client1 && client1->Joined() && client1->recvs.Empty()) {
		if (ds.GetLen() != ds1.GetLen()) {
			xx::CoutTN("ds.GetLen() != ds1.GetLen() ", ds.GetLen(), " ", ds1.GetLen());
		} else {
			auto n = memcmp(ds.GetBuf(), ds1.GetBuf(), ds1.GetLen());
			if (n) {
				xx::CoutTN("memcmp(ds.GetBuf(), ds1.GetBuf(), ds1.GetLen()) = ", n);
			}
		}
	}
#endif
}

void Looper::Draw() {
	if (!ok) return;

	xx::Quad q;
	if (client1) {
		client1->Draw();
	}

	std::string str;
	if (!server) {
		str = xx::ToString("Please click [s?] first.");
	} else {
		str = xx::ToString("Z, X zoom, mouse1 drop. numBullets = ", server->scene->bullets.Count(),". numMmonsters = ", server->scene->monsters.Count());
	}
	gLooper.ctcDefault.Draw({ 0, -gLooper.windowSize_2.y + 5 }, str, xx::RGBA8_Red, { 0.5f, 0 });
	gLooper.ctcDefault.Draw(XY{ 0, -gLooper.windowSize_2.y + 5 } + XY{ 2, 2 }, str, xx::RGBA8_White, { 0.5f, 0 });

	DrawNode(ui);

	//xx::Quad().SetFrame(ctcDefault.texs[0]).Draw();	// show font for issue
}
