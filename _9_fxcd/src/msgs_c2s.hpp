#pragma once

namespace Msgs {
	namespace C2S {	// id == 1000 ~ 1999

		inline int32_t Join::ReadFrom(xx::Data_r& dr) {
			return 0;
		}

		inline void Join::WriteTo(xx::Data& d) const {
		}


		inline int32_t Msgs::C2S::Summon::ReadFrom(xx::Data_r& dr) {
			return dr.Read(bornPos);
		}

		inline void Msgs::C2S::Summon::WriteTo(xx::Data& d) const {
			d.Write(bornPos);
		}

	}
}
