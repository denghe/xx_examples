#include "pch.h"
#include "looper.h"
#include "msgs.h"

namespace Msgs {
	namespace Global {

		int32_t Scene::ReadFrom(xx::Data_r& dr) {
			if (auto r = dr.Read(frameNumber, rnd, monsters, players)) return r;
			int32_t numRows{}, numCols{}, cellSize{}, monstersLen{};
			if (auto r = dr.Read(numRows, numCols, cellSize, monstersLen)) return r;
			if (monstersLen != monsters.len) return __LINE__;
			for (int32_t i = 0; i < monstersLen; ++i) {
				auto& m = monsters[i];
				m->_sgc = &monsterSpace;
				size_t prev{}, next{};
				if (auto r = dr.Read(prev, next, m->_sgcIdx, m->_x, m->_y, m->_radius)) return r;
				if (prev) {
					// m->_sgcPrev = todo
				} else {
					m->_sgcPrev = {};
				}
				if (next) {
					// m->_sgcNext = todo
				} else {
					m->_sgcPrev = {};
				}
			}
			return 0;
		}

		void Scene::WriteTo(xx::Data& d) const {
			d.Write(frameNumber, rnd, monsters, players);
			auto& s = monsterSpace;
			auto monstersLen = monsters.len;
			d.Write(s.numRows, s.numCols, s.cellSize, monstersLen);
			for (int32_t i = 0; i < monstersLen; ++i) {
				auto& m = monsters[i];
				// m->_sgc: ignore; always == monsterSpace
				if (m->_sgcPrev) {
					d.Write(xx::SharedFromThis(m->_sgcPrev).GetHeader()->ud);
				} else {
					d.WriteFixed(uint8_t(0));
				}
				if (m->_sgcNext) {
					d.Write(xx::SharedFromThis(m->_sgcNext).GetHeader()->ud);
				} else {
					d.WriteFixed(uint8_t(0));
				}
				d.Write(m->_sgcIdx, m->_x, m->_y, m->_radius);
			}
		}

		void Scene::Init() {
			if (gIsServer) {
				frameNumber = 1000;	// skip some cast delay
				monsterSpace.Init(100, 100, 128);
			} else {
				assert(false);
			}
		}

		void Scene::InitForDraw() {
			tex = gLooper.fb.MakeTexture({ gLooper.width_2, gLooper.height });
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

		void MonsterData::WriteTo(xx::Data& d) const {
			d.Write(x, y, radius, radians, frameIndex);
		}

		int32_t MonsterData::ReadFrom(xx::Data_r& dr) {
			return dr.Read(x, y, radius, radians, frameIndex);
		}

		/*******************************************************************************************/
		/*******************************************************************************************/

		int32_t Monster::ReadFrom(xx::Data_r& dr) {
			if (auto r = MonsterData::ReadFrom(dr)) return r;
			return dr.Read(scene, owner);
		}

		void Monster::WriteTo(xx::Data& d) const {
			MonsterData::WriteTo(d);
			d.Write(scene, owner);
		}

		Monster::~Monster() {
			if (_sgc) {
				_sgc->Remove(this);
			}
		}

		Monster* Monster::Init(Scene* scene_, xx::Shared<Player> const& owner_, xx::XYi const& bornPos) {
			MonsterData md;
			md.x = bornPos.x;
			md.y = bornPos.y;
			md.radius = scene_->rnd.Next<int32_t>(16, 129);
			md.radians = FX64{ scene_->rnd.Next<int32_t>(-31416, 31416) } / FX64{ 10000 };
			md.frameIndex.SetZero();
			return Init(scene_, owner_, md);
		}

		Monster* Monster::Init(Scene* scene_, xx::Shared<Player> const& owner_, MonsterData const& md) {
			scene = xx::WeakFromThis(scene_);
			owner = owner_.ToWeak();
			*(MonsterData*)this = md;
			scene->monsters.Emplace(xx::SharedFromThis(this));
			scene->monsterSpace.Add(this);
			return this;
		}

		bool Monster::Update() {
			frameIndex = frameIndex + cFrameIndexStep;
			if (frameIndex >= cFrameIndexMax) {
				frameIndex = frameIndex - cFrameIndexMax;
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
			q.pos = { x.ToFloat(), y.ToFloat() };
			q.anchor = *frame->anchor;
			q.scale = (radius / FX64{ 64 }).ToFloat();
			q.radians = radians.ToFloat();
			q.colorplus = 1;
			q.color = color;
			q.texRect.data = frame->textureRect.data;
		}

	}
}
