#pragma once

namespace Battle {

	XX_INLINE void StatInfo::Append(Stat const& s) {
		(&health)[(uint32_t)s.type] += s.value;
	}

};
