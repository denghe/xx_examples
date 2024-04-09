#include "pch.h"
#include "looper.h"
#include "cfg.h"
#include "bug.h"
#include "scene.h"

void Scene::CreateBug(XY const& headPos, int len) {
	auto& h = grid.EmplaceInit(headPos, SpaceWeak<BugBody>{}, SpaceWeak<BugBody>{}, false);
	h.radians = -gPI / 2;
	SpaceWeak<BugBody> hgw(h);
	auto gw = hgw;
	for (int i = 0; i < len; i++) {
		auto& o = gw();
		gw = grid.EmplaceInit(o.pos + XY{ 0, o.cDistance }, hgw, gw);
	}
	auto& o = gw();
	grid.EmplaceInit(o.pos + XY{ 0, o.cDistance }, hgw, gw, true);
}

void Scene::Init() {
	gScene = this;

	ui.Emplace()->Init();

	ui->MakeChildren<Button>()->Init(1, gDesign.xy7m, gDesign.xy7a, gLooper.s9cfg_btn, U"clear", [&]() {
		grid.Clear();
		});

	ui->MakeChildren<Button>()->Init(1, gDesign.xy4m + XY{ 0, 100 }, gDesign.xy4a, gLooper.s9cfg_btn, U"+1", [&]() {
		CreateBug(gCfg.mapCenterPos, rnd.Next<int>(10, 30));
		});
	ui->MakeChildren<Button>()->Init(1, gDesign.xy4m, gDesign.xy4a, gLooper.s9cfg_btn, U"+100", [&]() {
		for (int i = 0; i < 100; i++) {
			CreateBug(gCfg.mapCenterPos, rnd.Next<int>(10, 30));
		}
		});
	ui->MakeChildren<Button>()->Init(1, gDesign.xy4m - XY{ 0, 100 }, gDesign.xy4a, gLooper.s9cfg_btn, U"+1000", [&]() {
		for (int i = 0; i < 1000; i++) {
			CreateBug(gCfg.mapCenterPos, rnd.Next<int>(10, 30));
		}
		});

	ui->MakeChildren<Button>()->Init(1, gDesign.xy2m - XY{ 5,0 }, { 1,0 }, gLooper.s9cfg_btn, U"zoom in", [&]() {
		camera.IncreaseScale(0.1f, 5);
		});
	ui->MakeChildren<Button>()->Init(1, gDesign.xy2m + XY{ 5,0 }, { 0,0 }, gLooper.s9cfg_btn, U"zoom out", [&]() {
		camera.DecreaseScale(0.1f, 0.1f);
		});


	camera.SetMaxFrameSize({ 50, 50 });
	camera.SetOriginal(gCfg.mapCenterPos);
	camera.SetScale(1.f);

	grid.Init(gCfg.physNumRows, gCfg.physNumCols, gCfg.physCellSize);
}

void Scene::BeforeUpdate() {
	// scale control
	if (gLooper.KeyDownDelay(KeyboardKeys::Z, 0.02f)) {
		camera.IncreaseScale(0.1f, 5);
	} else if (gLooper.KeyDownDelay(KeyboardKeys::X, 0.02f)) {
		camera.DecreaseScale(0.1f, 0.1f);
	}
	camera.Calc();
}

void Scene::Update() {
	// hit control
	auto& m = gLooper.mouse;
	if (m.btnStates[0] && !gLooper.mouseEventHandler) {
		auto p = camera.ToLogicPos(m.pos);
		grid.ForeachByRange(gLooper.sgrdd, { p.x, p.y }, gCfg.mouseHitRange, [&](BugBody& o) {
			if (o.prev) {
				o.prev().isTail = true;
			}
			grid.Remove(o);
			});
	}

	// move logic
	grid.Foreach([](BugBody& o)->void {
		o.Update();
		});
}

void Scene::Draw() {
	camera.Calc();

	// draw all
	grid.Foreach([](BugBody& o)->void {
		o.Draw();
		});


	// draw mouse hit area
	LineStrip().FillCirclePoints({}, gCfg.mouseHitRange, {}, 100, camera.scale)
		.SetPosition(gLooper.mouse.pos)
		.Draw();

	// draw tips
	gLooper.ctcDefault.Draw({ 0, gLooper.windowSize_2.y - 5 }, "zoom: Z / X   hit: mouse click", RGBA8_Green, { 0.5f, 1 });

	auto str = ToString("total item count = ", grid.Count());
	gLooper.ctcDefault.Draw({ 0, gLooper.windowSize_2.y - 50 }, str, RGBA8_Green, { 0.5f, 1 });

	// draw ui
	gLooper.DrawNode(ui);
}
