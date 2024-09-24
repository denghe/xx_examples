#pragma once

namespace Battle {

	inline int32_t Scene::Update() {

		if (!gLooper.mouseEventHandler && gLooper.mouse.PressedMBLeft()) {
			auto p = gLooper.camera.ToLogicPos(gLooper.mouse.pos);
			GenMonsters(p, 100);
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

		return 0;
	}

};
