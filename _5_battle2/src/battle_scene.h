﻿#pragma once

namespace Battle {

	struct Scene {
		int32_t frameNumber{};
		int32_t autoId{};
		xx::SpaceABGrid<Block> blocks;
		xx::BlockLink<Explosion> explosions;
		xx::BlockLink<BladeLight> bladeLights;
		EffectTextManager effectTextManager;
		xx::SpaceGrid<Monster> monsters;
		xx::SpaceRingDiffuseData srdd;
		xx::Rnd rnd;

		void Init();
		int32_t Update();
		void Draw();

		xx::Task<> mainLogic;
		xx::Task<> MainLogic();
		void NewGame();
		// ...
	};

}