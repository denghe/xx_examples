#pragma once

namespace Msgs {
	namespace Global {	// id = 1 ~ 99

		struct Projectile : xx::SerdeBase {
			static constexpr uint16_t cTypeId{ 5 }; /////////////////////////////////////////////////////////////////////////////
			static constexpr uint16_t cParentTypeId{ xx::SerdeBase::cTypeId };
			/* S */ void WriteTo(xx::Data& d) const override;
			/* C */ int32_t ReadFrom(xx::Data_r& dr) override;

			xx::Weak<Scene> scene;
			XYp pos{};
			FX64 radians{};
			/* T */ XYp direction{};	// { cos(radians), sin(radians) }

			void FillDirectionByRadians();
			virtual int32_t Update() { assert(false); return 0; };
			virtual void Draw() { assert(false); };
		};

	}
}
