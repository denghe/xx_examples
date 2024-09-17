#pragma once

namespace Battle {

	inline void Scene::NewGame() {
		// todo: cleanup
		// blocks.Clear();
		explosions.Clear();
		bladeLights.Clear();
		effectTextManager.ens.Clear();
		monsters.Clear();

		mainLogic = MainLogic();
	}

	inline xx::Task<> Scene::MainLogic() {

		// generate some monsters
		XY p;
		for (int i = 0; i < 1000; ++i) {
			p.x = rnd.Next<float>(Cfg::mapEdgeMin.x + Cfg::maxItemSize_2, Cfg::mapEdgeMax.x - Cfg::maxItemSize_2);
			p.y = rnd.Next<float>(Cfg::mapEdgeMin.y + Cfg::maxItemSize_2, Cfg::mapEdgeMax.y - Cfg::maxItemSize_2);
			monsters.EmplaceInit(p);
		}

		while (true) {
			// todo
			co_yield 0;
		}
	}

	inline void Scene::MakeBlade(Monster* caster, XY const& shootPos, float radians, float radius, int32_t damage) {
		assert(caster);

		// make effect
		bladeLights.Emplace().Init(shootPos, radians, radius / BladeLight::cRadius);
		
		// hit
		monsters.Foreach9All<true>(shootPos.x, shootPos.y, [&](Monster& m)->xx::ForeachResult {
			auto d = m.pos - shootPos;
			auto r = m.radius + BladeLight::cRadius;
			auto mag2 = d.x * d.x + d.y * d.y;
			if (mag2 <= r * r) {	// cross
				if (caster->Hurt(m)) {	// dead
					return xx::ForeachResult::RemoveAndContinue;
				}
			}
			return xx::ForeachResult::Continue;
		}, caster);
	}

};
