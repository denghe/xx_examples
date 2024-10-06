#include "pch.h"
#include "looper.h"
#include "client.h"

namespace client {
	xx::SerdeInfo gSerdeInfo;

	void InitSerdeInfo() {
		gSerdeInfo.Init();
		gSerdeInfo.Register<Monster>();
		// ...
	}

	void Scene::Init() {
		tex = gLooper.fb.MakeTexture({ gLooper.width_2, gLooper.height });
		frame = xx::Frame::Create(tex);
	}

	void Scene::Update() {
		// restore all data from msg
		xx::DataEx_r dr{ gLooper.msg.GetBuf(), gLooper.msg.GetLen() };
		dr.si = &gSerdeInfo;
		int r = dr.Read(frameNumber, rnd, monsters);
		assert(!r);
		for (auto& m : monsters) {
			m->scene = this;
		}
		// todo: read monsterGrid
		assert(dr.offset == dr.len);
		//xx::CoutN(dr);
	}

	void Scene::Draw() {
		gLooper.fb.DrawTo(tex, xx::RGBA8_Black, [this] {
			for (int32_t i = 0, e = monsters.len; i < e; ++i) {
				auto& m = monsters[i];
				m->Draw();
			}
		});
	}

	Monster::~Monster() {
		if (_sgc) {
			_sgc->Remove(this);
		}
	}

	void Monster::Init(Scene* scene_) {
		scene = scene_;
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
		return dr.Read(x, y, radius, radians, frameIndex);
	}

}
