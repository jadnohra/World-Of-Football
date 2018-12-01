#ifndef h_WOF_match_SoundManager
#define h_WOF_match_SoundManager

#include "WE3/WEPtr.h"
#include "WE3/audio/WEAudioEngine.h"
#include "WE3/WETL/WETLArray.h"
#include "WE3/defaultImpl/world/WEWorldMaterialManager.h"
using namespace WE;

#include "DataTypes.h"
#include "Clock.h"

namespace WOF { namespace match {

	class Match;

	class SoundManager {
	public:

		enum Sound {

			Sound_None = -1, Sound_Kick, Sound_Dribble, Sound_Tackle, Sound_Fall, Sound_Jump, SoundCount
		};

	public:

		inline AudioEngine* getAudioEngine() { return mAudioEngine.ptr(); }
		inline AudioBufferContainer& getAudioBuffers() { return mAudioBufferContainer; }
	
	public:

		SoundManager();

		bool create(Match& match, AudioEngine* pEngine, const TCHAR* dynamicLibPath, bool setEngineCoordSys = true);
		
		bool isEnabled();

		void init(unsigned int maxSoundCount, float magScaler);
		void addTrack(const TCHAR* name, const TCHAR* path, float volume);
		void loadAudio();

		void setGain(float gain);
		void setAmbientGain(float gain);

		void startFrame(const Time& currTime);
		void endFrame();

		void setListenerTransform(const Matrix4x3Base& matrix);

		void playSound(AudioBuffer& buffer, const CtVector3& point, const float& mag, const bool& mergable, float pitch = 0.0f);
		
		void playSound(const TCHAR* name, CtVector3& point, const float& mag, const bool& mergable, float pitch = 0.0f);
		void playStockSound(const Sound& sound, const CtVector3& point, const float& mag, const bool& mergable, float pitch = 0.0f);

		void playAmbient();
		void playGoal();

	protected:

		struct ActiveSound {

			HardPtr<AudioSource> source;
			SoftPtr<AudioBuffer> buffer;

			Time startTime;

			Vector3 pos;
			float mag;
			float pitch;
		};

		struct Track {

			String path;
			float volume;
		};

		typedef WETL::StaticArray<ActiveSound, false, int> ActiveSounds;
		typedef WETL::CountedArray<ActiveSound*, false, int> ActiveSoundPtrs;
		typedef WETL::CountedPtrArray<Track*, false, int> Tracks;
	
	protected:

		void merge(ActiveSound& sound, const CtVector3& point, const float& mag);

	protected:

		SoftPtr<Match> mMatch;

		bool mIsInited;
		SoftPtr<AudioEngine> mAudioEngine;
		AudioBufferContainer mAudioBufferContainer;

		float mMagScaler;
		float mMergeDistSq;

		ActiveSounds mActiveSounds;
		ActiveSoundPtrs mUsedSounds;
		ActiveSoundPtrs mFreeSounds;
		ActiveSoundPtrs mFrameSounds;

		Time mFrameTime;
		bool mFrameStarted;
		int mCounter;

		Tracks mTracks;

		SoftRef<AudioBuffer> mSounds[SoundCount];
		HardPtr<AudioTrack> mAmbientTrack;
		float mAmbientTrackVolume;
		float mAmbientGain;

		HardPtr<AudioTrack> mGoalTrack;
	};

} }

#endif