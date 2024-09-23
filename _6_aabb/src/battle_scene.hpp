#pragma once

namespace Battle {

	inline void Scene::GenMonsters(XY const& pos) {
		for (int i = 0; i < 1000; ++i) {
			monsters.EmplaceInit(pos);
		}
	}
};
