﻿#include "pch.h"
#include "looper.h"
#include "msgs.h"

namespace Msgs {
	namespace Global {

		int32_t Scene::ReadFrom(xx::Data_r& dr) {
			if (auto r = dr.Read(frameNumber, rnd, monsters, players)) return r;
			auto& der = (xx::DataEx_r&)dr;
			int32_t numRows{}, numCols{}, cellSize{};
			if (auto r = dr.Read(numRows, numCols, cellSize)) return r;
			monsterSpace.Init(numRows, numCols, cellSize);
			for (int32_t i = 0, e = monsters.len; i < e; ++i) {
				auto& m = monsters[i];
				m->_sgc = &monsterSpace;
				m->_x = m->x.ToInt();
				m->_y = m->y.ToInt();
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
			return 0;
		}

		void Scene::WriteTo(xx::Data& d) const {
			//d.Write(frameNumber, rnd, monsters, players);
			d.Write(frameNumber, rnd);
			d.Write(monsters, players);
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
		}

		void Scene::Init() {
			if (gIsServer) {
				frameNumber = 1000;	// skip some cast delay
				monsterSpace.Init(2000, 2000, 64 + 32);
			} else {
				assert(false);
			}
		}

		void Scene::InitForDraw() {
			tex = gLooper.fb.MakeTexture({ gLooper.width_2, gLooper.height_2 });
			frame = xx::Frame::Create(tex);
		}

		void Scene::Update() {
			++frameNumber;
			for (int32_t i = monsters.len - 1; i >= 0; --i) {
				auto& m = monsters[i];
				if (m->Update()) {
					monsters.SwapRemoveAt(i);
				}
			}
			// auto generate some ?
			//if (frameNumber % ((int)gLooper.fps * 3) == 0) {
			//	monsters.Emplace().Emplace<Monster>()->Init(this);
			//}
		}

		void Scene::Draw() {
			gLooper.fb.DrawTo(tex, xx::RGBA8_Black, [this] {
				for (int32_t i = 0, e = monsters.len; i < e; ++i) {
					auto& m = monsters[i];
					m->Draw();
				}
			});
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

		/*******************************************************************************************/
		/*******************************************************************************************/

		void Player::WriteTo(xx::Data& d) const {
			d.Write(scene, clientId, score);
		}

		int32_t Player::ReadFrom(xx::Data_r& dr) {
			return dr.Read(scene, clientId, score);
		}

		void Player::Init(Scene* scene_, int32_t clientId_) {
			scene = xx::WeakFromThis(scene_);
			clientId = clientId_;
			scene_->players.Emplace(xx::SharedFromThis(this));
		}

		void Player::Update() {
			assert(false);
		}

		void Player::Draw() {
			assert(false);
		}

		/*******************************************************************************************/
		/*******************************************************************************************/

		int32_t Monster::ReadFrom(xx::Data_r& dr) {
			return dr.Read(
				scene, owner
				, x, y, radius, radians, frameIndex
				, incX, incY
				, timeout, runawayMode
			);
		}

		void Monster::WriteTo(xx::Data& d) const {
			d.Write(
				scene, owner
				, x, y, radius, radians, frameIndex
				, incX, incY
				, timeout, runawayMode
			);
		}

		Monster::~Monster() {
			if (_sgc) {
				_sgc->Remove(this);
			}
		}

		Monster* Monster::Init(Scene* scene_, xx::Shared<Player> const& owner_, xx::XYi const& bornPos) {
			scene_->monsters.Emplace(xx::SharedFromThis(this));
			scene = xx::WeakFromThis(scene_);
			owner = owner_.ToWeak();

			x = bornPos.x;
			y = bornPos.y;
			radius = scene_->rnd.Next<int32_t>(32, 65);
			radians = FX64{ scene_->rnd.Next<int32_t>(-c314159, c314159) } * c1_100000;
			frameIndex.SetZero();

			_x = x.ToInt();
			_y = y.ToInt();
			_radius = radius.ToInt();
			scene->monsterSpace.Add(this);
			return this;
		}

		bool Monster::FillCrossInc() {
			incX.SetZero();
			incY.SetZero();

			scene->monsterSpace.Foreach9All<true>(_x, _y, [&](Monster* m)->bool {
				auto dx = _x - m->_x;
				auto dy = _y - m->_y;
				auto mag2 = dx * dx + dy * dy;
				auto r2 = _radius + m->_radius;
				auto rr = r2 * r2;
				if (mag2 < rr) {	// cross?
					if (mag2 > 0) {
						auto mag = FX64{ mag2 }.SqrtFastest();	// todo: check zero ?
						auto vx = FX64{ dx } / mag;
						auto vy = FX64{ dy } / mag;
						auto r = FX64{ r2 };
						auto s = (r - mag) / r * cMovementSpeed2;
						incX += vx * s;
						incY += vy * s;
					} else {
						auto r = FX64{ scene->rnd.Next<int32_t>(-c314159, c314159) } *c1_100000;
						incX += r.CosFastest() * cMovementSpeed2;
						incY += r.SinFastest() * cMovementSpeed2;
					}
				}
				return false;
			}, this);

			// todo: deactive logic
			
			return true;
		}

		bool Monster::Update() {
			frameIndex = frameIndex + cFrameIndexStep;
			if (frameIndex >= cFrameIndexMax) {
				frameIndex = frameIndex - cFrameIndexMax;
			}

			auto ox = x;
			auto oy = y;

			// get round count and find move wayout
			auto count = scene->monsterSpace.cells[_sgcIdx].count;
			if (count > cNeighbourMaxCount) {
				if (!runawayMode) {
					runawayMode = true;
					timeout = scene->frameNumber + cTimeout;
					auto r = FX64{ scene->rnd.Next<int32_t>(-c314159, c314159) } * c1_100000;
					incX = r.CosFastest() * cMovementSpeed3;
					incY = r.SinFastest() * cMovementSpeed3;
				}
				x += incX;
				y += incY;
				if (timeout < scene->frameNumber) {
					runawayMode = false;
				}
			} else {
				runawayMode = false;
				if (FillCrossInc()) {
					x += incX;
					y += incY;
				}
			}

			if (ox != x || oy != y) {
				_x = x.ToInt();
				_y = y.ToInt();
				_radius = radius.ToInt();
				scene->monsterSpace.Update(this);
			}
			return false;
		}

		void Monster::Draw() {
			static constexpr xx::RGBA8 colors[] = { xx::RGBA8_Yellow, xx::RGBA8_Red, xx::RGBA8_Green, xx::RGBA8_Blue };
			xx::RGBA8 color;
			if (owner) {
				color = colors[owner->clientId & 0b11];
			} else {
				color = xx::RGBA8_White;
			}

			auto& frame = gRes.monster_[frameIndex.ToInt()];
			auto& q = *gLooper.ShaderBegin(gLooper.shaderQuadInstance).Draw(frame->tex->GetValue(), 1);
			q.pos = (XY{ x.ToFloat(), y.ToFloat() } - scene->monsterSpace.max / 2) * gLooper.camera.scale;
			q.anchor = *frame->anchor;
			q.scale = (radius / cResRadius).ToFloat() * gLooper.camera.scale;
			q.radians = radians.ToFloat();
			q.colorplus = 1;
			q.color = color;
			q.texRect.data = frame->textureRect.data;
		}

	}
}
