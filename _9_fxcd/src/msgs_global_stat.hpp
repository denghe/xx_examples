#pragma once

namespace Msgs {
	namespace Global {

		inline void StatPanelBase::Clear() {
			memset(this, 0, sizeof(StatPanelBase));
		}

		XX_INLINE inline Stat_t& StatPanelBase::At(StatTypes t) const {
			assert((uint32_t)t <= (uint32_t)StatTypes::__POINTS_END__);
			return ((Stat_t*)this)[(uint32_t)t];
		}

		inline void StatPanel::Clear() {
			memset(this, 0, sizeof(StatPanel));
		}

		XX_INLINE inline Stat_t& StatPanel::At(StatTypes t) const {
			assert((uint32_t)t <= (uint32_t)StatTypes::__RESULTS_END__);
			return ((Stat_t*)this)[(uint32_t)t];
		}

		inline void StatPanelExt::Clear() {
			memset(this, 0, sizeof(StatPanelExt));
		}

		XX_INLINE inline Stat_t& StatPanelExt::At(StatTypes t) const {
			assert((uint32_t)t <= (uint32_t)StatTypes::__MAX_VALUE__);
			return ((Stat_t*)this)[(uint32_t)t];
		}

	}
}
