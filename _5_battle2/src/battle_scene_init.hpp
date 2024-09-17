#pragma once

namespace Battle {

	inline void Scene::Init() {
		// inits
		effectTextManager.Init(8192);
		srdd.Init(100, gLooper.physCellSize);
		monsters.Init(gLooper.physNumRows, gLooper.physNumCols, gLooper.physCellSize);
		blocks.Init(gLooper.physNumRows, gLooper.physNumCols, gLooper.physCellSize);
		
/*
top1                  2                    3
 ┌─────────────────────────────────────────┐ 
 │                                         │ 
 │                                         │ 
 │                                         │ 
 │                                         │ 
 │                                         │ 
 │middle1                                  │2
 │                                         │ 
 │                                         │ 
 │                                         │ 
 │                                         │ 
 │                                         │ 
 └─────────────────────────────────────────┘
bottom1               2                    3
*/

		// top1
		blocks.EmplaceInit(0.f, 0.f, Cfg::mapEdgeMin.x - 0.1f, Cfg::mapEdgeMin.y - 0.1f);
		// top2
		blocks.EmplaceInit(Cfg::mapEdgeMin.x, 0.f, Cfg::mapEdgeMax.x - 0.1f, Cfg::mapEdgeMin.y - 0.1f);
		// top3
		blocks.EmplaceInit(Cfg::mapEdgeMax.x, 0.f, Cfg::mapSize.x - 0.1f, Cfg::mapEdgeMin.y - 0.1f);

		// middle1
		blocks.EmplaceInit(0.f, Cfg::mapEdgeMin.y, Cfg::mapEdgeMin.x - 0.1f, Cfg::mapEdgeMax.y - 0.1f);
		// middle2
		blocks.EmplaceInit(Cfg::mapEdgeMax.x, Cfg::mapEdgeMin.y, Cfg::mapSize.x - 0.1f, Cfg::mapEdgeMax.y - 0.1f);

		// bottom1
		blocks.EmplaceInit(0.f, Cfg::mapEdgeMax.y, Cfg::mapEdgeMin.x - 0.1f, Cfg::mapSize.y - 0.1f);
		// bottom2
		blocks.EmplaceInit(Cfg::mapEdgeMin.x, Cfg::mapEdgeMax.y, Cfg::mapEdgeMax.x - 0.1f, Cfg::mapSize.y - 0.1f);
		// bottom3
		blocks.EmplaceInit(Cfg::mapEdgeMax.x, Cfg::mapEdgeMax.y, Cfg::mapSize.x - 0.1f, Cfg::mapSize.y - 0.1f);

		blocks.Foreach([&](Block& o)->void {
			o.FillWayout(this);
		});

		mainLogic = MainLogic();
	}

};
