#include "pch.h"
#include "looper.h"

void Bullet::Init(xx::XY const& bornPos, xx::XY const& inc_) {
	pos = bornPos;
	inc = inc_ * cSpeed;
	damage = gLooper.rnd.Next(cDamage, cMaxDamage);
	life = cLife;
}

int32_t Bullet::UpdateCore() {
	return 0;
}

bool Bullet::Update() {
	if (--life <= 0) return true;

	pos += inc;
	if (pos.x < 0 || pos.y < 0) return true;
	if (pos.x >= gLooper.mapSize.x || pos.y >= gLooper.mapSize.y) return true;

	// check hit monsters
	if (auto r = gLooper.FindNeighborMonster(pos, cRadius)) {
		r->Hit(damage, pos);	// unsafe
		return true;
	}

	// calc aabb
	auto& sg = gLooper.trees;
	xx::FromTo<xx::XY> aabb{ pos - cRadius, pos + cRadius };
	if (!sg.TryFixAABB(aabb)) return true;

	// check hit trees
	sg.ForeachAABB(aabb);
	auto guard = xx::MakeSimpleScopeGuard([&] { sg.ClearResults(); });
	for (auto tree : sg.results) {
		if (xx::Calc::Intersects::BoxCircle<float>(tree->pos.x, tree->pos.y
			, Tree::cSize_2.x, Tree::cSize_2.y
			, pos.x, pos.y, cRadius)) {
			return true;
		}
	}

	return false;
}

void Bullet::Draw() {
	auto& camera = gLooper.camera;
	auto& q = xx::Quad::DrawOnce(gRes.bullet);
	q.pos = camera.ToGLPos(pos);
	q.anchor = { 0.5f, 0.5f };
	q.scale = camera.scale * (cRadius * 0.125f);
	q.radians = 0;
	q.colorplus = 1;
	q.color = { 255, 255, 255, 255 };
}
