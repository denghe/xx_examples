#include "pch.h"
#include "looper.h"
#include "msgs.h"

namespace Msgs {
	namespace Global {

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

		int32_t Scene::ReadFrom(xx::Data_r& dr) {
			if (auto r = dr.Read(frameNumber, rnd, monsters)) return r;
			// todo: monsterSpace
			assert(dr.offset == dr.len);
			// todo: restore more fields for Draw ?
			// xx::CoutN(dr);
			return 0;
		}

		void Scene::WriteTo(xx::Data& d) const {
			d.Write(frameNumber, rnd, monsters);
			// todo: monsterSpace
		}

		/*******************************************************************************************/
		/*******************************************************************************************/

		void Player::Init(Scene* scene_, int32_t clientId_) {
			scene = xx::WeakFromThis(scene_);
			clientId = clientId_;
		}

		void Player::Update() {
			assert(false);
		}

		void Player::Draw() {
			assert(false);
		}

		void Player::WriteTo(xx::Data& d) const {
			assert(false);
		}

		int32_t Player::ReadFrom(xx::Data_r& dr) {
			assert(false);
			return 0;
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

		Monster::~Monster() {
			if (_sgc) {
				_sgc->Remove(this);
			}
		}

		void Monster::Init(Scene* scene_) {
			scene = xx::WeakFromThis(scene_);
			x = scene->rnd.Next<int32_t>(-500, 500);
			y = scene->rnd.Next<int32_t>(-500, 500);
			radius = scene->rnd.Next<int32_t>(16, 129);
			radians = FX64{ scene->rnd.Next<int32_t>(-31416, 31416) } / FX64{ 10000 };
			frameIndex.SetZero();
		}

		void Monster::Init(Scene* scene_, MonsterData const& md) {
			scene = xx::WeakFromThis(scene_);
			*(MonsterData*)this = md;
		}

		bool Monster::Update() {
			frameIndex = frameIndex + cFrameIndexStep;
			if (frameIndex >= cFrameIndexMax) {
				frameIndex = frameIndex - cFrameIndexMax;
			}

			return false;
		}

		void Monster::Draw() {
			static constexpr xx::RGBA8 colors[] = { xx::RGBA8_Red, xx::RGBA8_Green, xx::RGBA8_Blue };
			xx::RGBA8 color;
			if (player) {
				color = colors[player->clientId & 0b11];
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

		int32_t Monster::ReadFrom(xx::Data_r& dr) {
			if (auto r = MonsterData::ReadFrom(dr)) return r;
			return dr.Read(scene, player);
		}

		void Monster::WriteTo(xx::Data& d) const {
			MonsterData::WriteTo(d);
			d.Write(scene, player);
		}

	}
}
