#pragma once

namespace Battle {

	struct Scene {
		int32_t frameNumber{};
		int32_t autoId{};
		int32_t genSpeed{ 10 };
		xx::SpaceABGrid<Block> blocks;
		xx::BlockLink<Explosion> explosions;
		xx::BlockLink<BladeLight> bladeLights;
		EffectTextManager effectTextManager;
		xx::SpaceGrid<Monster> monsters;
		xx::SpaceRingDiffuseData srdd;
		xx::Rnd rnd;
		xx::Task<> monsterEmitter;

		void Init();
		int32_t Update();
		void Draw();

		void StunAll();
		bool TryMakeMonster(XY const& pos);
		// ...
	};

}
