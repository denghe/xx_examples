#include "pch.h"
#include "looper.h"

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
	gLooper.title = "xx_examples_1_bugs";
	gLooper.Init();
	gLooper.Run<false>();
}
#endif


Task<> Looper::MainTask() {
	co_await res.AsyncLoad("res/", 128);

	s9cfg_btn.frame = res.button;
	s9cfg_btn.texScale = { 2, 2 };
	s9cfg_btn.center = { 2, 3, 2, 2 };
	s9cfg_btn.color = { 0x5f, 0x15, 0xd9, 0xff };

	ui.Emplace()->Init();

	ui->MakeChildren<Button>()->Init(1, xy7m, xy7a, s9cfg_btn, U"clear", [&]() {
		grid.Clear();
		});

	ui->MakeChildren<Button>()->Init(1, xy4m + XY{ 0, 100 }, xy4a, s9cfg_btn, U"+1", [&]() {
		CreateBug(mapSize_2, rnd.Next<int32_t>(10, 200));
		});

	ui->MakeChildren<Button>()->Init(1, xy4m, xy4a, s9cfg_btn, U"+100", [&]() {
		for (int32_t i = 0; i < 100; i++) {
			CreateBug(mapSize_2, rnd.Next<int32_t>(10, 200));
		}
		});

	ui->MakeChildren<Button>()->Init(1, xy4m - XY{ 0, 100 }, xy4a, s9cfg_btn, U"+1000", [&]() {
		for (int32_t i = 0; i < 1000; i++) {
			CreateBug(mapSize_2, rnd.Next<int32_t>(10, 200));
		}
		});

	ui->MakeChildren<Button>()->Init(1, xy2m - XY{ 5,0 }, { 1,0 }, s9cfg_btn, U"zoom in", [&]() {
		camera.IncreaseScale(0.1f, 5);
		});

	ui->MakeChildren<Button>()->Init(1, xy2m + XY{ 5,0 }, { 0,0 }, s9cfg_btn, U"zoom out", [&]() {
		camera.DecreaseScale(0.1f, 0.1f);
		});


	camera.SetMaxFrameSize(maxItemSize);
	camera.SetOriginal(mapSize_2);
	camera.SetScale(1.f);

	sgrdd.Init(128, 32);
	grid.Init(physNumRows, physNumCols, physCellSize);

	mouseHitRange = 50;

	ok = true;
}


void Looper::CreateBug(XY const& headPos, int32_t len) {
	auto& h = grid.EmplaceInit(headPos, SpaceWeak<BugBody>{}, SpaceWeak<BugBody>{}, false);
	h.radians = -gPI * 0.5f;
	SpaceWeak<BugBody> hgw(h);
	auto gw = hgw;
	for (int32_t i = 0; i < len; i++) {
		auto& o = gw();
		gw = grid.EmplaceInit(o.pos + XY{ 0, o.cDistance }, hgw, gw);
	}
	auto& o = gw();
	grid.EmplaceInit(o.pos + XY{ 0, o.cDistance }, hgw, gw, true);
}


void Looper::MouseHit() {
	auto p = camera.ToLogicPos(mouse.pos);
	grid.ForeachByRange(gLooper.sgrdd, p.x, p.y, mouseHitRange, [&](BugBody& o) {
		auto d = o.pos - p;
		auto mag2 = d.x * d.x + d.y * d.y;
		auto r = mouseHitRange + o.cRadius;
		if (mag2 < r * r) {
			if (o.prev) {
				o.prev().isTail = true;
			}
			grid.Remove(o);
		}
	});
}


void Looper::BeforeUpdate() {
	if (!ok) return;

	// scale control
	if (gLooper.KeyDownDelay(KeyboardKeys::Z, 0.02f)) {
		camera.IncreaseScale(0.1f, 5);
	} else if (gLooper.KeyDownDelay(KeyboardKeys::X, 0.02f)) {
		camera.DecreaseScale(0.1f, 0.1f);
	}

	// resize mouse hit area size
	if (gLooper.KeyDownDelay(KeyboardKeys::D, 0.01f)) {
		if (mouseHitRange > mouseHitRadius.from) mouseHitRange -= 5;
	} else if (gLooper.KeyDownDelay(KeyboardKeys::A, 0.01f)) {
		if (mouseHitRange < mouseHitRadius.to) mouseHitRange += 5;
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
		if (mbs && Calc::DistancePow2(lastMousePos, m.pos) > 16) {		// mouse down + moved == dragging
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

	if (!gLooper.mouseEventHandler && mouse.PressedMBLeft()) {
		MouseHit();
	}

	// move logic
	grid.Foreach([](BugBody& o)->void {
		o.Update();
		});
}


void Looper::Draw() {
	if (!ok) return;

	// draw all ( known issue: maybe need sort by z )
	grid.Foreach([](BugBody& o)->void {
		o.Draw();
		});

	// draw mouse hit area
	LineStrip().FillCirclePoints({}, mouseHitRange, {}, 100, camera.scale)
		.SetPosition(gLooper.mouse.pos)
		.Draw();

	// draw tips
	gLooper.ctcDefault.Draw({ 0, gLooper.windowSize_2.y - 5 }, "zoom: Z / X   resize hit area: A / D    move map: mouse right button   hit bug: mouse left button", RGBA8_Green, { 0.5f, 1 });

	auto str = ToString("total item count = ", grid.Count());
	gLooper.ctcDefault.Draw({ 0, gLooper.windowSize_2.y - 50 }, str, RGBA8_Green, { 0.5f, 1 });

	// draw ui
	gLooper.DrawNode(ui);
}
