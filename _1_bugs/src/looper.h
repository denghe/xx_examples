#pragma once
#include "pch.h"
#include "cfg.h"
#include "res_frames.h"
#include "bug.h"

struct Looper : Engine<Looper>, Cfg {

	// all picture resource here
	ResFrames res;

	// button config
	Scale9SpriteConfig s9cfg_btn;
	Shared<Node> ui;

	Camera camera;

	// bug container
	SpaceGrid<BugBody> grid;
	SpaceRingDiffuseData sgrdd;	// for grid.ForeachByRange

	// mouse states
	bool dragging{}, lastMBState{};
	XY lastMousePos{}, mouseOffset{};

	float mouseHitRange{};

	// true: loading finished
	bool ok{};

	void CreateBug(XY const& headPos, int32_t len);
	void MouseHit();

	Task<> MainTask();		// loading logic
	void BeforeUpdate();
	void Update();			// fixed update
	void Draw();
};

extern Looper gLooper;
extern ResFrames& gRes;
