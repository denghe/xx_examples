﻿#pragma once

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

		// generate some items on the ground

		{
			auto& item = *items.Emplace().Emplace<Item_Staff1>();
			item.pos = { 1920 - 100, 1080 };
			item.Init(nullptr);
			itemsSG.Add(&item);
		}
		{
			auto& item = *items.Emplace().Emplace<Item_Sword1>();
			item.pos = { 1920 + 100, 1080 };
			item.Init(nullptr);
			itemsSG.Add(&item);
		}


		// generate some monsters
		XY p;
		for (int i = 0; i < 1000; ++i) {
			p.x = rnd.Next<float>(gLooper.mapEdgeMin.x + gLooper.maxItemSize_2, gLooper.mapEdgeMax.x - gLooper.maxItemSize_2);
			p.y = rnd.Next<float>(gLooper.mapEdgeMin.y + gLooper.maxItemSize_2, gLooper.mapEdgeMax.y - gLooper.maxItemSize_2);
			monsters.EmplaceInit(p);
		}

		//monsters.EmplaceInit(XY{ 1920 - 500, 1080 });
		//monsters.EmplaceInit(XY{ 1920 + 500, 1080 });

		while (true) {
			// todo
			co_yield 0;
		}
	}

};
