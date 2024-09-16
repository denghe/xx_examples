#pragma once

namespace Battle {

	// int array's index
	enum class StatTypes : uint32_t {
		Health,
		Mana,
		Damage,
		// ...
	};

	struct Stats;

	// int array
	struct alignas(8) StatInfo {
		int32_t health{};
		int32_t mana{};
		int32_t damage{};
		// ...

		void Append(Stats& sp);
	};

	struct alignas(8) Stat {
		StatTypes type;
		int32_t value;
	};

	struct alignas(8) Stats {
		int32_t statsLen{};
		Stat stats[0];
		Stat& operator[](int32_t index);

		static xx::Ref<Stats> Make(int32_t statsLen);
	};

}
