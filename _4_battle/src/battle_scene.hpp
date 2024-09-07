#pragma once
#include "pch.h"
#include "battle.h"

namespace Battle {

	void Scene::Init() {
		srdd.Init(100, gLooper.physCellSize);
		monsters.Init(gLooper.physNumRows, gLooper.physNumCols, gLooper.physCellSize);
		monsterEmitter = [](Scene* scene)->xx::Task<> {
			float n{};
			while (true) {
				n += 300.f / gLooper.fps;
				for (; n >= 1.f; --n) {
					scene->monsters.EmplaceInit(scene);
				}
				co_yield 0;
			}
		}(this);
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
		monsterEmitter();

		return 0;
	}

	void Scene::Draw() {
		auto& c = gLooper.camera;

		xx::Quad q;
		q.SetFrame(gRes.cring);
		monsters.Foreach([&](Monster& o)->void {
			q.SetPosition(c.ToGLPos(o.pos))
				.SetScale(c.scale * (o.radius / 32))
				.Draw();
		});

#if 0
		xx::LineStrip ls;
		ls.FillCirclePoints({}, 32, {}, 10);
		monsters.Foreach([&](Monster& o)->void {
			ls.SetPosition(c.ToGLPos(o.pos))
				.SetScale(c.scale * (o.radius / 32))
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
