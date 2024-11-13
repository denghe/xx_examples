#pragma once
#include "pch.h"
#include "res_tp_frames.h"

struct Server;
struct Client;
struct Looper : xx::Engine<Looper>, xx::GDesign<1920, 1080, 60> {

	ResTpFrames res;
	xx::Scale9SpriteConfig btnCfg;
	xx::Shared<xx::Node> ui;
	xx::SpaceGridRingDiffuseData sgrdd;		// shared by grid search
	xx::Task<> MainTask();					// loading logic
	xx::Camera camera;
	bool ok{};								// true: loading finished

	// mouse states
	bool dragging{}, lastMBState{};
	XY lastMousePos{}, mouseOffset{};

	xx::Shared<Server> server;
	xx::Shared<Client> client1;

	void AfterInit();
	void BeforeUpdate();
	void Update();							// fixed update
	void Draw();
};

extern Looper gLooper;
extern ResTpFrames& gRes;					// gLooper.res
extern bool gIsServer;
