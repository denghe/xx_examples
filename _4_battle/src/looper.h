﻿#pragma once
#include "pch.h"
#include "cfg.h"
#include "res_frames.h"

namespace Battle {
	struct Scene;
}

struct Looper : xx::Engine<Looper>, Cfg {

	// all picture resource here
	ResFrames res;

	// button config
	xx::Scale9SpriteConfig s9cfg_btn;
	xx::Shared<xx::Node> ui;

	xx::Camera camera;
	xx::Shared<Battle::Scene> scene;

	// true: loading finished
	bool ok{};

	xx::Task<> MainTask();		// loading logic
	void BeforeUpdate();
	void Update();			// fixed update
	void Draw();
};

extern Looper gLooper;
extern ResFrames& gRes;