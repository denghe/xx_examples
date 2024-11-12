#pragma once

namespace Msgs {
	namespace Global {

		struct Equipment : xx::SerdeBase {
			static constexpr uint16_t cTypeId{ 3 }; /////////////////////////////////////////////////////////////////////////////
			static constexpr uint16_t cParentTypeId{ xx::SerdeBase::cTypeId };
			/* S */ void WriteTo(xx::Data& d) const override;
			/* C */ int32_t ReadFrom(xx::Data_r& dr) override;

			xx::Weak<Monster> owner;
			xx::TinyList<StatItem> stats;
			// ...
		};

	}
}
