#pragma once
#include "pch.h"
#include "battle.h"

namespace Battle {

	inline void Scene::Init() {
		// inits
		effectTextManager.Init(8192);
		srdd.Init(100, gLooper.physCellSize);
		monsters.Init(gLooper.physNumRows, gLooper.physNumCols, gLooper.physCellSize);
		blocks.Init(gLooper.physNumRows, gLooper.physNumCols, gLooper.physCellSize);
		
/*
top1      2           3         4          5
 ┌────────────────────┬────────────────────┐ 
 │                    │                    │ 
 │                    │                    │ 
 │                    │                    │ 
 │                    │                    │ 
 │                    │                    │ 
 │middle1             │2                   │3
 │                    │                    │ 
 │                    │                    │ 
 │                    │                    │ 
 │                    │                    │ 
 │                    │                    │ 
 └────────────────────┴────────────────────┘
bottom1    2          3         4          5
*/

		// todo: char map to blocks ?
		// combine 2 side link blocks?

		// top1
		blocks.EmplaceInit(0.f, 0.f, Cfg::mapEdgeMin.x - 0.1f, Cfg::mapEdgeMin.y - 0.1f);
		// top2
		blocks.EmplaceInit(Cfg::mapEdgeMin.x, 0.f, Cfg::mapSize_2.x - 64 - 0.1f, Cfg::mapEdgeMin.y - 0.1f);
		// top3
		blocks.EmplaceInit(Cfg::mapSize_2.x - 64, 0.f, Cfg::mapSize_2.x + 64 - 0.1f, Cfg::mapEdgeMin.y - 0.1f);
		// top4
		blocks.EmplaceInit(Cfg::mapSize_2.x + 64, 0.f, Cfg::mapEdgeMax.x - 0.1f, Cfg::mapEdgeMin.y - 0.1f);
		// top5
		blocks.EmplaceInit(Cfg::mapEdgeMax.x, 0.f, Cfg::mapSize.x - 0.1f, Cfg::mapEdgeMin.y - 0.1f);

		// middle1
		blocks.EmplaceInit(0.f, Cfg::mapEdgeMin.y, Cfg::mapEdgeMin.x - 0.1f, Cfg::mapEdgeMax.y - 0.1f);
		// middle2
		blocks.EmplaceInit(Cfg::mapSize_2.x - 64, Cfg::mapEdgeMin.y, Cfg::mapSize_2.x + 64 - 0.1f, Cfg::mapEdgeMax.y - 0.1f);
		// middle3
		blocks.EmplaceInit(Cfg::mapEdgeMax.x, Cfg::mapEdgeMin.y, Cfg::mapSize.x - 0.1f, Cfg::mapEdgeMax.y - 0.1f);

		// bottom1
		blocks.EmplaceInit(0.f, Cfg::mapEdgeMax.y, Cfg::mapEdgeMin.x - 0.1f, Cfg::mapSize.y - 0.1f);
		// bottom2
		blocks.EmplaceInit(Cfg::mapEdgeMin.x, Cfg::mapEdgeMax.y, Cfg::mapSize_2.x - 64 - 0.1f, Cfg::mapSize.y - 0.1f);
		// bottom3
		blocks.EmplaceInit(Cfg::mapSize_2.x - 64, Cfg::mapEdgeMax.y, Cfg::mapSize_2.x + 64 - 0.1f, Cfg::mapSize.y - 0.1f);
		// bottom4
		blocks.EmplaceInit(Cfg::mapSize_2.x + 64, Cfg::mapEdgeMax.y, Cfg::mapEdgeMax.x - 0.1f, Cfg::mapSize.y - 0.1f);
		// bottom5
		blocks.EmplaceInit(Cfg::mapEdgeMax.x, Cfg::mapEdgeMax.y, Cfg::mapSize.x - 0.1f, Cfg::mapSize.y - 0.1f);

		blocks.Foreach([&](Block& o)->void {
			o.FillWayout(this);
		});

		monsterEmitter = [](Scene* scene)->xx::Task<> {
			float n{};
			XY p;
			while (true) {
				n += scene->genSpeed / gLooper.fps;
				for (; n >= 1.f; --n) {
					p.x = scene->rnd.Next<float>(Cfg::mapEdgeMin.x + Cfg::maxItemSize_2, Cfg::mapEdgeMax.x - Cfg::maxItemSize_2);
					p.y = scene->rnd.Next<float>(Cfg::mapEdgeMin.y + Cfg::maxItemSize_2, Cfg::mapEdgeMax.y - Cfg::maxItemSize_2);
					scene->monsters.EmplaceInit(scene, p);
				}
				co_yield 0;
			}
		}(this);

	}

	inline int32_t Scene::Update() {
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

	inline void Scene::Draw() {
		auto& c = gLooper.camera;

		// blocks
		if constexpr (true) {
			xx::Quad q;
			q.SetFrame(gRes.quad).SetAnchor({ 0, 1 }).SetColor({55,55,55,255});
			blocks.Foreach([&](Block& o)->void {
				q.SetPosition(c.ToGLPos(o.aabb.from))
					.SetScale((o.aabb.to - o.aabb.from) / 64 * c.scale)
					.Draw();
				});
		}

		// monster body
		if constexpr (true) {
			xx::Quad q;
			monsters.Foreach([&](Monster& o)->void {
				q.SetFrame(gRes.monster_[o.frameIndex])
					.SetPosition(c.ToGLPos(o.pos))
					.SetColor(o.color)
					.SetScale(c.scale * (o.radius / 32))
					.Draw();
			});
		}

		// monster body( line )
		if constexpr (false) {
			xx::LineStrip ls;
			ls.FillCirclePoints({}, 32, {}, 10);
			monsters.Foreach([&](Monster& o)->void {
				ls.SetPosition(c.ToGLPos(o.pos))
					.SetColor(o.color)
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

	inline void Scene::StunAll() {
		monsters.Foreach([](Monster& o)->void {
			o.Stun(1);
		});
	}

	inline bool Scene::TryMakeMonster(XY const& pos) {
		if (pos.x < 0 || pos.x >= Cfg::mapSize.x
			|| pos.y < 0 || pos.y >= Cfg::mapSize.y) return false;
		scene->monsters.EmplaceInit(scene, pos);
		return true;
	}
};
