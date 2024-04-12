#pragma once
#include "pch.h"
#include "res_tp_frames.h"

struct Looper : xx::Engine<Looper>, xx::GDesign<1280, 800> {

	// all picture resource here
	ResTpFrames res;

	xx::Camera camera;

	xx::SpaceRingDiffuseData sgrdd;	// for grid.ForeachByRange

	// true: loading finished
	bool ok{};

	xx::Task<> MainTask();		// loading logic
	void BeforeUpdate();
	void Update();			// fixed update
	void Draw();
};

extern Looper gLooper;
extern ResTpFrames& gRes;
