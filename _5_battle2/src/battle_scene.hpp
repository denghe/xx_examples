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

	inline void Scene::NewGame(int32_t count) {
		// cleanup
		// blocks.Clear();	// no change
		explosions.Clear();
		bladeLights.Clear();
		effectTextManager.ens.Clear();
		monsters.Clear();
		projectiles.Clear();
		itemsSG.Clear();
		items.Clear();

		mainLogic = MainLogic(count);
	}

	inline void Scene::DropItem(ItemTypes t, XY pos) {
		if (pos.IsZeroSimple()) {
			pos.x = gLooper.rnd.Next<float>(gLooper.mapEdgeMin.x + 32, gLooper.mapEdgeMax.x - 32);
			pos.y = gLooper.rnd.Next<float>(gLooper.mapEdgeMin.y + 32, gLooper.mapEdgeMax.y - 32);
		}

		switch (t) {
		case ItemTypes::Staff1: {
			auto& item = *items.Emplace().Emplace<Item_Staff1>();
			item.pos = pos;
			item.Init(nullptr);
			itemsSG.Add(&item);
			break;
		}
		case ItemTypes::Sword1: {
			auto& item = *items.Emplace().Emplace<Item_Sword1>();
			item.pos = pos;
			item.Init(nullptr);
			itemsSG.Add(&item);
			break;
		}
		}
		// ...
	}

	inline xx::Task<> Scene::MainLogic(int32_t count) {

		// generate some items on the ground
		DropItem(ItemTypes::Staff1, { gLooper.mapSize_2.x - 300, gLooper.mapSize_2.y - 300 });
		DropItem(ItemTypes::Sword1, { gLooper.mapSize_2.x + 300, gLooper.mapSize_2.y - 300 });
		DropItem(ItemTypes::Staff1, { gLooper.mapSize_2.x + 300, gLooper.mapSize_2.y + 300 });
		DropItem(ItemTypes::Sword1, { gLooper.mapSize_2.x - 300, gLooper.mapSize_2.y + 300 });

		// generate some monsters
		XY p;
		for (int i = 0; i < count; ++i) {
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
