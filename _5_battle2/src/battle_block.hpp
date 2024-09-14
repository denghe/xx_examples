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

	inline void Block::Init(xx::FromTo<XY> const& aabb_) {
        Init(aabb_.from.x, aabb_.from.y, aabb_.to.x, aabb_.to.y);
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
        auto left = bs.ExistsPoint(aabb.from + XY{ -1, 0 });
        auto up = bs.ExistsPoint(aabb.from + XY{ 0, -1 });
        auto right = bs.ExistsPoint(aabb.to + XY{ 1, 0 });
        auto down = bs.ExistsPoint(aabb.to + XY{ 0, 1 });
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

    // b: box    c: circle    w: width    h: height    r: radius
    // if intersect, cx & cy will be changed & return true
    template<typename T, bool canUp, bool canRight, bool canDown, bool canLeft>
    XX_INLINE bool MoveCircleIfIntersectsBox(T bx, T by, T bHalfWidth, T bHalfHeight, T& cx, T& cy, T cr) {
        static_assert(canUp || canRight || canDown || canLeft);
        auto dx = std::abs(cx - bx);
        if (dx > bHalfWidth + cr) return false;

        auto dy = std::abs(cy - by);
        if (dy > bHalfHeight + cr) return false;

        if (dx <= bHalfWidth || dy <= bHalfHeight) {
            if constexpr (canUp && canRight && canDown && canLeft) {
                if (bHalfWidth - dx > bHalfHeight - dy) {
                    if (by > cy) {
                        cy = by - bHalfHeight - cr - 1;	// top
                    } else {
                        cy = by + bHalfHeight + cr + 1;	// bottom
                    }
                } else {
                    if (bx > cx) {
                        cx = bx - bHalfWidth - cr - 1;	// left
                    } else {
                        cx = bx + bHalfWidth + cr + 1;	// right
                    }
                }
            } else if constexpr (canUp && canRight && canDown) {
                if (bHalfWidth - dx > bHalfHeight - dy) {
                    if (by > cy) {
                        cy = by - bHalfHeight - cr - 1;	// top
                    } else {
                        cy = by + bHalfHeight + cr + 1;	// bottom
                    }
                } else {
                    cx = bx + bHalfWidth + cr + 1;	// right
                }
            } else if constexpr (canRight && canDown && canLeft) {
                if (bHalfWidth - dx > bHalfHeight - dy) {
                    cy = by + bHalfHeight + cr + 1;	// bottom
                } else {
                    if (bx > cx) {
                        cx = bx - bHalfWidth - cr - 1;	// left
                    } else {
                        cx = bx + bHalfWidth + cr + 1;	// right
                    }
                }
            } else if constexpr (canDown && canLeft && canUp) {
                if (bHalfWidth - dx > bHalfHeight - dy) {
                    if (by > cy) {
                        cy = by - bHalfHeight - cr - 1;	// top
                    } else {
                        cy = by + bHalfHeight + cr + 1;	// bottom
                    }
                } else {
                    cx = bx - bHalfWidth - cr - 1;	// left
                }
            } else if constexpr (canLeft && canUp && canRight) {
                if (bHalfWidth - dx > bHalfHeight - dy) {
                    cy = by - bHalfHeight - cr - 1;	// top
                } else {
                    if (bx > cx) {
                        cx = bx - bHalfWidth - cr - 1;	// left
                    } else {
                        cx = bx + bHalfWidth + cr + 1;	// right
                    }
                }
            } else if constexpr (canLeft && canRight) {
                if (bx > cx) {
                    cx = bx - bHalfWidth - cr - 1;	// left
                } else {
                    cx = bx + bHalfWidth + cr + 1;	// right
                }
            } else if constexpr (canUp && canDown) {
                if (by > cy) {
                    cy = by - bHalfHeight - cr - 1;	// top
                } else {
                    cy = by + bHalfHeight + cr + 1;	// bottom
                }
            } else if constexpr (canUp) {
                cy = by - bHalfHeight - cr - 1;	// top
            } else if constexpr (canRight) {
                cx = bx + bHalfWidth + cr + 1;	// right
            } else if constexpr (canDown) {
                cy = by + bHalfHeight + cr + 1;	// bottom
            } else if constexpr (canLeft) {
                cx = bx - bHalfWidth - cr - 1;	// left
            }
            return true;
        }

        auto dx2 = dx - bHalfWidth;
        auto dy2 = dy - bHalfHeight;
        if (dx2 * dx2 + dy2 * dy2 <= cr * cr) {
            // change cx & cy
            auto incX = dx2, incY = dy2;
            auto dSeq = dx2 * dx2 + dy2 * dy2;
            if (dSeq == T{}) {
                incX = bHalfWidth + T(cr * 0.7071067811865475 + 1);
                incY = bHalfHeight + T(cr * 0.7071067811865475 + 1);
            } else {
                auto d = std::sqrt(dSeq);
                incX = bHalfWidth + T(cr * dx2 / d) + 1;
                incY = bHalfHeight + T(cr * dy2 / d) + 1;
            }

            if constexpr (canUp && canRight && canDown && canLeft) {
                if (cx < bx) {
                    incX = -incX;
                }
                if (cy < by) {
                    incY = -incY;
                }
                cx = bx + incX;
                cy = by + incY;
            } else if constexpr (canUp && canRight && canDown) {
                if (cx < bx) {
                    incX = 0;
                }
                if (cy < by) {
                    incY = -incY;
                }
                cx = bx + incX;
                cy = by + incY;
            } else if constexpr (canRight && canDown && canLeft) {
                if (cx < bx) {
                    incX = -incX;
                }
                if (cy < by) {
                    incY = 0;
                }
                cx = bx + incX;
                cy = by + incY;
            } else if constexpr (canDown && canLeft && canUp) {
                if (cx < bx) {
                    incX = -incX;
                } else {
                    incX = 0;
                }
                if (cy < by) {
                    incY = -incY;
                }
                cx = bx + incX;
                cy = by + incY;
            } else if constexpr (canLeft && canUp && canRight) {
                if (cx < bx) {
                    incX = -incX;
                }
                if (cy < by) {
                    incY = -incY;
                } else {
                    incY = 0;
                }
                cx = bx + incX;
                cy = by + incY;
            } else if constexpr (canLeft && canRight) {
                if (bx > cx) {
                    cx = bx - bHalfWidth - cr - 1;	// left
                } else {
                    cx = bx + bHalfWidth + cr + 1;	// right
                }
            } else if constexpr (canUp && canDown) {
                if (by > cy) {
                    cy = by - bHalfHeight - cr - 1;	// top
                } else {
                    cy = by + bHalfHeight + cr + 1;	// bottom
                }
            } else if constexpr (canUp) {
                cy = by - bHalfHeight - cr - 1;	// top
            } else if constexpr (canRight) {
                cx = bx + bHalfWidth + cr + 1;	// right
            } else if constexpr (canDown) {
                cy = by + bHalfHeight + cr + 1;	// bottom
            } else if constexpr (canLeft) {
                cx = bx - bHalfWidth - cr - 1;	// left
            }

            return true;
        }
        return false;
    }


    // up
    void PushOut1(float bx, float by, float bHalfWidth, float bHalfHeight, float& cx, float& cy, float cr) {
        MoveCircleIfIntersectsBox<float, true, false, false, false>(bx, by, bHalfWidth, bHalfHeight, cx, cy, cr);
    }
    // right
    void PushOut2(float bx, float by, float bHalfWidth, float bHalfHeight, float& cx, float& cy, float cr) {
        MoveCircleIfIntersectsBox<float, false, true, false, false>(bx, by, bHalfWidth, bHalfHeight, cx, cy, cr);
    }
    // up + right
    void PushOut3(float bx, float by, float bHalfWidth, float bHalfHeight, float& cx, float& cy, float cr) {
        MoveCircleIfIntersectsBox<float, true, true, false, false>(bx, by, bHalfWidth, bHalfHeight, cx, cy, cr);
    }
    // down
    void PushOut4(float bx, float by, float bHalfWidth, float bHalfHeight, float& cx, float& cy, float cr) {
        MoveCircleIfIntersectsBox<float, false, false, true, false>(bx, by, bHalfWidth, bHalfHeight, cx, cy, cr);
    }
    // up + down
    void PushOut5(float bx, float by, float bHalfWidth, float bHalfHeight, float& cx, float& cy, float cr) {
        MoveCircleIfIntersectsBox<float, true, false, true, false>(bx, by, bHalfWidth, bHalfHeight, cx, cy, cr);
    }
    // right + down
    void PushOut6(float bx, float by, float bHalfWidth, float bHalfHeight, float& cx, float& cy, float cr) {
        MoveCircleIfIntersectsBox<float, false, true, true, false>(bx, by, bHalfWidth, bHalfHeight, cx, cy, cr);
    }
    // up + right + down
    void PushOut7(float bx, float by, float bHalfWidth, float bHalfHeight, float& cx, float& cy, float cr) {
        MoveCircleIfIntersectsBox<float, true, true, true, false>(bx, by, bHalfWidth, bHalfHeight, cx, cy, cr);
    }
    // left
    void PushOut8(float bx, float by, float bHalfWidth, float bHalfHeight, float& cx, float& cy, float cr) {
        MoveCircleIfIntersectsBox<float, false, false, false, true>(bx, by, bHalfWidth, bHalfHeight, cx, cy, cr);
    }
    // up + left
    void PushOut9(float bx, float by, float bHalfWidth, float bHalfHeight, float& cx, float& cy, float cr) {
        MoveCircleIfIntersectsBox<float, true, false, false, true>(bx, by, bHalfWidth, bHalfHeight, cx, cy, cr);
    }
    // right + left
    void PushOut10(float bx, float by, float bHalfWidth, float bHalfHeight, float& cx, float& cy, float cr) {
        MoveCircleIfIntersectsBox<float, false, true, false, true>(bx, by, bHalfWidth, bHalfHeight, cx, cy, cr);
    }
    // up + right + left
    void PushOut11(float bx, float by, float bHalfWidth, float bHalfHeight, float& cx, float& cy, float cr) {
        MoveCircleIfIntersectsBox<float, true, true, false, true>(bx, by, bHalfWidth, bHalfHeight, cx, cy, cr);
    }
    // down + left
    void PushOut12(float bx, float by, float bHalfWidth, float bHalfHeight, float& cx, float& cy, float cr) {
        MoveCircleIfIntersectsBox<float, false, false, true, true>(bx, by, bHalfWidth, bHalfHeight, cx, cy, cr);
    }
    // up + down + left
    void PushOut13(float bx, float by, float bHalfWidth, float bHalfHeight, float& cx, float& cy, float cr) {
        MoveCircleIfIntersectsBox<float, true, false, true, true>(bx, by, bHalfWidth, bHalfHeight, cx, cy, cr);
    }
    // right + down + left
    void PushOut14(float bx, float by, float bHalfWidth, float bHalfHeight, float& cx, float& cy, float cr) {
        MoveCircleIfIntersectsBox<float, false, true, true, true>(bx, by, bHalfWidth, bHalfHeight, cx, cy, cr);
    }
    // up + right + down + left
    void PushOut15(float bx, float by, float bHalfWidth, float bHalfHeight, float& cx, float& cy, float cr) {
        MoveCircleIfIntersectsBox<float, true, true, true, true>(bx, by, bHalfWidth, bHalfHeight, cx, cy, cr);
    }

    typedef void(*PushOutFunc)(float bx, float by, float bHalfWidth, float bHalfHeight, float& cx, float& cy, float cr);
    inline static PushOutFunc pushOutFuncs[] = {
        PushOut15,
        PushOut1,
        PushOut2,
        PushOut3,
        PushOut4,
        PushOut5,
        PushOut6,
        PushOut7,
        PushOut8,
        PushOut9,
        PushOut10,
        PushOut11,
        PushOut12,
        PushOut13,
        PushOut14,
        PushOut15,
    };

    XX_INLINE void Block::PushOut(Monster& m) {
        auto& p = m.pos;
        auto& r = m.radius;
        auto idx = (uint8_t&)wayout;
        assert(idx < _countof(pushOutFuncs));
        pushOutFuncs[idx](pos.x, pos.y, halfSize.x, halfSize.y, p.x, p.y, r);
    }

};
