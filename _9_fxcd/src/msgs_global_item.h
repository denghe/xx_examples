#pragma once

namespace Msgs {
	namespace Global {

		struct Item : xx::SerdeBase {
			static constexpr uint16_t cTypeId{ 1 }; /////////////////////////////////////////////////////////////////////////////
			static constexpr uint16_t cParentTypeId{ xx::SerdeBase::cTypeId };
			/* S */ void WriteTo(xx::Data& d) const override;
			/* C */ int32_t ReadFrom(xx::Data_r& dr) override;

			// fill by init
			xx::Weak<Scene> scene;
			xx::Weak<Monster> owner;										// empty: on floor

			// fill by logic
			ItemTypes cType;
			ItemQualities cQualitity;
			ItemEquipmentLocations cEquipmentLocation;
			int32_t cQuantityLimit;
			int32_t cCastDelay;
			int32_t quantity;
			int64_t lastCastTime;
			xx::TinyList<StatItem> stats;
			// ...

			void Init(Scene* scene_, Monster* owner_);						// owner_ == null mean on floor
			void MoveTo(Monster* owner_);
			virtual bool Cast(Monster* target) { return false; };			// return true: success
		};

	}
}
