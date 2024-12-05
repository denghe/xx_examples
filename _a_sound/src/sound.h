#pragma once

#include "pch.h"
#include <soloud.h>
#include <soloud_wav.h>
#include <soloud_file.h>

namespace xx {
	struct SoundSource {
		SoLoud::Wav wav;
	};

	struct Sound {
		SoLoud::Soloud soloud;

		Sound() {
			// small buffer for low latency. default is 0 ( auto: 4096 )
			soloud.init(SoLoud::Soloud::CLIP_ROUNDOFF, 0, 0, 1024);
		}

		~Sound() {
			soloud.deinit();
		}

		xx::Shared<SoundSource> Load(Data const& oggFileData, bool looping = false) {
			auto rtv = MakeShared<SoundSource>();
			rtv->wav.loadMem(oggFileData.buf, (unsigned int)oggFileData.len, false, false);
			if (looping) {
				rtv->wav.setLooping(true);
			}
			return rtv;
		}

		int Play(xx::Shared<SoundSource> const& ss, float volume = 1.f, float pan = 0.f, float speed = 1.f) {
			int h = soloud.play(ss->wav, volume, pan);
			if (speed != 1.f) {
				soloud.setRelativePlaySpeed(h, speed);
			}
			return h;
		}

		void Stop(int h) {
			soloud.stop(h);
		}

		void StopAll() {
			soloud.stopAll();
		}

		void SetPauseAll(bool b) {
			soloud.setPauseAll(b);
		}

		// todo: more wrapper functions
	};

}
