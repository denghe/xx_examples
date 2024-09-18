#pragma once

namespace Battle {

	struct Scene {
		float time{};		// do not use gScene->time for speedup
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

		void MakeBlade(Monster* caster, XY const& shootPos, float radians, float radius, int32_t damage);
	};

}
