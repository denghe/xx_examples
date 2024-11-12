#pragma once

namespace Msgs {
	namespace Global {	// id = 1 ~ 99

		struct Bullet_Box : Bullet_Base {
			using Base = Bullet_Base;
			static constexpr uint16_t cTypeId{ 10 }; /////////////////////////////////////////////////////////////////////////////
			static constexpr uint16_t cParentTypeId{ Bullet_Base::cTypeId };
			/* S */ void WriteTo(xx::Data& d) const override;
			/* C */ int32_t ReadFrom(xx::Data_r& dr) override;

			static constexpr FX64 cRotateStep{ FX64{0.05} / Scene::fps60ratio };

			XYp size{};

			Bullet_Box& Init(Scene* scene_, XYp const& pos_, FX64 radians_, XYp const& size_);
			int32_t Update() override;	// non zero: kill
			void Draw() override;
		};

	}
}
