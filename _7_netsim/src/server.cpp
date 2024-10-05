#include "pch.h"
#include "looper.h"
#include "server.h"

namespace server {
	xx::SerdeInfo gSerdeInfo;

	void InitSerdeInfo() {
		gSerdeInfo.Init();
		gSerdeInfo.Register<Monster>();
		// ...
	}

	void Scene::Init() {
		gLooper.msg.Clear();
		frameNumber = 1000;	// skip some cast delay
		// todo: init monsterGrid
	}

	void Scene::Update() {
		++frameNumber;

		for (int32_t i = monsters.len - 1; i >= 0; --i) {
			auto& m = monsters[i];
			if (m->Update()) {
				monsters.SwapRemoveAt(i);
			}
		}

		if (frameNumber % 180 == 0) {
			monsters.Emplace().Emplace<Monster, true>()->Init();
		}

		// write all to msg
		xx::DataEx d;
		d.si = &gSerdeInfo;
		d.Write(frameNumber, rnd, monsters);
		// todo: write monsterGrid
		//xx::CoutN(d);
		gLooper.msg = xx::DataShared(std::move(d));
	}

	Monster::~Monster() {
		if (_sgc) {
			_sgc->Remove(this);
		}
	}

	void Monster::Init() {
	}

	bool Monster::Update() {
		return false;
	}

	void Monster::WriteTo(xx::Data& d) {
		d.WriteFixedArray(&x,4);
	}

}
