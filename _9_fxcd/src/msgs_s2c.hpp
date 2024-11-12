#pragma once

namespace Msgs {
	namespace S2C {	// id == 2000 ~ 2999

		inline int32_t Join_r::ReadFrom(xx::Data_r& dr) {
			return dr.Read(clientId, scene);
		}

		inline void Join_r::WriteTo(xx::Data& d) const {
			d.Write(clientId, scene);
		}


		inline int32_t PlayerJoin::ReadFrom(xx::Data_r& dr) {
			return dr.Read(clientId, frameNumber);
		}

		inline void PlayerJoin::WriteTo(xx::Data& d) const {
			d.Write(clientId, frameNumber);
		}


		inline int32_t PlayerLeave::ReadFrom(xx::Data_r& dr) {
			return dr.Read(clientId, frameNumber);
		}

		inline void PlayerLeave::WriteTo(xx::Data& d) const {
			d.Write(clientId, frameNumber);
		}


		inline void Summon::WriteTo(xx::Data& d) const {
			d.Write(clientId, frameNumber, bornPos);
		}

		inline int32_t Summon::ReadFrom(xx::Data_r& dr) {
			return dr.Read(clientId, frameNumber, bornPos);
		}

	}
}
