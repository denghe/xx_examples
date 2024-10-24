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

        bool Bullet_Sector::IntersectCircle(XYp const& c, FX64 r) {
            return false;
        }

        void Bullet_Sector::Draw() {
            // todo
        }

	}
}
