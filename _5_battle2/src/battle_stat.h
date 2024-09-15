#pragma once

namespace Battle {

	// offset in StatPanel
	enum class StatTypes {
		Health, 
		Damage,
		// ...
	};

	struct alignas(8) StatPanel {
		int32_t health;
		int32_t damage;
		// ...
		// todo: operator +=
	};

	struct alignas(8) Stat {
		StatTypes type;
		int32_t value;
	};

}
