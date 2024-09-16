#pragma once

namespace Battle {

	XX_INLINE void StatInfo::Append(Stats& sp) {
		for (int32_t i = 0; i < sp.statsLen; ++i) {
			auto& s = sp[i];
			(&health)[(uint32_t)s.type] += s.value;
		}
	}

	XX_INLINE Stat& Stats::operator[](int32_t index) {
		assert(index < statsLen);
		return stats[index];
	}

	XX_INLINE xx::Ref<Stats> Stats::Make(int32_t statsLen) {
		xx::Ref<Stats> r;
		r.EmplaceEx(sizeof(Stat) * statsLen);
		r->statsLen = statsLen;
		return r;
	}

};
