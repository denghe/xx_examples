#include "pch.h"
#include "looper.h"
#include "msgs.h"

namespace Msgs {
	namespace C2S {	// id == 1000 ~ 1999

		int32_t Join::ReadFrom(xx::Data_r& dr) {
			return 0;
		}

		void Join::WriteTo(xx::Data& d) const {
		}


		int32_t Msgs::C2S::Summon::ReadFrom(xx::Data_r& dr) {
			return dr.Read(bornPos);
		}

		void Msgs::C2S::Summon::WriteTo(xx::Data& d) const {
			d.Write(bornPos);
		}

	}
}
