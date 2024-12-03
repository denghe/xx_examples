#pragma once

#include "pch.h"
#include <soloud.h>
#include <soloud_wav.h>
#include <soloud_file.h>
#define STB_VORBIS_HEADER_ONLY
#include <stb_vorbis.c>

namespace xx {
	struct SoundSource {
		xx::Data oggFileData;
		SoLoud::Wav wav;
		Listi32<int> handles;		// todo: timeout check? remove unused?

		// todo: release handles ?
	};

	struct Sound {
		SoLoud::Soloud soloud;
		Listi32<xx::Shared<SoundSource>> sources;

		Sound() {
			soloud.init();
		}

		~Sound() {
			for (auto& s : sources) {
				// todo: release
			}
			soloud.deinit();
		}

		// todo: return weak ?
		xx::Shared<SoundSource> Load(Data oggFileData) {
			auto rtv = MakeShared<SoundSource>();
			rtv->oggFileData = std::move(oggFileData);
			rtv->wav.loadMem(rtv->oggFileData.buf, (unsigned int)rtv->oggFileData.len, false, false);
			//rtv->wav.setLooping(true);	// todo
			return rtv;
		}

		int Play(xx::Shared<SoundSource> const& ss, float volume = 1.f, float pan = 0.f, float speed = 1.f) {
			int h = soloud.play(ss->wav, volume, pan);
			//soloud.setVolume(h, volume);
			//soloud.setPan(h, pan);
			if (speed != 1.f) {
				soloud.setRelativePlaySpeed(h, speed);
			}
			ss->handles.Emplace(h);
			return h;
		}

		// todo: stop all ?
	};

}
