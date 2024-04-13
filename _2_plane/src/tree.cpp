#include "pch.h"
#include "looper.h"

void Tree::Init(xx::XY const& pos_) {
	pos = pos_ + cOffset;
	aabb.from = pos - cSize_2;
	aabb.to = pos + cSize_2;
}

void Tree::Draw() {
	auto& camera = gLooper.camera;
	auto& q = xx::Quad::DrawOnce(gRes.tree);
	q.pos = camera.ToGLPos(pos);
	q.anchor = { 0.5f, cOffset.y / 128.f };
	q.scale = camera.scale;
	q.radians = 0;
	q.colorplus = 1;
	q.color = { 255, 255, 255, 255 };
}
