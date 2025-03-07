#pragma once

namespace Map {

	inline void Scene::Init() {

		// fill
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

		// dump
		{
			auto s = map.Dump();
			s[beginPos.y * (map.size.x + 1) + beginPos.x] = U'ｃ';
			s[endPos.y * (map.size.x + 1) + endPos.x] = U'ｅ';
			xx::CoutN(s);
		}

		// navigation test
		{
			auto beginSpace = map.At(beginPos);
			assert(beginSpace);
			auto endSpace = map.At(endPos);
			assert(endSpace);
			auto gid = beginSpace->groupId;
			xx::Cout(gid);
			while (gid != endSpace->groupId) {
				gid = map.spaceGroups[gid].navTips[endSpace->groupId];
				xx::Cout(" -- ", gid);
			}
			xx::CoutN();
		}

	}

	inline void Scene::Update() {
	}

	inline void Scene::Draw() {
		gLooper.ctcDefault.Draw({ 0, gLooper.windowSize_2.y - 5 }, "(map) show console", xx::RGBA8_Green, { 0.5f, 1 });
	}

}
