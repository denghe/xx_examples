#pragma once

namespace Battle {

	inline Item* Scene::SearchItem(XY const& pos, float maxDistance) {
		if (auto e = items.len; e < 20) {
			Item* rtv{};
			float maxV{};
			for (int32_t i = 0; i < e; ++i) {
				auto item = items[i].pointer;
				auto vx = item->pos.x - pos.x;
				auto vy = item->pos.y - pos.y;
				auto dd = vx * vx + vy * vy;
				auto r = maxDistance + item->radius;
				auto v = r * r - dd;
				if (v > maxV) {
					rtv = item;
					maxV = v;
				}
			}
			return rtv;
		} else return itemsSG.FindNearestByRange(gScene->srdd, pos.x, pos.y, maxDistance);
	}

	inline void Scene::NewGame() {
		// cleanup
		// blocks.Clear();	// no change
		explosions.Clear();
		bladeLights.Clear();
		effectTextManager.ens.Clear();
		monsters.Clear();
		projectiles.Clear();
		itemsSG.Clear();
		items.Clear();

		mainLogic = MainLogic();
	}

	inline xx::Task<> Scene::MainLogic() {

		// generate some items on the ground

		{
			auto& item = *items.Emplace().Emplace<Item_Staff1>();
			item.pos = { 1920 - 300, 1080 - 300 };
			item.Init(nullptr);
			itemsSG.Add(&item);
		}
		{
			auto& item = *items.Emplace().Emplace<Item_Sword1>();
			item.pos = { 1920 + 300, 1080 - 300 };
			item.Init(nullptr);
			itemsSG.Add(&item);
		}
		{
			auto& item = *items.Emplace().Emplace<Item_Staff1>();
			item.pos = { 1920 + 300, 1080 + 300 };
			item.Init(nullptr);
			itemsSG.Add(&item);
		}
		{
			auto& item = *items.Emplace().Emplace<Item_Sword1>();
			item.pos = { 1920 - 300, 1080 + 300 };
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
