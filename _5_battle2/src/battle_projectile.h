#pragma once

namespace Battle {

	struct Projectile : Drawable {
		Projectile() = default;
		Projectile(Projectile const&) = delete;
		Projectile& operator=(Projectile const&) = delete;
		virtual ~Projectile() {}
		virtual bool Update() { return false; }
		bool IsBlockIntersect();

		xx::SpaceWeak<Monster> owner;
		XY moveStep{};
		float moveSpeed{}, timeout{};
		float frameIndex{};
		// ...
	};

	struct Projectile_Fireball : Projectile {
		Projectile_Fireball() = default;
		void Init(xx::SpaceWeak<Monster> const& owner_, XY const& pos_, float radians_
			, float radius_, float moveSpeed_, float lifeSeconds_, int32_t damage_);
		bool Update() override;

		int32_t damage{};
		// todo
	};

}
