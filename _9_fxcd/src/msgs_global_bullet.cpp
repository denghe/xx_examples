#include "pch.h"
#include "looper.h"
#include "msgs.h"

namespace Msgs {
	namespace Global {

        void Bullet_Sector::WriteTo(xx::Data& d) const {
            d.Write(
                scene, pos, radius, radians, theta
            );
        }

        int32_t Bullet_Sector::ReadFrom(xx::Data_r& dr) {
            return dr.Read(
                scene, pos, radius, radians, theta
            );
        }

        Bullet_Sector& Bullet_Sector::Init(Scene* scene_, XYp const& pos_, FX64 radius_, FX64 radians_, FX64 theta_) {
            scene = xx::WeakFromThis(scene_);
            pos = pos_;
            radius = radius_;
            radians = radians_;
            theta = theta_;
            return *this;
        }

        int32_t Bullet_Sector::Update() {
            XYp direction{ radians.CosFastest(), radians.SinFastest() };
            auto& monsters = scene->monsters;
            for (int32_t i = monsters.len - 1; i >= 0; --i) {
                auto& m = monsters[i];
                if (xx::Math::IsSectorCircleIntersect<XYp>(pos, radius, direction, theta, m->pos, m->radius)) {
                    // todo: effect
                    monsters.SwapRemoveAt(i);
                }
            }

            return 0;
        }

        void Bullet_Sector::Draw() {
            auto p = gLooper.camera.ToGLPos(pos.As<float>());
            auto r = radius.ToFloat();
            auto u = radians.ToFloat();
            auto a = theta.ToFloat();

            xx::LineStrip{}.FillSectorPoints(p,r,u,a, 100, gLooper.camera.scale).Draw();
        }

	}
}
