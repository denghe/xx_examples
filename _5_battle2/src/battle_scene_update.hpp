#pragma once

namespace Battle {

	inline int32_t Scene::Update() {
		time += gLooper.frameDelay;

		if (!gLooper.mouseEventHandler && gLooper.mouse.PressedMBLeft()) {
			auto p = gLooper.camera.ToLogicPos(gLooper.mouse.pos);

			// todo: search nearest and fire ?

			projectiles.Emplace().Emplace<Projectile_Fireball>()->Init({}, p, 0, 32, 300, 2, 1);
		}

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

		// update projectiles
		for (int32_t i = projectiles.len - 1; i >= 0; --i) {
			if (projectiles[i]->Update()) {
				projectiles.SwapRemoveAt(i);
			}
		}

		return 0;
	}

};
