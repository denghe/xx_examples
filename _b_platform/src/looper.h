#pragma once
#include "pch.h"
#include "res_tp_frames.h"
#include "scene.h"

struct Looper : xx::Engine<Looper>, xx::GDesign<1920, 1080, 60> {

	ResTpFrames res;
	xx::Scale9SpriteConfig btnCfg;
	xx::Shared<xx::Node> ui;
	xx::Task<> MainTask();							// loading logic
	xx::Camera camera;
	bool ok{};										// true: loading finished

	xx::Shared<Scene> scene;

	void Update();
	void Draw();
};

extern Looper gLooper;
extern ResTpFrames& gRes;					// gLooper.res
