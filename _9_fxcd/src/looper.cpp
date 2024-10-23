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
		client1.Emplace()->Init({ -width_2 / 2, height_2 / 2 });
	});

	y -= 50;
	ui->MakeChildren<xx::Button>()->Init(1, xy7m + XY{ 0, y }, xy7a, btnCfg, U"client1 remove", [&]() {
		client1.Reset();
	});

	y -= 50;
	ui->MakeChildren<xx::Button>()->Init(1, xy7m + XY{ 0, y }, xy7a, btnCfg, U"client2 reset", [&]() {
		client2.Emplace()->Init({ width_2 / 2, height_2 / 2 });
	});

	y -= 50;
	ui->MakeChildren<xx::Button>()->Init(1, xy7m + XY{ 0, y }, xy7a, btnCfg, U"client2 remove", [&]() {
		client2.Reset();
	});

	y -= 50;
	ui->MakeChildren<xx::Button>()->Init(1, xy7m + XY{ 0, y }, xy7a, btnCfg, U"client3 reset", [&]() {
		client3.Emplace()->Init({ -width_2 / 2, -height_2 / 2 });
	});

	y -= 50;
	ui->MakeChildren<xx::Button>()->Init(1, xy7m + XY{ 0, y }, xy7a, btnCfg, U"client3 remove", [&]() {
		client3.Reset();
	});

	y -= 50;
	ui->MakeChildren<xx::Button>()->Init(1, xy7m + XY{ 0, y }, xy7a, btnCfg, U"client4 reset", [&]() {
		client4.Emplace()->Init({ width_2 / 2, -height_2 / 2 });
	});

	y -= 50;
	ui->MakeChildren<xx::Button>()->Init(1, xy7m + XY{ 0, y }, xy7a, btnCfg, U"client4 remove", [&]() {
		client4.Reset();
	});

	y -= 50;
	ui->MakeChildren<xx::Button>()->Init(1, xy7m + XY{ 0, y }, xy7a, btnCfg, U"camera fill", [&]() {
		camera.SetScale(gLooper.height_2 / float(Msgs::Global::Scene::mapSize.y));
	});

	y -= 50;
	ui->MakeChildren<xx::Button>()->Init(1, xy7m + XY{ 0, y }, xy7a, btnCfg, U"camra 1:1", [&]() {
		camera.SetScale(1);
	});

	y -= 50;
	ui->MakeChildren<xx::Button>()->Init(1, xy7m + XY{ 0, y }, xy7a, btnCfg, U"gen 250 monsters", [&]() {
		if (server) {
			server->Gen250Monsters();
		}
	});

	clearColor = { 33, 33, 33, 255 };
	fb.Init();

	camera.SetMaxFrameSize(64);			// unused
	camera.SetOriginal(100 * 96 / 2);	// unused
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

	if (client2) {
		client2->Update();
#ifndef NDEDBUG
		ds2 = Msgs::gSerdeInfo.MakeDataShared(client2->scene);
#endif
	}

	if (client3) {
		client3->Update();
#ifndef NDEDBUG
		ds3 = Msgs::gSerdeInfo.MakeDataShared(client3->scene);
#endif
	}

	if (client4) {
		client4->Update();
#ifndef NDEDBUG
		ds4 = Msgs::gSerdeInfo.MakeDataShared(client4->scene);
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
	if (server && client2 && client2->Joined() && client2->recvs.Empty()) {
		if (ds.GetLen() != ds2.GetLen()) {
			xx::CoutTN("ds.GetLen() != ds2.GetLen() ", ds.GetLen(), " ", ds2.GetLen());
		} else {
			auto n = memcmp(ds.GetBuf(), ds2.GetBuf(), ds2.GetLen());
			if (n) {
				xx::CoutTN("memcmp(ds.GetBuf(), ds2.GetBuf(), ds2.GetLen()) = ", n);
			}
		}
	}
	if (server && client3 && client3->Joined() && client3->recvs.Empty()) {
		if (ds.GetLen() != ds3.GetLen()) {
			xx::CoutTN("ds.GetLen() != ds3.GetLen() ", ds.GetLen(), " ", ds3.GetLen());
		} else {
			auto n = memcmp(ds.GetBuf(), ds3.GetBuf(), ds3.GetLen());
			if (n) {
				xx::CoutTN("memcmp(ds.GetBuf(), ds3.GetBuf(), ds3.GetLen()) = ", n);
			}
		}
	}
	if (server && client4 && client4->Joined() && client4->recvs.Empty()) {
		if (ds.GetLen() != ds4.GetLen()) {
			xx::CoutTN("ds.GetLen() != ds4.GetLen() ", ds.GetLen(), " ", ds4.GetLen());
		} else {
			auto n = memcmp(ds.GetBuf(), ds4.GetBuf(), ds4.GetLen());
			if (n) {
				xx::CoutTN("memcmp(ds.GetBuf(), ds4.GetBuf(), ds4.GetLen()) = ", n);
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
	if (client2) {
		client2->Draw();
	}
	if (client3) {
		client3->Draw();
	}
	if (client4) {
		client4->Draw();
	}

	xx::LineStrip().FillBoxPoints({}, {2, height}).SetColor(xx::RGBA8_Red).Draw();
	xx::LineStrip().FillBoxPoints({}, {width, 2}).SetColor(xx::RGBA8_Red).Draw();

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
