#pragma once
#include "pch.h"
#include "res_tp_frames.h"
#include "xx_sound.h"

struct Looper : xx::Engine<Looper>, xx::GDesign<1920, 1080, 60> {

	ResTpFrames res;
	xx::Scale9SpriteConfig btnCfg;
	xx::Shared<xx::Node> ui;
	xx::Task<> MainTask();							// loading logic
	xx::Camera camera;
	xx::Sound sound, sound2;						// sound groups
	xx::Listi32<xx::Shared<xx::SoundSource>> sss;	// sound resources
	std::optional<xx::Task<>> soundTask;			// random playback simulate
	xx::Task<> SoundTask();
	bool paused{};
	bool ok{};										// true: loading finished

	void Draw();
};

extern Looper gLooper;
extern ResTpFrames& gRes;					// gLooper.res
