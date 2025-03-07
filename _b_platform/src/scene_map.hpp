#pragma once

namespace Map {

	inline void Scene::InitMap(std::u32string_view mapText) {
		// '　'				space
		// 'Ｂ'				block
		// 'ｃ'				begin pos( character ? )
		// 'ｅ'				end pos
		assert(mapText.size());

		// auto remove head empty line
		while (mapText[0] == U'\n' || mapText[0] == U'\r') {
			mapText = mapText.substr(1);
		}

		// auto remove last empty line
		while (mapText.back() == U'\n' || mapText.back() == U'\r') {
			mapText = mapText.substr(0, mapText.size() - 1);
		}

		// scan first line length
		int32_t width{};
		for (; width < mapText.size(); ++width) {
			if (mapText[width] == U'\n' || mapText[width] == U'\r') break;
		}

		// scan line count
		int32_t height{ 1 };
		for (auto& c : mapText) {
			if (c == U'\n') ++height;
		}

		map.Init({ width, height });

		// fill map contents
		int32_t x{}, y{};
		for (auto c : mapText) {
			switch (c) {
			case U'\r': continue;
			case U'\n':
				assert(x == width);
				x = 0;
				++y;
				continue;
			case U'ｃ':
				beginPos = { x, y };
				goto LabDefault;
			case U'ｅ':
				endPos = { x, y };
				goto LabDefault;
			case U'Ｂ': {
				map[x, y].type = MapItemTypes::Block;
				break;
			}
			default: {
			LabDefault:
				map[x, y].type = MapItemTypes::Space;
			}
			}
			++x;
		}

		for (int32_t y = 0; y < height - 1; ++y) {
			for (int32_t x = 0; x < width; ++x) {
				map[x, y].bottomNeighborType = map[x, y + 1].type;
			}
		}
	}

	inline void Scene::InitSpaceGroups() {
		auto width = map.size.x;
		auto height = map.size.y;
		for (int32_t y = 0; y < height; ++y) {
			for (int32_t x = 0; x < width; ++x) {
				auto& mi = map[x, y];
				if (mi.Walkable()) {
					SpaceGroup* g{};
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
				asg.InitCell(x, y, map[ x, y ].type == MapItemTypes::Space);
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
		std::u32string s;

		auto width = map.size.x;
		auto height = map.size.y;
		for (int32_t y{}; y < height; ++y) {
			for (int32_t x{}; x < width; ++x) {
				switch (map.At(x, y)->type) {
				case MapItemTypes::Space:
					s.push_back(U'　');
					break;
				case MapItemTypes::Block:
					s.push_back(U'Ｂ');
					break;
				}
			}
			s.push_back(U'\n');
		}

		s[beginPos.y * (width + 1) + beginPos.x] = U'ｃ';
		s[endPos.y * (width + 1) + endPos.x] = U'ｅ';

		xx::CoutN(s);
	}

	inline void Scene::Init() {
		InitMap(UR"(
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
)");

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
