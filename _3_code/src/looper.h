#pragma once
#include "pch.h"
#include "cfg.h"
#include "res_frames.h"
#include "code.h"
#include "code2.h"
#include "code3.h"
#include "drop.h"

struct Looper : xx::Engine<Looper>, Cfg {

	// all picture resource here
	ResFrames res;

	// button config
	xx::Scale9SpriteConfig s9cfg_btn;
	xx::Shared<xx::Node> ui;

	xx::Camera camera;


	// true: loading finished
	bool ok{};

	xx::Task<> MainTask();		// loading logic
	void BeforeUpdate();
	void Update();			// fixed update
	void Draw();
};

extern Looper gLooper;
extern ResFrames& gRes;
