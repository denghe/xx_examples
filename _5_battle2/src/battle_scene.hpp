#pragma once

namespace Battle {

	inline void Scene::NewGame() {
		// cleanup
		// blocks.Clear();	// no change
		explosions.Clear();
		bladeLights.Clear();
		effectTextManager.ens.Clear();
		monsters.Clear();
		projectiles.Clear();

		mainLogic = MainLogic();
	}

	inline xx::Task<> Scene::MainLogic() {

		// generate some monsters
		XY p;
		for (int i = 0; i < 1000; ++i) {
			p.x = rnd.Next<float>(gLooper.mapEdgeMin.x + gLooper.maxItemSize_2, gLooper.mapEdgeMax.x - gLooper.maxItemSize_2);
			p.y = rnd.Next<float>(gLooper.mapEdgeMin.y + gLooper.maxItemSize_2, gLooper.mapEdgeMax.y - gLooper.maxItemSize_2);
			monsters.EmplaceInit(p);
		}

		while (true) {
			// todo
			co_yield 0;
		}
	}

};
