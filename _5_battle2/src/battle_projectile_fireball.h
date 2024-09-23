#pragma once

namespace Battle {

	struct Projectile_Fireball : Projectile {
		static constexpr float cRadius{ 7 };

		Projectile_Fireball() = default;
		void Init(xx::SpaceGridWeak<Monster> const& owner_, XY const& pos_, float radians_
			, float radius_, float moveSpeed_, float lifeSeconds_, int32_t damage_);
		bool Update() override;

		int32_t damage{};
		// ...
	};

}
