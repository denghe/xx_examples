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
		monsterSpace.Init(100, 100, 128);
	}

	void Scene::Update() {
		++frameNumber;

		for (int32_t i = monsters.len - 1; i >= 0; --i) {
			auto& m = monsters[i];
			if (m->Update()) {
				monsters.SwapRemoveAt(i);
			}
		}

		//if (frameNumber % 180 == 0) {
			monsters.Emplace().Emplace<Monster, true>()->Init(this);
		//}

		// write all to msg
		xx::DataEx d;
		d.si = &gSerdeInfo;
		d.Write(frameNumber, rnd, monsters);
		// todo: write monsterGrid
		gLooper.msg = xx::DataShared(std::move(d));
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

	void Monster::WriteTo(xx::Data& d) {
		d.Write(x, y, radius, radians, frameIndex);
	}

}
