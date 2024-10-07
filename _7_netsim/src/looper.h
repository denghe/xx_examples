﻿#pragma once
#include "pch.h"
#include "res_tp_frames.h"

struct Server;
struct Client;
struct Looper : xx::Engine<Looper>, xx::GDesign<1920, 1080, 60> {

	ResTpFrames res;
	xx::Scale9SpriteConfig btnCfg;
	xx::Shared<xx::Node> ui;
	xx::FrameBuffer fb;						// for draw client
	xx::Task<> MainTask();					// loading logic
	bool ok{};								// true: loading finished

	xx::Shared<Server> server;
	xx::Shared<Client> client1, client2;

	void AfterInit();
	void BeforeUpdate();
	void Update();							// fixed update
	void Draw();
};

extern Looper gLooper;
extern ResTpFrames& gRes;					// gLooper.res
extern bool gIsServer;
