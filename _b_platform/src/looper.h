#pragma once
#include "pch.h"
#include "res_tp_frames.h"
struct Cfg : xx::GDesign<1920, 1080, 120> {};
//#include "scene_float.h"
//#include "scene_int.h"
#include "scene_int2.h"
#include "scene_cd.h"
#include "scene_cd2.h"

struct Looper : xx::Engine<Looper>, Cfg {

	ResTpFrames res;
	xx::Scale9SpriteConfig btnCfg;
	xx::Shared<xx::Node> ui;
	xx::Task<> MainTask();					// loading logic
	xx::Camera camera;

	void Draw();
};

extern Looper gLooper;
extern ResTpFrames& gRes;					// gLooper.res
