#pragma once

namespace Battle {

	enum class StatTypes {
		Health,
		Mana,
		Damage,
		// ...
	};

	struct alignas(8) Stat {
		StatTypes type;
		int32_t value;
	};

	struct alignas(8) StatPack {
		// for config & display
		int32_t statsLen{};
		Stat stats[0];
		Stat& operator[](int32_t index) {
			assert(index < statsLen);
			return stats[index];
		}

		static xx::Ref<StatPack> Make(int32_t statsLen) {
			xx::Ref<StatPack> r;
			r.EmplaceEx(sizeof(Stat) * statsLen);
			r->statsLen = statsLen;
			return r;
		}
	};

	struct alignas(8) StatInfo {
		int32_t health{};
		int32_t mana{};
		int32_t damage{};
		// ...

		void Append(StatPack& sp);
	};

}
