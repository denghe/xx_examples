#pragma once

namespace Battle {

	inline int32_t Monster::Update() {
		auto posBak = pos;
		auto& ms = gScene->monsters;
		
		// todo: get round count and find move wayout
		auto idx = ms.PosToCIdx(pos);
		auto count = ms.counts[idx];
		if (count > cNeighbourMaxCount) {
			if (!runawayMode) {
				runawayMode = true;
				auto r = gLooper.rnd.Next<float>(float(M_PI * 2));
				inc = xx::XY{ std::cos(r), std::sin(r) } * cSpeed * 10;
			}
			pos += inc;
			// todo: check runawayMode timeout ?
		} else {
			if (moving) {
				pos += crossInc;
			}
			if (--leftMoveStep <= 0) {
				if (FillCrossInc()) {
					moving = true;
					leftMoveStep = 5;
				} else {
					moving = false;
				}
			}
		}

		if (BlocksLimit())
			return -1;
		if (posBak == pos) return 0;
		return 1;
	}

	XX_INLINE bool Monster::FillCrossInc() {
		int limit = cNeighbourMaxCount;
		crossInc = {};
		gScene->monsters.Foreach9All(pos.x, pos.y, [&](Monster& m)->bool {
			if (&m == this) return false;

			auto d = pos - m.pos;
			auto mag2 = d.x * d.x + d.y * d.y;
			auto r = m.radius + radius;
			auto rr = r * r;
			if (mag2 < rr) {	// cross?
				if (mag2 > 0) {
					auto mag = std::sqrt(mag2);
					auto v = d / mag;
					crossInc += v * cSpeed;
				} else {
					auto r = gLooper.rnd.Next<float>(float(M_PI * 2));
					crossInc += xx::XY{ std::cos(r), std::sin(r) } *cSpeed;
				}
			}
			return --limit < 0;
		});
		if (std::abs(crossInc.x) < 0.00001 && std::abs(crossInc.y) < 0.00001) {
			crossInc = {};
			return false;
		} else {
			crossInc = crossInc.Normalize() * cSpeed;
			return true;
		}
	}

};
