#pragma once

namespace Msgs {
	namespace Global {

        inline void Projectile::WriteTo(xx::Data& d) const {
            d.Write(
                scene, pos, radians
            );
        }

        inline int32_t Projectile::ReadFrom(xx::Data_r& dr) {
            if (auto r = dr.Read(
                scene, pos, radians
            ); r) return r;
            FillDirectionByRadians();
            return 0;
        }

        inline void Projectile::FillDirectionByRadians() {
            direction.x = radians.CosFastest();
            direction.y = radians.SinFastest();
        }

	}
}
