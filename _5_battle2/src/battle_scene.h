#pragma once

namespace Battle {

	struct Scene {
		float time{};		// replace gScene->time for speedup
		xx::Rnd rnd;
		xx::SpaceRingDiffuseData srdd;
		xx::SpaceABGrid<Block> blocks;
		xx::BlockLink<Explosion> explosions;
		xx::BlockLink<BladeLight> bladeLights;
		EffectTextManager effectTextManager;
		xx::SpaceGrid<Monster> monsters;
		xx::Listi32<xx::Shared<Projectile>> projectiles;

		void Init();
		int32_t Update();
		void Draw();

		xx::Task<> mainLogic;
		xx::Task<> MainLogic();
		void NewGame();
		// ...
	};

}
