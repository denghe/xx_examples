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

	void Scene::Init(XY centerPos_) {
		centerPos = centerPos_;
	}

	void Scene::Update() {
		// todo: restore all data from msg
		xx::DataEx_r dr{ gLooper.msg.GetBuf(), gLooper.msg.GetLen() };
		dr.si = &gSerdeInfo;
		int r = dr.Read(frameNumber, rnd, monsters);
		assert(!r);
		// todo: read monsterGrid
		assert(dr.offset == dr.len);
	}

	void Scene::Draw() {
		for (int32_t i = 0, e = monsters.len; i < e; ++i) {
			auto& m = monsters[i];
			m->Draw();
		}

		xx::LineStrip ls;
		ls.FillCirclePoints(centerPos, 20).Draw();
	}

	void Monster::Init() {
	}

	bool Monster::Update() {
		return false;
	}

	void Monster::Draw() {

	}

	Monster::~Monster() {
		if (_sgc) {
			_sgc->Remove(this);
		}
	}

	int32_t Monster::ReadFrom(xx::Data_r& dr) {
		return dr.ReadFixedArray(&x, 4);
	}

}
