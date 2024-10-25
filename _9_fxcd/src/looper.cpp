#include "pch.h"
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
	gLooper.title = "xx_examples_8_netsim2";
	gLooper.Init();
	//gLooper.Run<true>();	// auto sleep
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

	float y = 0;
	ui->MakeChildren<xx::Button>()->Init(1, xy7m + XY{ 0, y }, xy7a, btnCfg, U"server reset", [&]() {
		server.Emplace()->Init();
	});

	y -= 50;
	ui->MakeChildren<xx::Button>()->Init(1, xy7m + XY{ 0, y }, xy7a, btnCfg, U"client1 reset", [&]() {
		client1.Emplace()->Init();
	});

	clearColor = { 33, 33, 33, 255 };
	fb.Init();

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

	// quickly reset support
	if (gLooper.KeyDownDelay(xx::KeyboardKeys::R, 0.1f)) {
		server.Emplace()->Init();
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
		str = xx::ToString("Please click [server reset] first.");
	} else {
		str = xx::ToString("Z, X zoom, MBtn1 or 2 drop. count = ", server->scene->monsters.Count());
	}
	gLooper.ctcDefault.Draw({ 0, gLooper.windowSize_2.y - 5 }, str, xx::RGBA8_Red, { 0.5f, 1 });
	gLooper.ctcDefault.Draw(XY{ 0, gLooper.windowSize_2.y - 5 } + XY{ 2, 2 }, str, xx::RGBA8_White, { 0.5f, 1 });

	DrawNode(ui);
}
