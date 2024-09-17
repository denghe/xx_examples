#pragma once

namespace Battle {

	// int array's index
	enum class StatTypes : uint32_t {
		Health,
		Mana,
		Damage,
		// ...
	};

	struct alignas(8) Stat {
		StatTypes type;
		int32_t value;
	};

	// int array
	struct alignas(8) StatInfo {
		int32_t health{};
		int32_t mana{};
		int32_t damage{};
		// ...

		void Append(Stat const& sp);
	};

}
