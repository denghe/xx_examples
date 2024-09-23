#pragma once

namespace Battle {

	struct Projectile : Drawable {
		Projectile() = default;
		Projectile(Projectile const&) = delete;
		Projectile& operator=(Projectile const&) = delete;
		virtual ~Projectile() {}
		virtual bool Update() { return false; }
		bool IsBlockIntersect();

		xx::SpaceGridWeak<Monster> owner;
		XY moveStep{};
		float moveSpeed{}, timeout{};
		float frameIndex{};
		// ...
	};

}
