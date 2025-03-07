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
		int32_t mapWidth{};
		for (; mapWidth < mapText.size(); ++mapWidth) {
			if (mapText[mapWidth] == U'\n' || mapText[mapWidth] == U'\r') break;
		}

		// scan line count
		int32_t mapHeight{ 1 };
		for (auto& c : mapText) {
			if (c == U'\n') ++mapHeight;
		}

		map.Init({ mapWidth, mapHeight });

		// fill map contents
		int32_t x{}, y{};
		for (auto c : mapText) {
			switch (c) {
			case U'\r': continue;
			case U'\n':
				assert(x == mapWidth);
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
				map[x, y].isBlock = true;
				break;
			}
			default: {
			LabDefault:
				map[x, y].isBlock = false;
			}
			}
			++x;
		}

	}

	inline void Scene::DumpMap() {
		std::u32string s;

		auto height = map.size.y;
		auto width = map.size.x;
		for (int32_t y{}; y < height; ++y) {
			for (int32_t x{}; x < width; ++x) {
				if (!map.At(x, y)->isBlock) s.push_back(U'　');
				else s.push_back(U'Ｂ');
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
	}

	inline void Scene::Update() {
	}

	inline void Scene::Draw() {
		gLooper.ctcDefault.Draw({ 0, gLooper.windowSize_2.y - 5 }, "(map) show console", xx::RGBA8_Green, { 0.5f, 1 });
	}

}
