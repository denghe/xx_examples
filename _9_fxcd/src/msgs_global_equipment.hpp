#pragma once

namespace Msgs {
	namespace Global {

		inline void Equipment::WriteTo(xx::Data& d) const {
			d.Write(owner, stats);
		}

		inline int32_t Equipment::ReadFrom(xx::Data_r& dr) {
			return dr.Read(owner, stats);
		}

	}
}
