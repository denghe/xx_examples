#pragma once

namespace Map {



	inline void Scene::InitSpaceGroups() {
		auto width = map.size.x;
		auto height = map.size.y;
		for (int32_t y = 0; y < height; ++y) {
			for (int32_t x = 0; x < width; ++x) {
				auto& mi = map[x, y];
				if (mi.Walkable()) {
					xx::MapCellGroup* g{};
					if (x > 0 && map[x - 1, y].Walkable()) {
						// same group with left neighbor
						mi.groupId = map[x - 1, y].groupId;
						g = &spaceGroups[mi.groupId];
						g->mapX.to = x;
					}
					else {
						// first element
						mi.groupId = spaceGroups.len;
						g = &spaceGroups.Emplace();
						g->mapY = y;
						g->mapX.from = g->mapX.to = x;
						g->leftEdgeCanFall = x > 0
							? map[x - 1, y].Fallable()
							: false;
					}
					g->rightEdgeCanFall = x + 1 < width
						? map[x + 1, y].Fallable()
						: false;
				}
			}
		}
	}

	inline void Scene::InitAStarGrid() {
		auto width = map.size.x;
		auto height = map.size.y;
		asg.Init(width, height);

		// generate cells
		for (int32_t y = 0; y < height; ++y) {
			for (int32_t x = 0; x < width; ++x) {
				asg.InitCell(x, y, map[ x, y ].type == xx::MapCellTypes::Space);
			}
		}

		// todo: custom neighbors support
		// fill neighbors
		xx::Listi32<XYi> nos;
		for (int32_t y = 0; y < height; ++y) {
			for (int32_t x = 0; x < width; ++x) {
				// keyboard number area style
				// 7 8 9
				// 4 5 6
				// 1 2 3

				// [???]
				// [?.?]
				// [???]
				if (map.IsSpaceAt(x, y)) {
					auto c1 = map.IsSpaceAt(x - 1, y + 1);
					auto c2 = map.IsSpaceAt(x, y + 1);
					auto c3 = map.IsSpaceAt(x + 1, y + 1);
					auto c4 = map.IsSpaceAt(x - 1, y);
					auto c6 = map.IsSpaceAt(x + 1, y);
					auto c7 = map.IsSpaceAt(x - 1, y - 1);
					auto c8 = map.IsSpaceAt(x, y - 1);
					auto c9 = map.IsSpaceAt(x + 1, y - 1);

#if 1
					xx::CoutN(x, ", ", y, '\n'
						, c7 ? '.' : 'B', c8 ? '.' : 'B', c9 ? '.' : 'B', '\n'
						, c4 ? '.' : 'B', 'c', c6 ? '.' : 'B', '\n'
						, c1 ? '.' : 'B', c2 ? '.' : 'B', c3 ? '.' : 'B'
						, '\n'
					);
#endif

					if (c2) continue;

					// handle 7 and 9
					if (c8)
					{
						// can to 7
						// [..?]
						// [Bc?]
						// [?B?]
						if (!c4 && c7) {
							nos.Emplace(-1, -1);
						}

						// can to 9
						// [?..]
						// [?cB]
						// [?B?]
						if (!c6 && c9) {
							nos.Emplace(1, -1);
						}
					}

					// handle 4
					// [???]
					// [.c?]
					// [BB?]
					if (c4 && !c1) {
						nos.Emplace(-1, 0);
					}

					// handle 6
					// [???]
					// [?c.]
					// [?BB]
					if (c6 && !c3) {
						nos.Emplace(1, 0);
					}

					// handle falling 1
					// [???]
					// [.c?]
					// [.B?]
					if (c4 && c1) {
						if (auto fy = map.FindFallingY(x - 1, y); fy.has_value()) {
							nos.Emplace(-1, *fy - y);
						}
					}

					// handle falling 3
					// [???]
					// [?c.]
					// [?B.]
					if (c6 && c3) {
						if (auto fy = map.FindFallingY(x + 1, y); fy.has_value()) {
							nos.Emplace(1, *fy - y);
						}
					}

					auto c = asg.At(x, y);
					assert(c->walkable);
					asg.InitCellNeighbors(c, nos.buf, nos.len);
					nos.Clear();
				}
			}
		}

	}

	inline void Scene::FillNavTips() {
		for (auto n = spaceGroups.len, i = 0; i < n; ++i) {

			auto beginGroup = &spaceGroups[i];
			XYi beginPos{ beginGroup->mapX.from, beginGroup->mapY };
			beginGroup->navTips.Resize(n);

			for (int32_t j = 0; j < n; ++j) {
				if (i == j) {
					beginGroup->navTips[j] = j;
					continue;
				}
				auto endGroup = &spaceGroups[j];
				XYi endPos{ endGroup->mapX.from, endGroup->mapY };
				assert(beginPos != endPos);

				if (asg.Search(beginPos, endPos)) {
					auto& path = asg.path;
					assert(path.len);
					// find first diff group from asg.path & fill to navTips
					for (int32_t k = path.len - 1; k >= 0; --k) {
						auto c = path[k];
						auto gi = map[c->x, c->y].groupId;
						if (i != gi) {
							beginGroup->navTips[j] = gi;
							break;
						}
					}
				}
				else {
					beginGroup->navTips[j] = -1;
				}
				asg.Cleanup();
			}
		}
	}

	inline void Scene::DumpMap() {
		auto s = map.Dump();
		s[beginPos.y * (map.size.x + 1) + beginPos.x] = U'ｃ';
		s[endPos.y * (map.size.x + 1) + endPos.x] = U'ｅ';
		xx::CoutN(s);
	}

	inline void Scene::Init() {
		map.InitByText(UR"(
ＢＢＢＢＢＢＢＢＢＢＢＢ
Ｂ　　　　　　　　　　Ｂ
Ｂ　　　Ｂ　Ｂ　　　　Ｂ
Ｂｃ　ＢＢＢＢ　　　　Ｂ
ＢＢＢＢＢＢＢＢ　　　Ｂ
ＢＢＢＢＢＢＢＢＢＢ　Ｂ
Ｂ　　　　　　　　　　Ｂ
Ｂ　　　　　ＢＢＢＢＢＢ
Ｂ　　　　　　ＢＢＢＢＢ
Ｂ　ｅ　　　　　　　　Ｂ
ＢＢＢＢＢＢＢＢＢＢＢＢ
)"
, [&](auto x, auto y) { beginPos = { x, y }; }
, [&](auto x, auto y) { endPos = { x, y }; }
);
		DumpMap();
		InitSpaceGroups();
		InitAStarGrid();
		FillNavTips();
	}

	inline void Scene::Update() {
	}

	inline void Scene::Draw() {
		gLooper.ctcDefault.Draw({ 0, gLooper.windowSize_2.y - 5 }, "(map) show console", xx::RGBA8_Green, { 0.5f, 1 });
	}

}
