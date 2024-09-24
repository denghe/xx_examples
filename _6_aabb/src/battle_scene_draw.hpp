#pragma once

namespace Battle {

	inline void Scene::Draw() {
		
		blocks.Foreach([&](Block& o)->void {
			o.Draw();
		});

#if 1
		monsters.Foreach([&](Monster& o)->void {
			o.Draw();
		});
#else

		{
			int32_t rowFrom, rowTo, colFrom, colTo;
			gLooper.camera.FillRowColIdxRange<1, 3, 1, 2>(gLooper.physNumRows, gLooper.physNumCols, gLooper.physCellSize,
				rowFrom, rowTo, colFrom, colTo);

			for (int32_t rowIdx = rowFrom; rowIdx < rowTo; ++rowIdx) {
				for (int32_t colIdx = colFrom; colIdx < colTo; ++colIdx) {
					auto cidx = monsters.CrIdxToCIdx({ colIdx, rowIdx });
					monsters.ForeachCell(cidx, [](Monster& o) {
						o.Draw();
					});
				}
			}
		}

#endif

		// ...

	}

};
