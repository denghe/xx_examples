#pragma once
#include "pch.h"
#include "battle.h"

namespace Battle {

	void Scene::Init() {
		srdd.Init(100, gLooper.physCellSize);
		monsters.Init(gLooper.physNumRows, gLooper.physNumCols, gLooper.physCellSize);

		gLooper.ui->MakeChildren<xx::Button>()->Init(1, gLooper.xy7m + XY{0, 0}, gLooper.xy7a
			, gLooper.s9cfg_btn, U"stun", [&]() {
			monsters.Foreach([](Monster& o)->void {
				o.Stun(1);
			});
		});

		gLooper.ui->MakeChildren<xx::Button>()->Init(1, gLooper.xy7m + XY{0, -50}, gLooper.xy7a
			, gLooper.s9cfg_btn, U"genSpeed = 1", [&]() {
				genSpeed = 1;
		});

		gLooper.ui->MakeChildren<xx::Button>()->Init(1, gLooper.xy7m + XY{0, -100}, gLooper.xy7a
			, gLooper.s9cfg_btn, U"genSpeed = 10", [&]() {
				genSpeed = 10;
		});

		gLooper.ui->MakeChildren<xx::Button>()->Init(1, gLooper.xy7m + XY{0, -150}, gLooper.xy7a
			, gLooper.s9cfg_btn, U"genSpeed = 100", [&]() {
				genSpeed = 100;
		});

		gLooper.ui->MakeChildren<xx::Button>()->Init(1, gLooper.xy7m + XY{0, -200}, gLooper.xy7a
			, gLooper.s9cfg_btn, U"genSpeed = 1000", [&]() {
				genSpeed = 1000;
		});

		gLooper.ui->MakeChildren<xx::Button>()->Init(1, gLooper.xy7m + XY{0, -250}, gLooper.xy7a
			, gLooper.s9cfg_btn, U"genSpeed = 10000", [&]() {
				genSpeed = 10000;
		});

		monsterEmitter = [](Scene* scene)->xx::Task<> {
			float n{};
			XY p;
			while (true) {
				n += scene->genSpeed / gLooper.fps;
				for (; n >= 1.f; --n) {
					p.x = Cfg::mapSize_2.x + scene->rnd.Next<float>(Cfg::width) - Cfg::width_2;
					p.y = Cfg::mapSize_2.y + scene->rnd.Next<float>(Cfg::height) - Cfg::height_2;
					scene->monsters.EmplaceInit(scene, p);
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

#if 0
#endif

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

#if 1
		xx::Quad q;
		q.SetFrame(gRes.cring);
		monsters.Foreach([&](Monster& o)->void {
			q.SetPosition(c.ToGLPos(o.pos))
				.SetScale(c.scale * (o.radius / 32))
				.Draw();
		});
#endif

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
