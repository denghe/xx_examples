#pragma once

namespace Battle {

	inline void Scene::Draw() {
		
		// todo: some content draw order by pos.y

		blocks.Foreach([&](Block& o)->void {
			o.Draw();
		});

		monsters.Foreach([&](Monster& o)->void {
			o.Draw();
			o.DrawBars();
		});

		for (int32_t i = 0, e = items.len; i < e; ++i) {
			items[i]->Draw();
		}

		for (int32_t i = 0, e = projectiles.len; i < e; ++i) {
			projectiles[i]->Draw();
		}

		bladeLights.ForeachFlags([&](BladeLight& o)->void {
			o.Draw();
		});

		explosions.ForeachFlags([&](Explosion& o)->void {
			o.Draw();
		});

		effectTextManager.Draw();

		// ...

	}

};
