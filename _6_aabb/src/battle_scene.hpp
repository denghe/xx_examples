#pragma once

namespace Battle {

	inline void Scene::GenMonsters(XY const& pos, int32_t count) {
		for (int i = 0; i < count; ++i) {
			monsters.EmplaceInit(pos);
		}
	}
};
