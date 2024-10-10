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
	gLooper.title = "xx_examples_7_netsim";
	gLooper.Init();
	gLooper.Run<true>();	// auto sleep
}
#endif


xx::Task<> Looper::MainTask() {
	co_await res.AsyncLoad("res/");

	btnCfg.frame = res.button;
	btnCfg.texScale = { 0.99, 0.99 };
	btnCfg.center = { 2, 3, 2, 2 };
	btnCfg.color = { 0x5f, 0x15, 0xd9, 0xff };

	ui.Emplace()->Init();

	ui->MakeChildren<xx::Button>()->Init(1, xy7m + XY{ 0, 0 }, xy7a, btnCfg, U"server reset", [&]() {
		server.Emplace()->Init();
	});

	ui->MakeChildren<xx::Button>()->Init(1, xy7m + XY{ 0, -50 }, xy7a, btnCfg, U"client1 reset", [&]() {
		client1.Emplace()->Init({ -width_2 / 2, 0 });
	});

	ui->MakeChildren<xx::Button>()->Init(1, xy7m + XY{ 0, -100 }, xy7a, btnCfg, U"client1 remove", [&]() {
		client1.Reset();
	});

	ui->MakeChildren<xx::Button>()->Init(1, xy7m + XY{ 0, -150 }, xy7a, btnCfg, U"client2 reset", [&]() {
		client2.Emplace()->Init({ width_2 / 2, 0 });
	});

	ui->MakeChildren<xx::Button>()->Init(1, xy7m + XY{ 0, -200 }, xy7a, btnCfg, U"client2 remove", [&]() {
		client2.Reset();
	});

	clearColor = { 33, 33, 33, 255 };
	fb.Init();

	ok = true;
}

void Looper::AfterInit() {
	Msgs::InitSerdeInfo();
}

void Looper::BeforeUpdate() {
	if (!ok) return;
}

void Looper::Update() {
	if (!ok) return;

	xx::DataShared ds, ds1, ds2;
	if (server) {
		server->Update();
		ds = Msgs::gSerdeInfo.MakeDataShared(server->scene);
	}

	if (client1) {
		client1->Update();
		ds1 = Msgs::gSerdeInfo.MakeDataShared(client1->scene);
	}
	if (client2) {
		client2->Update();
		ds2 = Msgs::gSerdeInfo.MakeDataShared(client2->scene);
	}

	// verify data
	if (server && client1 && client1->Joined()) {
		if (ds.GetLen() != ds1.GetLen()) {
			xx::CoutTN("ds.GetLen() != ds1.GetLen() ", ds.GetLen(), " ", ds1.GetLen());
		} else {
			auto n = memcmp(ds.GetBuf(), ds1.GetBuf(), ds1.GetLen());
			if (n) {
				xx::CoutTN("memcmp(ds.GetBuf(), ds1.GetBuf(), ds1.GetLen()) = ", n);
				ds = Msgs::gSerdeInfo.MakeDataShared(server->scene);
			}
		}
	}
	if (server && client2 && client2->Joined()) {
		if (ds.GetLen() != ds2.GetLen()) {
			xx::CoutTN("ds.GetLen() != ds2.GetLen() ", ds.GetLen(), " ", ds2.GetLen());
		} else {
			auto n = memcmp(ds.GetBuf(), ds2.GetBuf(), ds2.GetLen());
			if (n) {
				xx::CoutTN("memcmp(ds.GetBuf(), ds2.GetBuf(), ds2.GetLen()) = ", n);
			}
		}
	}
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

	xx::LineStrip ls;
	ls.FillBoxPoints({}, { 2, height }).SetColor(xx::RGBA8_Red).Draw();

	DrawNode(ui);
}
