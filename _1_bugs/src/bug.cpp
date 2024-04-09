#include "pch.h"
#include "looper.h"
#include "cfg.h"
#include "bug.h"
#include "scene.h"

void BugBody::Init(XY const& pos_, SpaceWeak<BugBody> head_, SpaceWeak<BugBody> prev_, bool isTail_) {
	head = head_;
	prev = prev_;
	isTail = isTail_;
	pos = pos_;
}

XY BugBody::GenRndPos(float radius, float safeRadius) {
	auto& rnd = gScene->rnd;
	float len = radius - safeRadius;
	auto r = std::sqrt(rnd.Next<float>() * (len / radius) + safeRadius / radius) * radius;
	auto a = rnd.Next<float>(gNPI, gPI);
	auto p = pos + XY{ std::cos(a) * r, std::sin(a) * r };
	// map edge limit
	if (p.x < gCfg.mapSafeMinPos.x) {
		p.x = gCfg.mapSafeMinPos.x - (p.x - gCfg.mapSafeMinPos.x);
	} else if (p.x >= gCfg.mapSafeMaxPos.x) {
		p.x = gCfg.mapSafeMaxPos.x - (p.x - gCfg.mapSafeMaxPos.x);
	}
	if (p.y < gCfg.mapSafeMinPos.y) {
		p.y = gCfg.mapSafeMinPos.y - (p.y - gCfg.mapSafeMinPos.y);
	} else if (p.y >= gCfg.mapSafeMaxPos.y) {
		p.y = gCfg.mapSafeMaxPos.y - (p.y - gCfg.mapSafeMaxPos.y);
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
			gScene->grid.Update(*this);
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
				gScene->grid.Update(*this);
			} else {
				pos = tarPos;
				gScene->grid.Update(*this);
				break;
			}
			co_yield 0;
		}
	}
}

#else

int BugBody::UpdateCore() {
	auto& grid = gScene->grid;
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
					grid.Update(*this);
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
				RotateControl::Step(radians, r, cMinRadians);
				XY inc{ std::cos(radians), std::sin(radians) };
				if (v.x * v.x + v.y * v.y > cSpeed * cSpeed) {
					pos += inc * cSpeed;
					grid.Update(*this);
				} else {
					pos = tarPos;
					grid.Update(*this);
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
	Ref<Frame>* f;
	if (prev) {
		if (isTail) f = &gRes.bug_tail;
		else f = &gRes.bug_body;
	} else {
		f = &gRes.bug_head1;
	}
	auto& q = Quad::DrawOnce(*f);
	q.pos = gScene->camera.ToGLPos(pos);
	q.anchor = cAnchor;
	q.scale = gScene->camera.scale * cScale;
	q.radians = radians;
	q.colorplus = 1;
	q.color = RGBA8_White;
}
