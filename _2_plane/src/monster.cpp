#include "pch.h"
#include "looper.h"

void Monster::Hit(int damage, xx::XY const& bulletPos) {
	assert(damage > 0);
	if (damage >= hp) {
		// make damage number effect ( red color )
		gLooper.enm.Add(bulletPos, { 0, -1 }, xx::RGBA8_Red, hp);

		// make explosion effect
		gLooper.explosions.Emplace().Init(pos, scale);

		// remove this from container ( unsafe )
		gLooper.monsters.Remove(*this);
	} else {
		hp -= damage;

		// make damage number effect ( yellow color )
		gLooper.enm.Add(bulletPos, { 0, -1 }, xx::RGBA8_Yellow, damage);

		// make template task for color change to white a while
		if (!hertStateTG) {
			hertStateTG(gLooper.tasks, [this]()->xx::Task<> {
				while (hertStateLife) {
					--hertStateLife;
					colorPlus = 100000;
					co_yield 0;
				}
				colorPlus = 1;
			});
		}

		// refresh state life
		hertStateLife = cHertStateLife;
	}
}

void Monster::Init(xx::XY const& bornPos) {
	pos = bornPos;
	radius = cRadius;
	scale = 0;
	tarScale = gLooper.rnd.Next<float>(cScale, cMaxScale);
	scaleStep = tarScale / cScaleSteps;
}

int32_t Monster::UpdateCore() {
	COR_BEGIN

	// scale in
	while (scale < tarScale) {
		scale += scaleStep;
		radius = scale / cScale * cRadius;
		COR_YIELD
	}
	scale = tarScale;
	radius = scale / cScale * cRadius;

	// follow shooter
	while (--life > 0) {
		if (hertStateLife > 0) {
			COR_YIELD						// when hert, can't move
		}

		// step frame anim
		frameIndex += cFrameInc;
		if (frameIndex >= cFrameMaxIndex) {
			frameIndex -= cFrameMaxIndex;
		}

		// physics simulate
		{
			// calc neighbor cross force
			xx::XY combineForce{};
			int numCross{}, limit = 8;
			gLooper.monsters.Foreach9All(pos.x, pos.y, [&](Monster& m)->xx::ForeachResult {
				if (&m == this) return xx::ForeachResult::Continue;	// skip self
				auto d = pos - m.pos;
				auto rr = (m.radius + radius) * (m.radius + radius);
				auto dd = d.x * d.x + d.y * d.y;
				if (rr > dd) {										// cross?
					++numCross;
					if (dd) {
						combineForce += d / std::sqrt(dd);		// normalize
					}
				}
				return --limit < 0 ? xx::ForeachResult::Break : xx::ForeachResult::Continue;	// number limit
			});

			// calc ship follow force
			auto d = gLooper.planeLastPos - pos;
			auto dd = d.x * d.x + d.y * d.y;

			// calc new pos
			xx::XY newPos = pos;
			if (numCross) {											// cross?
				if (dd) {
					combineForce += d / std::sqrt(dd) / 100;	// weak force assign for ship follow
				}
				if (combineForce.x * combineForce.x < 0.0001 && combineForce.y * combineForce.y < 0.0001) {
					auto r = gLooper.rnd.Next<float>(float(M_PI * 2));
					newPos += xx::XY{ std::cos(r), std::sin(r) } *cSpeed * 3;
				} else {
					newPos += combineForce.Normalize() * cSpeed;
				}
			} else {
				if (dd > cSpeed * cSpeed) {							// follow shooter directly
					newPos += d / std::sqrt(dd) * cSpeed;		// normalize
				} else {
					newPos = gLooper.planeLastPos;
				}
			}

			// calc aabb
			auto& sg = gLooper.trees;
			xx::FromTo<xx::XY> aabb{ newPos - cRadius, newPos + cRadius };
			if (!sg.TryFixAABB(aabb)) COR_EXIT;

			// check block trees
			sg.ForeachAABB(aabb);
			auto guard = xx::MakeSimpleScopeGuard([&] { sg.ClearResults(); });
			for (auto b : sg.results) {
				xx::TranslateControl::MoveCircleIfIntersectsBox2(b->aabb, newPos, radius);
			}
			if (newPos.IsOutOfEdge(gLooper.mapSize)) COR_EXIT;

			// upgrade space grid
			if (newPos != pos) {
				pos = newPos;
				gLooper.monsters.Update(*this);
			}
		}

		COR_YIELD
	}

	// scale out
	while (scale > scaleStep) {
		scale -= scaleStep;
		radius = scale / cScale * cRadius;
		COR_YIELD
	}

	COR_END
}

bool Monster::Update() {
	return (lineNumber = UpdateCore()) == 0;
}

void Monster::Draw() {
	auto& camera = gLooper.camera;
	auto& f = gRes.monster_[(int32_t)frameIndex];
	auto& q = xx::Quad::DrawOnce(f);
	q.pos = camera.ToGLPos(pos);
	q.anchor = { 0.5f, 0.5f };
	q.scale = camera.scale * scale;
	q.radians = 0;
	q.colorplus = colorPlus;
	q.color = { 255, 255, 255, 255 };
}
