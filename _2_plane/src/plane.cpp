#include "pch.h"
#include "looper.h"

void Plane::Init(xx::XY const& bornPos) {
	gLooper.planeLastPos = pos = bornPos;
}

bool Plane::Update() {
	if (auto inc = gLooper.GetKeyboardMoveInc(); inc.has_value()) {
		pos += inc->second * cSpeed;
	}
	if (pos.IsOutOfEdge(gLooper.mapSize)) return true;
	gLooper.planeLastPos = pos;

	auto mp = gLooper.camera.ToLogicPos(gLooper.mouse.pos) - pos;
	auto r = std::atan2(mp.y, mp.x);

	radians = xx::RotateControl::LerpAngleByFixed(r, radians, cFrameMaxChangeRadian);

	if (gLooper.mouse.PressedMBLeft()) {
		xx::XY inc{ std::cos(radians), std::sin(radians) };
		gLooper.bullets.Emplace().Init(pos + inc * cFireDistance, inc);
		for (size_t i = 1; i <= 3; ++i) {
			auto r1 = r + 0.1f * (float)i;
			inc = { std::cos(r1), std::sin(r1) };
			gLooper.bullets.Emplace().Init(pos + inc * cFireDistance, inc);
			auto r2 = r - 0.1f * (float)i;
			inc = { std::cos(r2), std::sin(r2) };
			gLooper.bullets.Emplace().Init(pos + inc * cFireDistance, inc);
		}
	}

	return false;
}

void Plane::Draw() {
	auto& camera = gLooper.camera;
	auto& q = xx::Quad::DrawOnce( gRes.plane );
	q.pos = camera.ToGLPos(pos);
	q.anchor = { 0.5f, 0.5f };
	q.scale = camera.scale * cScale;
	q.radians = radians;
	q.colorplus = 1;
	q.color = { 255, 255, 255, 255 };
}
