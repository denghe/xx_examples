#pragma once

namespace Battle {

	inline int32_t Monster::Update() {
		auto posBak = pos;
		auto& ms = gScene->monsters;
		
		// get round count and find move wayout
		auto idx = ms.PosToCIdx(pos);
		auto count = ms.counts[idx];
		if (count > cNeighbourMaxCount) {
			if (!runawayMode) {
				runawayMode = true;
				timeout = gLooper.time + cTimeout;
				auto r = gLooper.rnd.Next<float>(float(M_PI * 2));
				inc = xx::XY{ std::cos(r), std::sin(r) } * cSpeed;
			}
			if (timeout > gLooper.time) {
				pos += inc;
				color = xx::RGBA8_Red;
			} else {
				runawayMode = false;
				timeout = gLooper.time + cTimeout;
			}
		} else {
			if (runawayMode) {
				runawayMode = false;
				timeout = gLooper.time + cTimeout;
			}
			if (timeout > gLooper.time) {
				if (FillCrossInc()) {
					pos += inc;
					color = xx::RGBA8_Blue;
				} else {
					color = xx::RGBA8_Yellow;
				}
			} else {
				color = xx::RGBA8_White;
			}
		}

		if (color == xx::RGBA8_White) {
			if (blocksLimitCount < 5) {
				if (BlocksLimit()) return -1;
				else ++blocksLimitCount;
			}
		} else {
			if (BlocksLimit()) return -1;
		}
		if (posBak == pos) {
			return 0;
		} else {
			return 1;
		}
	}

	XX_INLINE bool Monster::FillCrossInc() {
		//int limit = cNeighbourMaxCount;
		inc = {};
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
					auto s = (r - mag) / r * cSpeed;
					inc += v * s;
					//m.timeout = gLooper.time + cTimeout;
				} else {
					auto r = gLooper.rnd.Next<float>(float(M_PI * 2));
					inc += xx::XY{ std::cos(r), std::sin(r) } *cSpeed;
				}
			}
			//return --limit < 0;
			return false;
		});
		if (std::abs(inc.x) < 0.1 && std::abs(inc.y) < 0.1) {
			inc = {};
			return false;
		} else {
			// speed limit
			if (inc.x * inc.x + inc.y * inc.y > cSpeed * cSpeed) {
				inc = inc.Normalize() * cSpeed;
			}
			return true;
		}
	}

};
