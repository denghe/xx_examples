#pragma once

namespace Battle {

	inline int32_t Scene::Update() {
		time += gLooper.frameDelay;

		// update monsters
		monsters.Foreach([&](Monster& o)->xx::ForeachResult {
			auto r = o.Update();
			if (r == -1) return xx::ForeachResult::RemoveAndContinue;
			else if (r == 1) {
				monsters.Update(o);
			}
			return xx::ForeachResult::Continue;
		});

		// make some monsters
		mainLogic();

		// update blade light
		bladeLights.ForeachFlags([&](BladeLight& o)->xx::ForeachResult {
			return o.Update() ? xx::ForeachResult::RemoveAndContinue : xx::ForeachResult::Continue;
		});

		// update explosions
		explosions.ForeachFlags([&](Explosion& o)->xx::ForeachResult {
			return o.Update() ? xx::ForeachResult::RemoveAndContinue : xx::ForeachResult::Continue;
		});

		// update effect text
		effectTextManager.Update();

		return 0;
	}

};
