#pragma once
#define STB_VORBIS_HEADER_ONLY
#include "stb_vorbis.c"

#ifdef _MSC_VER
#include <xaudio2.h>
#pragma comment(lib, "xaudio2.lib")
#else
#include "backend/xaudio2/xaudio2.h"
#endif

#include "xx_aligned.h"

// todo: support xaudio2 & web audio api
// todo: support ogg playback
// todo: copy code from soloud
// todo: refine( inline -> XX_INLINE  rename ... )

namespace xx {
	using AudioLength = double;

	struct AudioSource;
	struct AudioSourceInstance;
	struct XAudio2Data;
	struct VoiceCallback;
	using AlignedFloatBuffer = AlignedBuffer<float, 16, true>;
	using TinyAlignedFloatBuffer = AlignedBuffer<float, 16, false>;

	struct Sound {
		static constexpr int32_t numMaxChannels{ 8 };
		static constexpr int32_t numChannels{ 2 };
		static constexpr uint32_t samplerate{ 44100 };
		static constexpr uint32_t bufferSize{ 4096 };
		static constexpr uint32_t totalBufferSize{ bufferSize * numChannels };

		Shared<XAudio2Data> data;

		// todo: more members....
		AlignedFloatBuffer mScratch;

		int32_t Init();
		Shared<AudioSource> CreateAudioSource(Span oggFileData);

		void mix(float* aBuffer, unsigned int aSamples);
		void mix_internal(unsigned int aSamples);
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

	inline void interlace_samples_float(const float* aSourceBuffer, float* aDestBuffer, unsigned int aSamples, unsigned int aChannels) {
		// 111222 -> 121212
		unsigned int i, j, c;
		c = 0;
		for (j = 0; j < aChannels; j++) {
			for (i = j; i < aSamples * aChannels; i += aChannels) {
				aDestBuffer[i] = aSourceBuffer[c];
				c++;
			}
		}
	}

	/********************************************************************************/

	inline Shared<AudioSourceInstance> AudioSource::CreateInstance() {
		return MakeShared<AudioSourceInstance>(this);
	}

	inline AudioLength AudioSource::GetLength() {
		// todo
		return 0;
	}

	/********************************************************************************/

	inline AudioSourceInstance::AudioSourceInstance(AudioSource* parent_) {
		parent = WeakFromThis(parent_);
		// todo
	}


	/********************************************************************************/

	inline int32_t Sound::Init() {
		if (FAILED(CoInitializeEx(0, COINIT_MULTITHREADED))) return __LINE__;

		//mInsideAudioThreadMutex = false;
		//mScratchSize = 0;
		//mScratchNeeded = 0;
		//mSamplerate = 0;
		//mBufferSize = 0;
		//mFlags = 0;
		//mGlobalVolume = 0;
		//mPlayIndex = 0;
		//mBackendData = NULL;
		//mAudioThreadMutex = NULL;
		//mPostClipScaler = 0;
		//mBackendCleanupFunc = NULL;
		//mChannels = 2;
		//mStreamTime = 0;
		//mLastClockedTime = 0;
		//mAudioSourceID = 1;
		//mBackendString = 0;
		//mBackendID = 0;
		//mActiveVoiceDirty = true;
		//mActiveVoiceCount = 0;
		//int i;
		//for (i = 0; i < VOICE_COUNT; i++)
		//	mActiveVoice[i] = 0;
		//for (i = 0; i < FILTERS_PER_STREAM; i++) {
		//	mFilter[i] = NULL;
		//	mFilterInstance[i] = NULL;
		//}
		//for (i = 0; i < 256; i++) {
		//	mFFTData[i] = 0;
		//	mVisualizationWaveData[i] = 0;
		//	mWaveData[i] = 0;
		//}
		//for (i = 0; i < MAX_CHANNELS; i++) {
		//	mVisualizationChannelVolume[i] = 0;
		//}
		//for (i = 0; i < VOICE_COUNT; i++) {
		//	mVoice[i] = 0;
		//}
		//mVoiceGroup = 0;
		//mVoiceGroupCount = 0;

		//m3dPosition[0] = 0;
		//m3dPosition[1] = 0;
		//m3dPosition[2] = 0;
		//m3dAt[0] = 0;
		//m3dAt[1] = 0;
		//m3dAt[2] = -1;
		//m3dUp[0] = 0;
		//m3dUp[1] = 1;
		//m3dUp[2] = 0;
		//m3dVelocity[0] = 0;
		//m3dVelocity[1] = 0;
		//m3dVelocity[2] = 0;
		//m3dSoundSpeed = 343.3f;
		//mMaxActiveVoices = 16;
		//mHighestVoice = 0;
		//mResampleData = NULL;
		//mResampleDataOwner = NULL;
		//for (i = 0; i < 3 * MAX_CHANNELS; i++)
		//	m3dSpeakerPosition[i] = 0;

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
		// aSoloud->postinit_internal(aSamplerate, aBuffer * format.nChannels, aFlags, numChannels);		// todo
		data->thread = std::jthread{ [data = data.pointer] {
			int bufferIndex = 0;
			while (WAIT_OBJECT_0 != WaitForSingleObject(data->audioProcessingDoneEvent, 0)) {
				XAUDIO2_VOICE_STATE state;
				data->sourceVoice->GetState(&state);
				while (state.BuffersQueued < XAudio2Data::numBuffers) {
					data->sound->mix(data->buffer[bufferIndex].get(), data->samples);
					XAUDIO2_BUFFER info = {0};
					info.AudioBytes = data->bufferLengthBytes;
					info.pAudioData = (const BYTE*)(data->buffer[bufferIndex].get());
					data->sourceVoice->SubmitSourceBuffer(&info);
					++bufferIndex;
					if (bufferIndex >= XAudio2Data::numBuffers) {
						bufferIndex = 0;
					}
					data->sourceVoice->GetState(&state);
				}
				WaitForSingleObject(data->bufferEndEvent, INFINITE);
			}
		} };
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

	inline void Sound::mix(float* aBuffer, unsigned int aSamples) {
		mix_internal(aSamples);
		interlace_samples_float(mScratch, aBuffer, aSamples, numChannels);
	}

	inline void Sound::mix_internal(unsigned int aSamples) {

#ifdef _MCW_DN
		{
			static bool once = false;
			if (!once) {
				once = true;
				if (!(mFlags & NO_FPU_REGISTER_CHANGE)) {
					_controlfp(_DN_FLUSH, _MCW_DN);
				}
			}
		}
#endif

		{
			static bool once = false;
			if (!once) {
				once = true;
				// Set denorm clear to zero (CTZ) and denorms are zero (DAZ) flags on.
				// This causes all math to consider really tiny values as zero, which
				// helps performance. I'd rather use constants from the sse headers,
				// but for some reason the DAZ value is not defined there(!)
				if (!(mFlags & NO_FPU_REGISTER_CHANGE)) {
					_mm_setcsr(_mm_getcsr() | 0x8040);
				}
			}
		}

		float buffertime = aSamples / (float)mSamplerate;
		float globalVolume[2];
		mStreamTime += buffertime;
		mLastClockedTime = 0;

		globalVolume[0] = mGlobalVolume;
		if (mGlobalVolumeFader.mActive) {
			mGlobalVolume = mGlobalVolumeFader.get(mStreamTime);
		}
		globalVolume[1] = mGlobalVolume;

		lockAudioMutex_internal();

		// Process faders. May change scratch size.
		int i;
		for (i = 0; i < (signed)mHighestVoice; i++) {
			if (mVoice[i] && !(mVoice[i]->mFlags & AudioSourceInstance::PAUSED)) {
				float volume[2];

				mVoice[i]->mActiveFader = 0;

				if (mGlobalVolumeFader.mActive > 0) {
					mVoice[i]->mActiveFader = 1;
				}

				mVoice[i]->mStreamTime += buffertime;
				mVoice[i]->mStreamPosition += (double)buffertime * (double)mVoice[i]->mOverallRelativePlaySpeed;

				// TODO: this is actually unstable, because mStreamTime depends on the relative
				// play speed. 
				if (mVoice[i]->mRelativePlaySpeedFader.mActive > 0) {
					float speed = mVoice[i]->mRelativePlaySpeedFader.get(mVoice[i]->mStreamTime);
					setVoiceRelativePlaySpeed_internal(i, speed);
				}

				volume[0] = mVoice[i]->mOverallVolume;
				if (mVoice[i]->mVolumeFader.mActive > 0) {
					mVoice[i]->mSetVolume = mVoice[i]->mVolumeFader.get(mVoice[i]->mStreamTime);
					mVoice[i]->mActiveFader = 1;
					updateVoiceVolume_internal(i);
					mActiveVoiceDirty = true;
				}
				volume[1] = mVoice[i]->mOverallVolume;

				if (mVoice[i]->mPanFader.mActive > 0) {
					float pan = mVoice[i]->mPanFader.get(mVoice[i]->mStreamTime);
					setVoicePan_internal(i, pan);
					mVoice[i]->mActiveFader = 1;
				}

				if (mVoice[i]->mPauseScheduler.mActive) {
					mVoice[i]->mPauseScheduler.get(mVoice[i]->mStreamTime);
					if (mVoice[i]->mPauseScheduler.mActive == -1) {
						mVoice[i]->mPauseScheduler.mActive = 0;
						setVoicePause_internal(i, 1);
					}
				}

				if (mVoice[i]->mStopScheduler.mActive) {
					mVoice[i]->mStopScheduler.get(mVoice[i]->mStreamTime);
					if (mVoice[i]->mStopScheduler.mActive == -1) {
						mVoice[i]->mStopScheduler.mActive = 0;
						stopVoice_internal(i);
					}
				}
			}
		}

		if (mActiveVoiceDirty)
			calcActiveVoices_internal();

		// Resize scratch if needed.
		if (mScratchSize < mScratchNeeded) {
			mScratchSize = mScratchNeeded;
			mScratch.init(mScratchSize * MAX_CHANNELS);
		}

		mixBus_internal(mOutputScratch.mData, aSamples, aSamples, mScratch.mData, 0, (float)mSamplerate, mChannels);

		for (i = 0; i < FILTERS_PER_STREAM; i++) {
			if (mFilterInstance[i]) {
				mFilterInstance[i]->filter(mOutputScratch.mData, aSamples, mChannels, (float)mSamplerate, mStreamTime);
			}
		}

		unlockAudioMutex_internal();

		clip_internal(mOutputScratch, mScratch, aSamples, globalVolume[0], globalVolume[1]);

		if (mFlags & ENABLE_VISUALIZATION) {
			for (i = 0; i < MAX_CHANNELS; i++) {
				mVisualizationChannelVolume[i] = 0;
			}
			if (aSamples > 255) {
				for (i = 0; i < 256; i++) {
					int j;
					mVisualizationWaveData[i] = 0;
					for (j = 0; j < (signed)mChannels; j++) {
						float sample = mScratch.mData[i + j * aSamples];
						float absvol = (float)fabs(sample);
						if (mVisualizationChannelVolume[j] < absvol)
							mVisualizationChannelVolume[j] = absvol;
						mVisualizationWaveData[i] += sample;
					}
				}
			} else {
				// Very unlikely failsafe branch
				for (i = 0; i < 256; i++) {
					int j;
					mVisualizationWaveData[i] = 0;
					for (j = 0; j < (signed)mChannels; j++) {
						float sample = mScratch.mData[(i % aSamples) + j * aSamples];
						float absvol = (float)fabs(sample);
						if (mVisualizationChannelVolume[j] < absvol)
							mVisualizationChannelVolume[j] = absvol;
						mVisualizationWaveData[i] += sample;
					}
				}
			}
		}
	}
}
