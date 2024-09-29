#include "pch.h"
#include "looper.h"
#include "code.h"
#include "code2.h"
#include "code3.h"
#include "code4.h"
#include "drop.h"

Looper gLooper;
ResFrames& gRes(gLooper.res);

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
	gLooper.title = "xx_examples_3_code";
	gLooper.Init();
	gLooper.Run<false>();
}
#endif


xx::Task<> Looper::MainTask() {
	co_await res.AsyncLoad("res/", 128);

	s9cfg_btn.frame = res.button;
	s9cfg_btn.texScale = { 2, 2 };
	s9cfg_btn.center = { 2, 3, 2, 2 };
	s9cfg_btn.color = { 0x5f, 0x15, 0xd9, 0xff };

	ui.Emplace()->Init();

	// create code test button
	ui->MakeChildren<xx::Button>()->Init(1, xy7m, xy7a, s9cfg_btn, U"run code", [&]() {
		auto n = 10000000;

		for (size_t i = 0; i < 10; i++) {
			Code::Context2 c;
			c.Init(n);
			auto secs = xx::NowEpochSeconds();
			c.Run();
			xx::CoutN("n = ", n, " Context2 Run elapsed secs = ", xx::NowEpochSeconds(secs));
		}

		for (size_t i = 0; i < 10; i++) {
			Code::Context3 c;
			c.Init(n);
			auto secs = xx::NowEpochSeconds();
			c.Run();
			xx::CoutN("n = ", n, " Context3 Run elapsed secs = ", xx::NowEpochSeconds(secs));
		}

		for (size_t i = 0; i < 10; i++) {
			Code::Context4 c;
			c.Init(n);
			auto secs = xx::NowEpochSeconds();
			c.Run();
			xx::CoutN("n = ", n, " Context4 Run elapsed secs = ", xx::NowEpochSeconds(secs));
		}

		for (size_t i = 0; i < 10; i++) {
			Code2::Context c;
			c.Init(n);
			auto secs = xx::NowEpochSeconds();
			c.Run();
			xx::CoutN("n = ", n, " Code2::Context Run elapsed secs = ", xx::NowEpochSeconds(secs));
		}

		for (size_t i = 0; i < 10; i++) {
			Code::Context2 c;
			auto secs = xx::NowEpochSeconds();
			c.Run2(n);
			xx::CoutN("n = ", n, " Context2 Run2 elapsed secs = ", xx::NowEpochSeconds(secs));
		}

	});

	ui->MakeChildren<xx::Button>()->Init(1, xy7m + XY{0, -50}, xy7a, s9cfg_btn, U"run drop test", [&]() {
		Drop::Test1(rnd, 10000000, 200, 100, { 1, 10000 });
		Drop::Test2(rnd, 10000000, 200, 100, { 1, 10000 });
	});

	ui->MakeChildren<xx::Button>()->Init(1, xy7m + XY{0, -100}, xy7a, s9cfg_btn, U"run buff test", [&]() {
		Code3::Test();
	});

	ui->MakeChildren<xx::Button>()->Init(1, xy7m + XY{0, -150}, xy7a, s9cfg_btn, U"run code4 test", [&]() {
		Code4::Test();
	});

	camera.SetMaxFrameSize(maxItemSize);
	camera.SetOriginal(mapSize_2);
	camera.SetScale(1.f);

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

}


void Looper::Draw() {
	if (!ok) return;

	//	auto str = xx::ToString("total item count = ", grid.Count());
	//	gLooper.ctcDefault.Draw({ 0, gLooper.windowSize_2.y - 50 }, str, xx::RGBA8_Green, { 0.5f, 1 });

		// draw ui
	gLooper.DrawNode(ui);
}
