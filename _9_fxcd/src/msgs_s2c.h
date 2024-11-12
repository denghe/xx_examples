#pragma once

namespace Msgs {
	namespace S2C {	// id == 200 ~ 299

		// 1 to 1
		struct Join_r : xx::SerdeBase {
			static constexpr uint16_t cTypeId{ 201 }; /////////////////////////////////////////////////////////////////////////////
			static constexpr uint16_t cParentTypeId{ xx::SerdeBase::cTypeId };
			/* S */ void WriteTo(xx::Data& d) const override;
			/* C */ int32_t ReadFrom(xx::Data_r& dr) override;

			int32_t clientId{};
			xx::Shared<Global::Scene> scene;
		};
	}
}

namespace Msgs {
	namespace S2C {	// id == 200 ~ 299

		// 1 to n ( after Join_r )
		struct PlayerJoin : xx::SerdeBase {
			static constexpr uint16_t cTypeId{ 202 }; /////////////////////////////////////////////////////////////////////////////
			static constexpr uint16_t cParentTypeId{ xx::SerdeBase::cTypeId };
			/* S */ void WriteTo(xx::Data& d) const override;
			/* C */ int32_t ReadFrom(xx::Data_r& dr) override;

			int32_t clientId{};
			int64_t frameNumber{};
		};
	}
}

namespace Msgs {
	namespace S2C {	// id == 200 ~ 299

		// 1 to n ( after player disconnect )
		struct PlayerLeave : xx::SerdeBase {
			static constexpr uint16_t cTypeId{ 203 }; /////////////////////////////////////////////////////////////////////////////
			static constexpr uint16_t cParentTypeId{ xx::SerdeBase::cTypeId }; 
			/* S */ void WriteTo(xx::Data& d) const override;
			/* C */ int32_t ReadFrom(xx::Data_r& dr) override;

			int32_t clientId{};
			int64_t frameNumber{};
		};
	}
}

namespace Msgs {
	namespace S2C {	// id == 200 ~ 299

		// 1 to n
		struct Summon : xx::SerdeBase {
			static constexpr uint16_t cTypeId{ 204 }; /////////////////////////////////////////////////////////////////////////////
			static constexpr uint16_t cParentTypeId{ xx::SerdeBase::cTypeId };
			/* S */ void WriteTo(xx::Data& d) const override;
			/* C */ int32_t ReadFrom(xx::Data_r& dr) override;

			int32_t clientId{};
			int64_t frameNumber{};
			xx::XYi bornPos;
		};

	}
}
