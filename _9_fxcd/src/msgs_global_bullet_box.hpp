#pragma once

namespace Msgs {
	namespace Global {

        inline void Bullet_Box::WriteTo(xx::Data& d) const {
            Base::WriteTo(d);
            d.Write(
                size
            );
        }

        inline int32_t Bullet_Box::ReadFrom(xx::Data_r& dr) {
            if (auto r = Base::ReadFrom(dr); r) return r;
            return dr.Read(
                size
            );
        }

        inline Bullet_Box& Bullet_Box::Init(Scene* scene_, XYp const& pos_, FX64 radians_, XYp const& size_) {
            Base::Init(scene_, pos_, radians_, 5);
            size = size_;
            return *this;
        }

        inline int32_t Bullet_Box::Update() {
            // rotate
            radians += cRotateStep;
            if (radians > FX64_PI) {
                radians = FX64_N2PI + radians;
            }
            FillDirectionByRadians();
            auto& monsters = scene->monsters;

            HitBlackListClear(cPierceDelay);

            auto radius = size.x.ToInt();   // todo: + min(size.x, size.y) / 2 * 1.4 ?

#if 1
            // search monster by range
            scene->monsterSpace.ForeachByRange(gLooper.sgrdd, pos.x.ToInt(), pos.y.ToInt(), radius, [&](Monster* m) {
                if (xx::Math::IsBoxCircleIntersect<XYp>(pos, size, direction, m->pos, m->radius)) {
                    if (HitBlackListTryAdd(cPierceDelay, m)) {
                        m->Hurt(this);
                    }
                }
            });
#else
            for (int32_t i = monsters.len - 1; i >= 0; --i) {
                auto& m = monsters[i];
                if (xx::Math::IsBoxCircleIntersect<XYp>(pos, size, direction, m->pos, m->radius)) {
                    if (HitBlackListTryAdd(cPierceDelay, m)) {
                        m->Hurt(this);
                    }
                }
            }
#endif

            return 0;
        }

        inline void Bullet_Box::Draw() {
            auto p = gLooper.camera.ToGLPos(pos.As<float>());
            auto wh = size.As<float>();
            xx::LineStrip{}.FillBoxPoints({}, wh)
                .SetPosition(p)
                .SetScale(gLooper.camera.scale)
                .SetRotate(radians.ToFloat())
                .Draw();
        }

	}
}
