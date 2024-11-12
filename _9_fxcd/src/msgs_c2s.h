#pragma once

namespace Msgs {
	namespace C2S {	// id == 100 ~ 199

		// todo: list/array data length limit protect

		struct Join : xx::SerdeBase {
			static constexpr uint16_t cTypeId{ 101 }; /////////////////////////////////////////////////////////////////////////////
			static constexpr uint16_t cParentTypeId{ xx::SerdeBase::cTypeId };
			/* S */ void WriteTo(xx::Data& d) const override;
			/* C */ int32_t ReadFrom(xx::Data_r& dr) override;
		};
	}
}

namespace Msgs {
	namespace C2S {	// id == 100 ~ 199

		struct Summon : xx::SerdeBase {
			static constexpr uint16_t cTypeId{ 102 }; /////////////////////////////////////////////////////////////////////////////
			static constexpr uint16_t cParentTypeId{ xx::SerdeBase::cTypeId };
			/* S */ void WriteTo(xx::Data& d) const override;
			/* C */ int32_t ReadFrom(xx::Data_r& dr) override;

			xx::XYi bornPos;
		};

	}
}
