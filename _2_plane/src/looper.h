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

	xx::Camera camera;
	ResTpFrames res;
	xx::Ref<xx::TMX::Map> tiledMap;
	xx::XYi mapNumRowsCols{};
	xx::XY mapSize{}, mapSize_2{};
	xx::TMX::Layer_Tile* layerBG{}, *layerTrees{};

	xx::SpaceRingDiffuseData srdd;		// for sg.ForeachByRange
	xx::SpaceGrid<Monster> monsters;
	xx::SpaceABGrid<Tree> trees;
	xx::BlockLink<Bullet> bullets;
	xx::BlockLink<Explosion> explosions;
	EffectNumberManager enm;
	xx::Shared<Plane> plane;

	typedef void(*DrawFunc)(void*);
	xx::Listi32<std::tuple<float, DrawFunc, void*>> yDraws;

	Monster* FindNeighborMonster(xx::XY const& pos, float radius);

	bool ok{};							// true: loading finished
	xx::Task<> MainTask();				// loading logic

	void BeforeUpdate();
	void Update();						// fixed update
	void Draw();
};

extern Looper gLooper;
extern ResTpFrames& gRes;
