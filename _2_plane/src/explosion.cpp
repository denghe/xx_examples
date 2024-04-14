#include "pch.h"
#include "looper.h"

void Explosion::Init(xx::XY const& bornPos, float scale_) {
	pos = bornPos;
	scale = cScale * scale_;
	radians = gLooper.rnd.Next<float>(xx::gNPI, xx::gPI);
}

bool Explosion::Update() {
	frameIndex += cFrameInc;
	return frameIndex >= cFrameMaxIndex;
}

void Explosion::Draw() {
	auto& camera = gLooper.camera;
	auto& q = xx::Quad::DrawOnce(gRes.explosion_[(int32_t)frameIndex]);
	q.pos = camera.ToGLPos(pos);
	q.anchor = { 0.5f, 0.5f };
	q.scale = camera.scale * scale;
	q.radians = 0;
	q.colorplus = 1;
	q.color = { 255, 255, 255, 255 };
}
