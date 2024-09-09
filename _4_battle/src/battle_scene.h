﻿#pragma once

namespace Battle {

	struct Scene {
		int32_t frameNumber{};
		int32_t autoId{};
		int32_t genSpeed{1};
		XY screenAreaMin{}, screenAreaMax{};
		xx::BlockLink<Explosion> explosions;
		xx::BlockLink<BladeLight> bladeLights;
		xx::SpaceGrid<Monster> monsters;
		xx::SpaceRingDiffuseData srdd;
		xx::Rnd rnd;
		xx::Task<> monsterEmitter;
		// todo: wall ?

		void Init();
		void BeforeUpdate();
		int32_t Update();
		void Draw();

		// todo: make xxx ?
	};

}
