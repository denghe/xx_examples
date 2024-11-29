#pragma once
#define STB_VORBIS_HEADER_ONLY
#include "stb_vorbis.c"

// todo: support xaudio2 & web audio api
// todo: support ogg playback
// todo: copy code from soloud

namespace xx {
	using AudioLength = double;

	struct AudioSource;
	struct AudioSourceInstance;

	struct Sound {
		static constexpr int32_t numMaxChannels{ 8 };
		void Init();
		Shared<AudioSource> CreateAudioSourceFromOgg(Span fileData);
	};

	struct AudioSource {
		float baseSampleRate{};
		int32_t numChannels{};
		int32_t numSamples{};
		std::unique_ptr<float[]> data;
		Shared<AudioSourceInstance> CreateInstance();
		AudioLength GetLength();
	};

	struct AudioSourceInstance {
		Weak<AudioSource> parent;
		int32_t offset{};
		AudioSourceInstance(AudioSource* parent_);
	};

}

/********************************************************************************/

namespace xx {

	inline Shared<AudioSourceInstance> AudioSource::CreateInstance() {
		return MakeShared<AudioSourceInstance>(this);
	}

	inline AudioLength AudioSource::GetLength() {
		return 0;
	}

	/********************************************************************************/

	inline AudioSourceInstance::AudioSourceInstance(AudioSource* parent_) {
		parent = WeakFromThis(parent_);
	}

	/********************************************************************************/

	inline void Sound::Init() {

	}

	inline Shared<AudioSource> Sound::CreateAudioSourceFromOgg(Span d) {
		Shared<AudioSource> r;
		if (auto vorbis = stb_vorbis_open_memory(d.buf, (int)d.len, nullptr, nullptr)) {
			r.Emplace();
			auto info = stb_vorbis_get_info(vorbis);
			r->baseSampleRate = (float)info.sample_rate;
			auto numSamples = stb_vorbis_stream_length_in_samples(vorbis);
			r->numChannels = std::min(numMaxChannels, info.channels);
			r->data = std::make_unique_for_overwrite<float[]>(numSamples * r->numChannels);
			r->numSamples = numSamples;
			numSamples = 0;
			float** outputs;
			while (auto n = stb_vorbis_get_frame_float(vorbis, nullptr, &outputs)) {
				for (int32_t ch = 0; ch < r->numChannels; ch++) {
					memcpy(r->data.get() + numSamples + r->numSamples * ch, outputs[ch], sizeof(float) * n);
				}
				numSamples += n;
			}
			stb_vorbis_close(vorbis);
		}
		return r;
	}

}
