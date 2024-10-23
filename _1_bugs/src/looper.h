#pragma once
#include "pch.h"
#include "cfg.h"
#include "res_frames.h"
#include "bug.h"

struct Looper : xx::Engine<Looper>, Cfg {

	// all picture resource here
	ResFrames res;

	// button config
	xx::Scale9SpriteConfig s9cfg_btn;
	xx::Shared<xx::Node> ui;

	xx::Camera camera;

	// bug container
	xx::SpaceGrid<BugBody> grid;
	xx::SpaceGridRingDiffuseData sgrdd;	// for grid.ForeachByRange

	// mouse states
	bool dragging{}, lastMBState{};
	XY lastMousePos{}, mouseOffset{};

	float mouseHitRange{};

	// true: loading finished
	bool ok{};

	void CreateBug(XY const& headPos, int32_t len);
	void MouseHit();

	xx::Task<> MainTask();		// loading logic
	void BeforeUpdate();
	void Update();				// fixed update
	void Draw();
};

extern Looper gLooper;
extern ResFrames& gRes;
