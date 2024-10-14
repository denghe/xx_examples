#include "pch.h"
#include "looper.h"
#include "msgs.h"

namespace Msgs {
	namespace S2C {	// id == 2000 ~ 2999

		int32_t Join_r::ReadFrom(xx::Data_r& dr) {
			return dr.Read(clientId, scene);
		}

		void Join_r::WriteTo(xx::Data& d) const {
			d.Write(clientId, scene);
		}


		int32_t PlayerJoin::ReadFrom(xx::Data_r& dr) {
			return dr.Read(clientId, frameNumber);
		}

		void PlayerJoin::WriteTo(xx::Data& d) const {
			d.Write(clientId, frameNumber);
		}


		int32_t PlayerLeave::ReadFrom(xx::Data_r& dr) {
			return dr.Read(clientId, frameNumber);
		}

		void PlayerLeave::WriteTo(xx::Data& d) const {
			d.Write(clientId, frameNumber);
		}


		void Summon::WriteTo(xx::Data& d) const {
			d.Write(clientId, frameNumber, bornPos);
		}

		int32_t Summon::ReadFrom(xx::Data_r& dr) {
			return dr.Read(clientId, frameNumber, bornPos);
		}

	}
}
