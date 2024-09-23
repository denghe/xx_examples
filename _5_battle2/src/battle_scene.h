#pragma once

namespace Battle {

	struct Scene {
		float time{};		// replace gScene->time for speedup
		xx::Rnd rnd;
		xx::SpaceGridRingDiffuseData srdd;
		xx::SpaceGridAB<Block> blocks;
		xx::BlockLink<Explosion> explosions;
		xx::BlockLink<BladeLight> bladeLights;
		EffectTextManager effectTextManager;
		xx::SpaceGrid<Monster> monsters;
		xx::Listi32<xx::Shared<Projectile>> projectiles;

		xx::Listi32<xx::Shared<Item>> items;	// on the ground
		xx::SpaceGrid2<Item> itemsSG;
		Item* SearchItem(XY const& pos, float maxDistance);

		void Init();
		int32_t Update();
		void Draw();

		xx::Task<> mainLogic;
		xx::Task<> MainLogic();
		void NewGame();
		// ...
	};

}
