#pragma once
#include "pch.h"
#include "cfg.h"
#include "res_tp_frames.h"
#include "monster.h"
#include "plane.h"
#include "bullet.h"
#include "tree.h"
#include "explosion.h"
#include "effect_number.h"
// ...

struct Looper : xx::Engine<Looper>, Cfg {

	xx::SpaceRingDiffuseData srdd;		// for sg.ForeachByRange
	ResTpFrames res;					// texture packer
	xx::TMX::Map map;					// tiled map
	xx::Camera camera;

	xx::Shared<Plane> plane;
	xx::SpaceGrid<Monster> monsters;
	xx::SpaceGrid<Tree> trees;
	xx::BlockLink<Bullet> bullets;
	xx::BlockLink<Explosion> explosions;
	EffectNumberManager enm;

	bool ok{};							// true: loading finished
	xx::Task<> MainTask();				// loading logic

	void BeforeUpdate();
	void Update();						// fixed update
	void Draw();
};

extern Looper gLooper;
extern ResTpFrames& gRes;
