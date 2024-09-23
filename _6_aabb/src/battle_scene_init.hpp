#pragma once

namespace Battle {

	inline void Scene::Init() {
		// inits
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
		blocks.EmplaceInit(0.f, 0.f, gLooper.mapEdgeMin.x - 0.1f, gLooper.mapEdgeMin.y - 0.1f);
		// top2
		blocks.EmplaceInit(gLooper.mapEdgeMin.x, 0.f, gLooper.mapEdgeMax.x - 0.1f, gLooper.mapEdgeMin.y - 0.1f);
		// top3
		blocks.EmplaceInit(gLooper.mapEdgeMax.x, 0.f, gLooper.mapSize.x - 0.1f, gLooper.mapEdgeMin.y - 0.1f);

		// middle1
		blocks.EmplaceInit(0.f, gLooper.mapEdgeMin.y, gLooper.mapEdgeMin.x - 0.1f, gLooper.mapEdgeMax.y - 0.1f);
		// middle2
		blocks.EmplaceInit(gLooper.mapEdgeMax.x, gLooper.mapEdgeMin.y, gLooper.mapSize.x - 0.1f, gLooper.mapEdgeMax.y - 0.1f);

		// bottom1
		blocks.EmplaceInit(0.f, gLooper.mapEdgeMax.y, gLooper.mapEdgeMin.x - 0.1f, gLooper.mapSize.y - 0.1f);
		// bottom2
		blocks.EmplaceInit(gLooper.mapEdgeMin.x, gLooper.mapEdgeMax.y, gLooper.mapEdgeMax.x - 0.1f, gLooper.mapSize.y - 0.1f);
		// bottom3
		blocks.EmplaceInit(gLooper.mapEdgeMax.x, gLooper.mapEdgeMax.y, gLooper.mapSize.x - 0.1f, gLooper.mapSize.y - 0.1f);

		blocks.Foreach([&](Block& o)->void {
			o.FillWayout(this);
		});

	}

};
