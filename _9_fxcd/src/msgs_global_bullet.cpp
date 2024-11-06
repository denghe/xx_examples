#include "pch.h"
#include "looper.h"
#include "msgs.h"

namespace Msgs {
	namespace Global {

        void Bullet_Base::Init(Scene* scene_, XYp const& pos_, FX64 radians_) {
            assert(scene_->bullets.Empty() || scene_->bullets.Top().pointer != this);	// auto add check
            scene_->bullets.Emplace(xx::SharedFromThis(this));
            scene = xx::WeakFromThis(scene_);
            pos = pos_;
            radians = radians_;
            FillDirectionByRadians();
        }

        void Bullet_Base::FillDirectionByRadians() {
            direction.x = radians.CosFastest();
            direction.y = radians.SinFastest();
        }

        void Bullet_Base::WriteTo(xx::Data& d) const {
            d.Write(
                scene, pos, radians
            );
        }

        int32_t Bullet_Base::ReadFrom(xx::Data_r& dr) {
            if (auto r = dr.Read(
                scene, pos, radians
            ); r) return r;
            FillDirectionByRadians();
            return 0;
        }

        /*********************************************************************************************/

        void Bullet_Sector::WriteTo(xx::Data& d) const {
            Base::WriteTo(d);
            d.Write(
                radius, theta
            );
        }

        int32_t Bullet_Sector::ReadFrom(xx::Data_r& dr) {
            if (auto r = Base::ReadFrom(dr); r) return r;
            return dr.Read(
                radius, theta
            );
        }

        Bullet_Sector& Bullet_Sector::Init(Scene* scene_, XYp const& pos_, FX64 radians_, FX64 radius_, FX64 theta_) {
            Base::Init(scene_, pos_, radians_);
            radius = radius_;
            theta = theta_;
            return *this;
        }

        int32_t Bullet_Sector::Update() {
            // rotate
            radians += cRotateStep;
            if (radians > FX64_PI) {
                radians = FX64_N2PI + radians;
            }
            FillDirectionByRadians();
            auto& monsters = scene->monsters;

#if 1
            // search monster by range
            scene->monsterSpace.ForeachByRange(gLooper.sgrdd, pos.x.ToInt(), pos.y.ToInt(), radius.ToInt(), [&](Monster* m) {
                if (xx::Math::IsSectorCircleIntersect<XYp>(pos, radius, direction, theta, m->pos, m->radius)) {
                    // todo: effect
                    m->Remove();
                }
            });
#else
            // foreach monster
            for (int32_t i = monsters.len - 1; i >= 0; --i) {
                auto& m = monsters[i];
                if (xx::Math::IsSectorCircleIntersect<XYp>(pos, radius, direction, theta, m->pos, m->radius)) {
                    // todo: effect
                    m->Remove();
                }
            }
#endif

            return 0;
        }

        void Bullet_Sector::Draw() {
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

        /*********************************************************************************************/

        void Bullet_Box::WriteTo(xx::Data& d) const {
            Base::WriteTo(d);
            d.Write(
                size
            );
        }

        int32_t Bullet_Box::ReadFrom(xx::Data_r& dr) {
            if (auto r = Base::ReadFrom(dr); r) return r;
            return dr.Read(
                size
            );
        }

        Bullet_Box& Bullet_Box::Init(Scene* scene_, XYp const& pos_, FX64 radians_, XYp const& size_) {
            Base::Init(scene_, pos_, radians_);
            size = size_;
            return *this;
        }

        int32_t Bullet_Box::Update() {
            // rotate
            radians += cRotateStep;
            if (radians > FX64_PI) {
                radians = FX64_N2PI + radians;
            }
            FillDirectionByRadians();
            auto& monsters = scene->monsters;

            auto radius = size.x.ToInt();   // todo: + min(size.x, size.y) / 2 * 1.4 ?

#if 1
            // search monster by range
            scene->monsterSpace.ForeachByRange(gLooper.sgrdd, pos.x.ToInt(), pos.y.ToInt(), radius, [&](Monster* m) {
                if (xx::Math::IsBoxCircleIntersect<XYp>(pos, size, direction, m->pos, m->radius)) {
                    // todo: effect
                    m->Remove();
                }
            });
#else
            for (int32_t i = monsters.len - 1; i >= 0; --i) {
                auto& m = monsters[i];
                if (xx::Math::IsBoxCircleIntersect<XYp>(pos, size, direction, m->pos, m->radius)) {
                    // todo: effect
                    m->Remove();
                }
            }
#endif

            return 0;
        }

        void Bullet_Box::Draw() {
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
