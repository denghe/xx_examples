#include "pch.h"
#include "looper.h"
#include "bug.h"

void BugBody::Init(XY const& pos_, xx::SpaceGridWeak<BugBody> head_, xx::SpaceGridWeak<BugBody> prev_, bool isTail_) {
	head = head_;
	prev = prev_;
	isTail = isTail_;
	pos = pos_;
}

XY BugBody::GenRndPos(float radius, float safeRadius) {
	float len = radius - safeRadius;
	float radius_1 = 1 / radius;
	auto r = std::sqrt(gLooper.rnd.Next<float>() * (len * radius_1) + safeRadius * radius_1) * radius;
	auto a = gLooper.rnd.Next<float>(xx::gNPI, xx::gPI);
	auto p = pos + XY{ std::cos(a) * r, std::sin(a) * r };
	// map edge limit
	if (p.x < Cfg::mapEdgeMin.x || p.x >= Cfg::mapEdgeMax.x
		|| p.y < Cfg::mapEdgeMin.y || p.y >= Cfg::mapEdgeMax.y) {
		return Cfg::mapSize_2;
	}
	return p;
}

#ifdef UPDATE_USE_COROUTINE

Task<> BugBody::UpdateTask_() {
	// is body: follow prev
	while (prev) {
		auto& p = prev();
		auto v = p.pos - pos;
		auto distance = std::sqrt(v.x * v.x + v.y * v.y);
		if (auto d = distance - cDistance; d > 0) {
			auto inc = v / distance * std::min(d, cSpeed);
			pos += inc;
            gLooper.grid.Update(*this);
		}
		co_yield 0;
	}
	// is head: select random pos & gogogo
	while (true) {
		tarPos = GenRndPos(300, 50);
		while (true) {
			auto v = tarPos - pos;
			auto r = std::atan2(v.y, v.x);
			RotateControl::Step(radians, r, cMinRadians);
			XY inc{ std::cos(radians), std::sin(radians) };
			if (v.x * v.x + v.y * v.y > cSpeed * cSpeed) {
				pos += inc * cSpeed;
                gLooper.grid.Update(*this);
			} else {
				pos = tarPos;
                gLooper.grid.Update(*this);
				break;
			}
			co_yield 0;
		}
	}
}

#else

int BugBody::UpdateCore() {
	COR_BEGIN
		// is body: follow prev
		while (prev) {
			{
				auto& p = prev();
				auto v = p.pos - pos;
				auto distance = std::sqrt(v.x * v.x + v.y * v.y);
				if (auto d = distance - cDistance; d > 0) {
					auto inc = v / distance * std::min(d, cSpeed);
					pos += inc;
					gLooper.grid.Update(*this);
				}
			}
			COR_YIELD
		}
	// is head: select random pos & gogogo
	while (true) {
		tarPos = GenRndPos(300, 50);
		while (true) {
			{
				auto v = tarPos - pos;
				auto r = std::atan2(v.y, v.x);
				xx::RotateControl::Step(radians, r, cMinRadians);
				if (v.x * v.x + v.y * v.y > cSpeed * cSpeed) {
					XY inc{ std::cos(radians), std::sin(radians) };
					pos += inc * cSpeed;
					gLooper.grid.Update(*this);
				} else {
					pos = tarPos;
					gLooper.grid.Update(*this);
					break;
				}
			}
			COR_YIELD
		}
	}
	COR_END
}

#endif

bool BugBody::Update() {
#ifdef UPDATE_USE_COROUTINE
	return UpdateTask();
#else
	return (lineNumber = UpdateCore()) == 0;
#endif
}

void BugBody::Draw() {
	xx::Ref<xx::Frame>* f;
	if (prev) {
		if (isTail) f = &gRes.bug_tail;
		else f = &gRes.bug_body;
	} else {
		f = &gRes.bug_head1;
	}
	auto& q = xx::Quad::DrawOnce(*f);
	q.pos = gLooper.camera.ToGLPos(pos);
	q.anchor = cAnchor;
	q.scale = gLooper.camera.scale * cScale;
	q.radians = radians;
	q.colorplus = 1;
	q.color = xx::RGBA8_White;
}
