#pragma once
#define STB_VORBIS_HEADER_ONLY
#include "stb_vorbis.c"

#ifdef _MSC_VER
#include <xaudio2.h>
#pragma comment(lib, "xaudio2.lib")
#else
#include "backend/xaudio2/xaudio2.h"
#endif

// todo: support xaudio2 & web audio api
// todo: support ogg playback
// todo: copy code from soloud

namespace xx {
	using AudioLength = double;

	struct AudioSource;
	struct AudioSourceInstance;
	struct XAudio2Data;
	struct VoiceCallback;

	struct AlignedFloatBuffer {
		float* data{};							// aligned for SSE
		std::unique_ptr<char[]> rawData;
		size_t numFloats{};

		void Init(size_t numFloats_);
		void Clear();
	};

	struct Sound {
		static constexpr int32_t numMaxChannels{ 8 };
		static constexpr int32_t numChannels{ 2 };
		static constexpr uint32_t samplerate{ 44100 };
		static constexpr uint32_t bufferSize{ 4096 };
		static constexpr uint32_t totalBufferSize{ bufferSize * numChannels };

		Shared<XAudio2Data> data;

		float globalVolume{};

		int32_t Init();
		Shared<AudioSource> CreateAudioSource(Span oggFileData);
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

	struct XAudio2Data {
		static constexpr int32_t numBuffers{ 2 };
		
		std::array<std::unique_ptr<float[]>, numBuffers> buffer;
		IXAudio2* xaudio2{};
		IXAudio2MasteringVoice* masteringVoice{};
		IXAudio2SourceVoice* sourceVoice{};
		HANDLE bufferEndEvent{};
		HANDLE audioProcessingDoneEvent{};
		VoiceCallback* voiceCb{};
		int samples{};
		UINT32 bufferLengthBytes{};
		Weak<Sound> sound;
		std::jthread thread;
	};

	struct VoiceCallback : IXAudio2VoiceCallback {
		VoiceCallback(HANDLE bufferEndEvent) : bufferEndEvent(bufferEndEvent) {}
		virtual ~VoiceCallback() {}
		void __stdcall OnVoiceProcessingPassStart(UINT32 BytesRequired) override {}
		void __stdcall OnVoiceProcessingPassEnd() override {}
		void __stdcall OnStreamEnd() override {}
		void __stdcall OnBufferStart(void* pBufferContext) override {}
		void __stdcall OnBufferEnd(void* pBufferContext) override { SetEvent(bufferEndEvent); }
		void __stdcall OnLoopEnd(void* pBufferContext) override {}
		void __stdcall OnVoiceError(void* pBufferContext, HRESULT Error) override {}
		HANDLE bufferEndEvent{};
	};

}

/********************************************************************************/

namespace xx {


	inline void AlignedFloatBuffer::Init(size_t numFloats_) {
		numFloats = numFloats_;
		rawData = std::make_unique_for_overwrite<char[]>(numFloats_ * sizeof(float) + 16);
		data = (float*)(((size_t)rawData.get() + 15) & ~15);
	}

	inline void AlignedFloatBuffer::Clear() {
		memset(data, 0, sizeof(float) * numFloats);
	}


	/********************************************************************************/

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

	inline int32_t Sound::Init() {
		if (FAILED(CoInitializeEx(0, COINIT_MULTITHREADED))) return __LINE__;

		data.Emplace();
		data->bufferEndEvent = CreateEvent(0, FALSE, FALSE, 0);
		if (0 == data->bufferEndEvent) return __LINE__;
		data->audioProcessingDoneEvent = CreateEvent(0, FALSE, FALSE, 0);
		if (0 == data->audioProcessingDoneEvent) return __LINE__;
		WAVEFORMATEX format{};
		format.wFormatTag = WAVE_FORMAT_IEEE_FLOAT;
		format.nChannels = 2;
		format.nSamplesPerSec = samplerate;
		format.nAvgBytesPerSec = samplerate * sizeof(float) * format.nChannels;
		format.nBlockAlign = sizeof(float) * format.nChannels;
		format.wBitsPerSample = sizeof(float) * 8;
		if (FAILED(XAudio2Create(&data->xaudio2))) return __LINE__;
		if (FAILED(data->xaudio2->CreateMasteringVoice(&data->masteringVoice, format.nChannels, samplerate))) return __LINE__;
		data->voiceCb = new VoiceCallback(data->bufferEndEvent);
		if (FAILED(data->xaudio2->CreateSourceVoice(&data->sourceVoice, &format, XAUDIO2_VOICE_NOSRC | XAUDIO2_VOICE_NOPITCH, 2.f, data->voiceCb))) return __LINE__;
		data->bufferLengthBytes = bufferSize * format.nChannels * sizeof(float);
		for (int i = 0; i < data->numBuffers; ++i) {
			data->buffer[i] = std::make_unique_for_overwrite<float[]>(bufferSize * format.nChannels);
		}
		data->samples = bufferSize;
		data->sound = WeakFromThis(this);
		//aSoloud->postinit_internal(aSamplerate, aBuffer * format.nChannels, aFlags, numChannels);
		//data->thread = Thread::createThread(xaudio2Thread, data);
		data->sourceVoice->Start();

		return 0;
	}

	inline Shared<AudioSource> Sound::CreateAudioSource(Span oggFileData) {
		Shared<AudioSource> r;
		if (auto vorbis = stb_vorbis_open_memory(oggFileData.buf, (int)oggFileData.len, nullptr, nullptr)) {
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
