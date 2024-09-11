#pragma once
#include "pch.h"
#include "battle.h"

namespace Battle {

	void Scene::Init() {
		effectTextManager.Init(8192);
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
					p.x = scene->rnd.Next<float>(Cfg::mapEdgeMin.x, Cfg::mapEdgeMax.x);
					p.y = scene->rnd.Next<float>(Cfg::mapEdgeMin.y, Cfg::mapEdgeMax.y);
					scene->monsters.EmplaceInit(scene, p);
				}
				co_yield 0;
			}
		}(this);

	}

	void Scene::BeforeUpdate() {
	}

	int32_t Scene::Update() {
		++frameNumber;

		// update monsters
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

		// update blade light
		bladeLights.ForeachFlags([&](BladeLight& o)->xx::ForeachResult {
			return o.Update() ? xx::ForeachResult::RemoveAndContinue : xx::ForeachResult::Continue;
		});

		// update explosions
		explosions.ForeachFlags([&](Explosion& o)->xx::ForeachResult {
			return o.Update() ? xx::ForeachResult::RemoveAndContinue : xx::ForeachResult::Continue;
		});

		// update effect text
		effectTextManager.Update();

		return 0;
	}

	void Scene::Draw() {
		auto& c = gLooper.camera;

		// monster body
		if constexpr (true) {
			xx::Quad q;
			q.SetFrame(gRes.cring);
			monsters.Foreach([&](Monster& o)->void {
				q.SetPosition(c.ToGLPos(o.pos))
					.SetScale(c.scale * (o.radius / 32))
					.Draw();
			});
		}

		// monster body line
		if constexpr (false) {
			xx::LineStrip ls;
			ls.FillCirclePoints({}, 32, {}, 10);
			monsters.Foreach([&](Monster& o)->void {
				ls.SetPosition(c.ToGLPos(o.pos))
					.SetScale(c.scale * (o.radius / 32))
					.Draw();
			});
		}

		// monster id text
		if constexpr (false) {
			std::u32string str;
			monsters.Foreach([&](Monster& o)->void {
				auto len = xx::IntToStringTo(str, o.id);
				gLooper.ctcDefault.Draw(c.ToGLPos(o.pos), { 0.5f, 0.5f }, xx::RGBA8_Blue, str);
			});
		}

		// blade light
		if constexpr (true) {
			xx::Quad q;
			q.SetFrame(gRes.blade_light);
			bladeLights.ForeachFlags([&](BladeLight& o)->void {
				q.SetPosition(c.ToGLPos(o.pos))
					.SetScale(c.scale * o.scale)
					.SetRotate(o.radians)
					.SetColorA(int8_t(255 * o.alpha))
					.Draw();
			});
		}

		// explosions
		if constexpr (true) {
			xx::Quad q;
			q.SetColorA(123);
			explosions.ForeachFlags([&](Explosion& o)->void {
				q.SetFrame(gRes.explosion_13_[(size_t)o.frameIndex])
					.SetPosition(c.ToGLPos(o.pos))
					.SetScale(c.scale * o.scale)
					.SetRotate(o.radians)
					.Draw();
			});
		}

		// health bar
		if constexpr (true) {
			xx::Quad q;
			q.SetFrame(gRes.quad).SetAnchor({ 0, 0.5f });
			monsters.Foreach([&](Monster& o)->void {
				auto healthBarBaseScaleX = (float)o.health / o.maxHealth;
				q.SetPosition(c.ToGLPos(o.pos + XY{ -32, -24 }))
					.SetScale(c.scale * XY{ 1, 0.1})
					.SetColor(xx::RGBA8_Black)
					.Draw()
					.SetPositionX(q.pos.x + 1)
					.SetScale(c.scale * XY{ healthBarBaseScaleX * (62.f / 64.f), 0.1f * (4.4f / 6.4f) })
					.SetColor(xx::RGBA8_Red)
					.Draw();
			});
		}

		// effect text
		if constexpr (true) {
			effectTextManager.Draw();
		}

	}

};
