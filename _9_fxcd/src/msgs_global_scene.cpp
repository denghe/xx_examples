#include "pch.h"
#include "looper.h"
#include "msgs.h"

namespace Msgs {
	namespace Global {

		int32_t Scene::ReadFrom(xx::Data_r& dr) {
			if (auto r = dr.Read(frameNumber, rnd, monsters, players, blocks, bullets)) return r;

			// serialize monsterSpace
			auto& der = (xx::DataEx_r&)dr;
			int32_t numRows{}, numCols{}, cellSize{};
			if (auto r = dr.Read(numRows, numCols, cellSize)) return r;
			monsterSpace.Init(numRows, numCols, cellSize);
			for (int32_t i = 0, e = monsters.len; i < e; ++i) {
				auto& m = monsters[i];
				m->_sgc = &monsterSpace;
				m->_x = m->pos.x.ToInt();
				m->_y = m->pos.y.ToInt();
				m->_radius = m->radius.ToInt();
				m->_sgcIdx = monsterSpace.PosToCIdx(m->_x, m->_y);
				size_t prev{}, next{};
				if (auto r = dr.Read(prev, next)) return r;
				if (prev) {
					m->_sgcPrev = (decltype(m->_sgcPrev))der.ptrs[(int32_t)prev - 1].pointer;
				} else {
					m->_sgcPrev = {};
					monsterSpace.cells[m->_sgcIdx].item = m.pointer; 
				}
				if (next) {
					m->_sgcNext = (decltype(m->_sgcNext))der.ptrs[(int32_t)next - 1].pointer;
				} else {
					m->_sgcNext = {};
				}
				++monsterSpace.cells[m->_sgcIdx].count;
			}

			// restore serialize blockSpace from blocks
			blockSpace.Init(monsterSpace.numRows, monsterSpace.numCols, monsterSpace.cellSize);
			for (auto& o : blocks) {
				blockSpace.Add(o);
			}

			return 0;
		}

		void Scene::WriteTo(xx::Data& d) const {
			d.Write(frameNumber, rnd, monsters, players, blocks, bullets);

			// deserialize monsterSpace
			auto& s = monsterSpace;
			d.Write(s.numRows, s.numCols, s.cellSize);
			for (auto e = monsters.len, i = 0; i < e; ++i) {
				auto& m = monsters[i];
				// m->_sgc: ignore; always == monsterSpace
				// m->_x, m->_y, m->_radius, m->_sgcIdx: ignore; fill by calculate
				if (m->_sgcPrev) {
					d.Write(xx::GetPtrHeader(m->_sgcPrev)->ud);
				} else {
					d.WriteFixed(uint8_t(0));
				}
				if (m->_sgcNext) {
					d.Write(xx::GetPtrHeader(m->_sgcNext)->ud);
				} else {
					d.WriteFixed(uint8_t(0));
				}
			}

			// skip serialize blockSpace ( because blocks never change )
		}

		void Scene::Init() {
			assert(gIsServer);
			frameNumber = 1000;	// skip some cast delay

			monsterSpace.Init(numRows, numCols, cellSize);
			blockSpace.Init(numRows, numCols, cellSize);
		
/*
top1                  2                    3
 ┌─────────────────────────────────────────┐ 
 │                                         │ 
 │                                         │ 
 │                                         │ 
 │                                         │ 
 │                                         │ 
 │middle1                                  │2
 │                                         │ 
 │                                         │ 
 │                                         │ 
 │                                         │ 
 │                                         │ 
 └─────────────────────────────────────────┘
bottom1               2                    3
*/

			// top1
			xx::MakeShared<Block>()->Init(this, 0, 0, mapEdgeMin.x - 1, mapEdgeMin.y - 1);
			// top2
			xx::MakeShared<Block>()->Init(this, mapEdgeMin.x, 0, mapEdgeMax.x - 1, mapEdgeMin.y - 1);
			// top3
			xx::MakeShared<Block>()->Init(this, mapEdgeMax.x, 0, mapSize.x - 1, mapEdgeMin.y - 1);

			// middle1
			xx::MakeShared<Block>()->Init(this, 0, mapEdgeMin.y, mapEdgeMin.x - 1, mapEdgeMax.y - 1);
			// middle2
			xx::MakeShared<Block>()->Init(this, mapEdgeMax.x, mapEdgeMin.y, mapSize.x - 1, mapEdgeMax.y - 1);

			// bottom1
			xx::MakeShared<Block>()->Init(this, 0, mapEdgeMax.y, mapEdgeMin.x - 1, mapSize.y - 1);
			// bottom2
			xx::MakeShared<Block>()->Init(this, mapEdgeMin.x, mapEdgeMax.y, mapEdgeMax.x - 1, mapSize.y - 1);
			// bottom3
			xx::MakeShared<Block>()->Init(this, mapEdgeMax.x, mapEdgeMax.y, mapSize.x - 1, mapSize.y - 1);

			for (auto& o : blocks) {
				o->FillWayout();
			}

			// make some bullet
			bullets.Emplace().Emplace()->Init(this, mapSize_2 + XYi{ -300, 0 }, 500, 0, 0.5);
		}

		void Scene::InitForDraw() {
		}

		void Scene::Update() {
			++frameNumber;

			for (int32_t i = bullets.len - 1; i >= 0; --i) {
				auto& o = bullets[i];
				if (o->Update()) {
					bullets.SwapRemoveAt(i);
				}
			}

			for (int32_t i = monsters.len - 1; i >= 0; --i) {
				auto& m = monsters[i];
				if (m->Update1()) {
					monsters.SwapRemoveAt(i);
				}
			}
			for (int32_t i = monsters.len - 1; i >= 0; --i) {
				auto& m = monsters[i];
				if (m->Update2()) {
					monsters.SwapRemoveAt(i);
				}
			}

			//xx::MakeShared<Msgs::Global::Monster>()->Init(this, {}, XYi{ 4060, 3818 });
		}

		void Scene::Draw() {
			for (auto e = blocks.len, i = 0; i < e; ++i) {
				blocks[i]->Draw();
			}

			for (auto e = monsters.len, i = 0; i < e; ++i) {
				monsters[i]->Draw();
			}

			for (auto e = bullets.len, i = 0; i < e; ++i) {
				bullets[i]->Draw();
			}

		}

		xx::Shared<Player> const& Scene::RefPlayer(int32_t clientId) {
			for (int32_t i = 0, e = players.len; i < e; ++i) {
				if (players[i]->clientId == clientId) return players[i];
			}
			return (xx::Shared<Player>&)xx::Nil;
		}

		void Scene::RemovePlayer(int32_t clientId) {
			// find & remove player
			xx::Weak<Msgs::Global::Player> p;
			bool found{};
			for (auto j = players.len - 1; j >= 0; --j) {
				auto& player = players[j];
				if (player->clientId == clientId) {
					p = player.ToWeak();
					found = true;
					players.SwapRemoveAt(j);
					break;
				}
			}
			assert(found);

			// remove all owned monsters
			for (auto k = monsters.len - 1; k >= 0; --k) {
				if (monsters[k]->owner == p) {
					monsters.SwapRemoveAt(k);
				}
			}
		}

		void Scene::PosLimitByMapSize(XYp& p) {
			// map edge protect
			if (p.x < FX64_0)
				p.x = FX64_0;
			else if (p.x >= mapSizep.x)
				p.x = mapSizep.x;
			if (p.y < FX64_0)
				p.y = FX64_0;
			else if (p.y >= mapSizep.y)
				p.y = mapSizep.y;
		}
	}
}
