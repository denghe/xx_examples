#pragma once

namespace Battle {

    inline void Block::Init(float minX, float minY, float maxX, float maxY) {
		aabb.from.x = minX;
		aabb.from.y = minY;
		aabb.to.x = maxX;
		aabb.to.y = maxY;
        auto siz = aabb.to - aabb.from;
        halfSize = siz / 2;
        pos = aabb.from + halfSize;
    }

    inline void Block::Init(XY const& centerPos, XY const& size) {
        pos = centerPos;
        halfSize = size / 2;
        aabb.from = pos - halfSize;
        aabb.to = pos + halfSize;
    }

	inline void Block::FillWayout(Scene* scene) {
        // search neighbor & set wayout bit
        auto& bs = scene->blocks;
        (uint8_t&)wayout = 0;
        static constexpr int dx{ 5 }, dy{ 5 };
        auto left = bs.ExistsPoint(aabb.from + XYi{ -dx, dy });
        auto up = bs.ExistsPoint(aabb.from + XYi{ dy, -dx });
        auto right = bs.ExistsPoint(aabb.to + XYi{ dx, -dy });
        auto down = bs.ExistsPoint(aabb.to + XYi{ -dy, dx });

        isMapCorner = left == 2 && up == 2
            || up == 2 && right == 2
            || right == 2 && down == 2
            || down == 2 && left == 2;

        if (left > 0 && up > 0 && right > 0 && down > 0) {
            wayout.left = left == 1;
            wayout.up = up == 1;
            wayout.right = right == 1;
            wayout.down = down == 1;
        } else {
            wayout.left = left == 0;
            wayout.up = up == 0;
            wayout.right = right == 0;
            wayout.down = down == 0;
        }
	}

    XX_INLINE bool Block::IntersectCircle(XY const& p, float radius) {
        return xx::Calc::Intersects::BoxCircle<float>(pos.x, pos.y, halfSize.x, halfSize.y, p.x, p.y, radius);
    }

    XX_INLINE bool Block::PushOut(Monster& m) {
        auto& p = m.pos;
        auto& r = m.radius;
        return xx::Math::MoveCircleIfIntersectsBox<float>(wayout, pos.x, pos.y, halfSize.x, halfSize.y, p.x, p.y, r, isMapCorner);
    }

};
