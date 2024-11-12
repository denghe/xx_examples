#pragma once

namespace Msgs {
	namespace Global {	// id = 1 ~ 99

		struct Bullet_Base : Projectile {
			using Base = Projectile;
			static constexpr uint16_t cTypeId{ 8 }; /////////////////////////////////////////////////////////////////////////////
			static constexpr uint16_t cParentTypeId{ xx::SerdeBase::cTypeId };
			/* S */ void WriteTo(xx::Data& d) const override;
			/* C */ int32_t ReadFrom(xx::Data_r& dr) override;

			static constexpr int32_t cPierceDelay{ uint32_t(Looper::fps) >> 3 };

			int32_t damage{};
			xx::Listi32<std::pair<xx::Weak<Monster>, int64_t>> hitBlackList;

			void Init(Scene* scene_, XYp const& pos_, FX64 radians_, int32_t damage_);
			void HitBlackListClear(int32_t pierceDelay_);
			bool HitBlackListTryAdd(int32_t pierceDelay_, Monster* m);
			// todo
		};

	}
}
