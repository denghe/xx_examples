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
				tex = gLooper.fb.MakeTexture({ gLooper.width_2, gLooper.height });
				frame = xx::Frame::Create(tex);
			}
		}

		void Scene::Update() {
			++frameNumber;
			for (int32_t i = monsters.len - 1; i >= 0; --i) {
				auto& m = monsters[i];
				if (m->Update()) {
					monsters.SwapRemoveAt(i);
				}
			}
			if (frameNumber % ((int)gLooper.fps * 3) == 0) {
				monsters.Emplace().Emplace<Monster>()->Init(this);
			}
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
			// restore all data from msg
			if (auto r = dr.Read(frameNumber, rnd, monsters)) return r;
			// todo: monsterSpace
			assert(dr.offset == dr.len);
			// todo: restore more fields? for client draw ?
			//xx::CoutN(dr);
			return 0;
		}

		void Scene::WriteTo(xx::Data& d) const {
			d.Write(frameNumber, rnd, monsters);
			// todo: monsterSpace
		}

		int32_t Player::ReadFrom(xx::Data_r& dr) {
			assert(false);
			return 0;
		}

		void Player::WriteTo(xx::Data& d) const {
			assert(false);
		}

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

		bool Monster::Update() {
			frameIndex = frameIndex + cFrameIndexStep;
			if (frameIndex >= cFrameIndexMax) {
				frameIndex = frameIndex - cFrameIndexMax;
			}

			return false;
		}

		void Monster::Draw() {
			auto& frame = gRes.monster_[frameIndex.ToInt()];
			auto& q = *gLooper.ShaderBegin(gLooper.shaderQuadInstance).Draw(frame->tex->GetValue(), 1);
			q.pos = { x.ToFloat(), y.ToFloat() };
			q.anchor = *frame->anchor;
			q.scale = (radius / FX64{ 64 }).ToFloat();
			q.radians = radians.ToFloat();
			q.colorplus = 1;
			q.color = xx::RGBA8_White;
			q.texRect.data = frame->textureRect.data;
		}

		int32_t Monster::ReadFrom(xx::Data_r& dr) {
			return dr.Read(scene, x, y, radius, radians, frameIndex);
		}

		void Monster::WriteTo(xx::Data& d) const {
			d.Write(scene, x, y, radius, radians, frameIndex);
		}

	}
}
