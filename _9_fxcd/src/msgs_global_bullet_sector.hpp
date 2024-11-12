#pragma once

namespace Msgs {
	namespace Global {

        inline void Bullet_Sector::WriteTo(xx::Data& d) const {
            Base::WriteTo(d);
            d.Write(
                radius, theta
            );
        }

        inline int32_t Bullet_Sector::ReadFrom(xx::Data_r& dr) {
            if (auto r = Base::ReadFrom(dr); r) return r;
            return dr.Read(
                radius, theta
            );
        }

        // todo: damage args
        inline Bullet_Sector& Bullet_Sector::Init(Scene* scene_, XYp const& pos_, FX64 radians_, FX64 radius_, FX64 theta_) {
            Base::Init(scene_, pos_, radians_, 5);
            radius = radius_;
            theta = theta_;
            return *this;
        }

        inline int32_t Bullet_Sector::Update() {
            // rotate
            radians += cRotateStep;
            if (radians > FX64_PI) {
                radians = FX64_N2PI + radians;
            }
            FillDirectionByRadians();
            auto& monsters = scene->monsters;

            HitBlackListClear(cPierceDelay);

#if 1
            // search monster by range
            scene->monsterSpace.ForeachByRange(gLooper.sgrdd, pos.x.ToInt(), pos.y.ToInt(), radius.ToInt(), [&](Monster* m) {
                if (xx::Math::IsSectorCircleIntersect<XYp>(pos, radius, direction, theta, m->pos, m->radius)) {
                    if (HitBlackListTryAdd(cPierceDelay, m)) {
                        m->Hurt(this);
                    }
                }
            });
#else
            // foreach monster
            for (int32_t i = monsters.len - 1; i >= 0; --i) {
                auto& m = monsters[i];
                if (xx::Math::IsSectorCircleIntersect<XYp>(pos, radius, direction, theta, m->pos, m->radius)) {
                    if (HitBlackListTryAdd(cPierceDelay, m)) {
                        m->Hurt(this);
                    }
                }
            }
#endif

            return 0;
        }

        inline void Bullet_Sector::Draw() {
            auto p = gLooper.camera.ToGLPos(pos.As<float>());
            auto r = radius.ToFloat();
            auto u = radians.ToFloat();
            auto a = theta.ToFloat();

            xx::LineStrip{}.FillSectorPoints({}, r, u, a, 5)
                .SetPosition(p)
                .SetScale(gLooper.camera.scale)
                .SetRotate(radians.ToFloat())
                .Draw();
        }

	}
}
