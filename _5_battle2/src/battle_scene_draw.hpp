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
