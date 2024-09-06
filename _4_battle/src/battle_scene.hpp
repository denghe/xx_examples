#pragma once
#include "pch.h"
#include "battle.h"

namespace Battle {

	void Scene::Init() {
		monsters.Init(gLooper.physNumRows, gLooper.physNumCols, gLooper.physCellSize);
	}

	void Scene::BeforeUpdate() {
		screenAreaMin.x = gLooper.camera.safeMinX;
		screenAreaMin.y = gLooper.camera.safeMinY;
		screenAreaMax.x = gLooper.camera.safeMaxX;
		screenAreaMax.y = gLooper.camera.safeMaxY;
	}

	int32_t Scene::Update() {
		++frameNumber;

		// simulate stun event every ?? frames
		if (frameNumber % 20 == 0) {
			monsters.Foreach([](Monster& o)->void {
				o.Stun(1);
			});
		}

		// update all monsters
		monsters.Foreach([&](Monster& o)->xx::ForeachResult {
			auto r = o.Update();
			if (r == -1) return xx::ForeachResult::RemoveAndContinue;
			else if (r == 1) {
				monsters.Update(o);
			}
			return xx::ForeachResult::Continue;
		});

		// make some monsters
		for (int32_t i = 0; i < 10; i++) {
			monsters.EmplaceInit(this);
		}

		return 0;
	}

	void Scene::Draw() {
		auto& c = gLooper.camera;
		xx::LineStrip ls;
		ls.FillCirclePoints({}, 32);

#if 1
		monsters.Foreach([&](Monster& o)->void {
			ls.SetPosition(c.ToGLPos(o.pos))
				.SetScale(c.scale)
				.Draw();
		});
#endif

#if 1
		std::u32string str;
		monsters.Foreach([&](Monster& o)->void {
			auto len = xx::IntToStringTo(str, o.id);
			gLooper.ctcDefault.Draw(c.ToGLPos(o.pos), { 0.5f, 0.5f }, xx::RGBA8_Blue, str);
		});
#endif

	}

};
