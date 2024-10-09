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


		void Summon_r::WriteTo(xx::Data& d) const {
			data.WriteTo(d);
		}

		int32_t Summon_r::ReadFrom(xx::Data_r& dr) {
			return data.ReadFrom(dr);
		}

	}
}
