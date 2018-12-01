#include "SoundManager.h"

#include "WE3/binding/openal/WEAudioOpenAL.h"
using namespace WE;

#include "Match.h"

namespace WOF { namespace match {

SoundManager::SoundManager() : mIsInited(false), mFrameStarted(false) {

	mAmbientGain = 1.0f;
}

bool SoundManager::isEnabled() {

	return mIsInited;
}


bool SoundManager::create(Match& match, AudioEngine* pEngine, const TCHAR* dynamicLibPath, bool setEngineCoordSys) {

	if (mIsInited) {

		assrt(false);
		return false;
	}
	
	mMatch = &match;

	mMergeDistSq = match.getCoordSys().convUnit(1.0f);
	mMergeDistSq = mMergeDistSq * mMergeDistSq;

	mAudioEngine = pEngine;

	if (mAudioEngine.isValid()) {

		if (setEngineCoordSys) 
			mAudioEngine->setCoordSys(&match.getLoadContext().convPool(), &match.getBaseCoordSys());
		
		mAudioBufferContainer.setEngine(mAudioEngine.ptr());
	
		mIsInited = true;
	}

	return true;
}

void SoundManager::init(unsigned int maxSoundCount, float magScaler) {

	if (!mIsInited) {

		assrt(false);
		return;
	}

	mMagScaler = magScaler;

	mActiveSounds.setSize(maxSoundCount);
	
	mUsedSounds.reserve(mActiveSounds.size);
	mFreeSounds.reserve(mActiveSounds.size);
	mFrameSounds.reserve(mActiveSounds.size);


	for (ActiveSounds::Index i = 0; i < mActiveSounds.size; ++i) {

		mAudioEngine->createSource(mActiveSounds[i].source.ptrRef(), mMatch->mTweakAudioRefDist, mMatch->mTweakAudioMaxDist);
		mFreeSounds.addOne(&(mActiveSounds[i]));
	}
}

void SoundManager::addTrack(const TCHAR* name, const TCHAR* path, float volume) {

	if (tcharCompare(name, L"ambient") == 0) {

		if (mTracks.count == 0) {

			SoftPtr<Track> track;

			MMemNew(track.ptrRef(), Track());

			track->path.assign(path);
			track->volume = volume;

			mTracks.addOne(track.ptr());
		}

	} else if (tcharCompare(name, L"goal") == 0) {

		if (mTracks.count == 1) {

			SoftPtr<Track> track;

			MMemNew(track.ptrRef(), Track());

			track->path.assign(path);
			track->volume = volume;

			mTracks.addOne(track.ptr());
		}
	}
}

void SoundManager::playAmbient() {

	if (mIsInited == false) return;

	if (mAmbientTrack.isValid()) {

		return;
	}

	if (mTracks.count) {

		if (mAudioEngine->createTrack(mTracks.el[0]->path.c_tstr(), mAmbientTrack.ptrRef())) {

			mAmbientTrackVolume = mTracks.el[0]->volume;

			mAudioEngine->setTrackGain(mAmbientTrack.ptr(), mAmbientGain * mAmbientTrackVolume);
			mAudioEngine->setTrackLooping(mAmbientTrack.ptr(), true);
			mAudioEngine->start(mAmbientTrack.ptr());
		}
	}
}

void SoundManager::playGoal() {

	if (mIsInited == false) return;

	if (mGoalTrack.isValid()) {

		if (!mAudioEngine->isPlaying(mGoalTrack)) {

			mGoalTrack.destroy();

		} else {

			return;
		}
	}

	if (mTracks.count >= 2) {

		if (mAudioEngine->createTrack(mTracks.el[1]->path.c_tstr(), mGoalTrack.ptrRef())) {

			float goalTrackVolume = mTracks.el[1]->volume;

			mAudioEngine->setTrackGain(mGoalTrack.ptr(), mAmbientGain * goalTrackVolume);
			mAudioEngine->setTrackLooping(mGoalTrack.ptr(), false);
			mAudioEngine->start(mGoalTrack.ptr());
		}
	}
}

void SoundManager::setAmbientGain(float gain) {

	mAmbientGain = gain;

	if (mIsInited == false) return;

	if (mAmbientTrack.isValid()) {

		mAudioEngine->setTrackGain(mAmbientTrack.ptr(), mAmbientGain * mAmbientTrackVolume);
	}
}

void SoundManager::setGain(float gain) {

	if (mIsInited == false) return;

	mAudioEngine->setGain(gain);
}

void SoundManager::loadAudio() {

	if (mIsInited == false) return;

	mSounds[Sound_Kick] = mAudioBufferContainer.getBuffer(L"kick");

	if (mSounds[Sound_Kick].isNull()) {

		log(LOG_ERROR, L"Missing sound [kick]");
	}

	mSounds[Sound_Dribble] = mAudioBufferContainer.getBuffer(L"dribble");

	if (mSounds[Sound_Dribble].isNull()) {

		log(LOG_ERROR, L"Missing sound [dribble]");
	}

	mSounds[Sound_Tackle] = mAudioBufferContainer.getBuffer(L"tackle");

	if (mSounds[Sound_Tackle].isNull()) {

		log(LOG_ERROR, L"Missing sound [tackle]");
	}

	mSounds[Sound_Fall] = mAudioBufferContainer.getBuffer(L"fall");

	if (mSounds[Sound_Fall].isNull()) {

		log(LOG_ERROR, L"Missing sound [fall]");
	}

	mSounds[Sound_Jump] = mAudioBufferContainer.getBuffer(L"jump");

	if (mSounds[Sound_Jump].isNull()) {

		log(LOG_ERROR, L"Missing sound [jump]");
	}
}

void SoundManager::startFrame(const Time& currTime) {

	if (mIsInited == false) return;

	if (mActiveSounds.size == 0) {

		return;
	}

	//if (alGetError() != AL_NO_ERROR) assrt(false);

	//String::debug(L"+startFrame %u %f\n", timeGetTime(), currTime);
	
	AudioEngine& eng = mAudioEngine.dref();

	for (ActiveSounds::Index i = 0; i < mUsedSounds.count; ++i) {

		ActiveSound& sound = dref(mUsedSounds[i]);

		if (sound.buffer.isNull() || (eng.isPlaying(sound.source.ptr()) == false)) {

			sound.buffer.destroy();
			mFreeSounds.addOne(&sound);
			mUsedSounds.removeSwapWithLast(i, true);
			--i;
		} 
	}

	//if (alGetError() != AL_NO_ERROR) assrt(false);

	if (mFreeSounds.count != 0) {

		mFrameTime = currTime;

		mFrameStarted = true;
		mCounter = 0;

		mFrameSounds.count = 0;

	} else {

		mFrameStarted = false;
	}

	//String::debug(L"-startFrame %u %f\n", timeGetTime(), currTime);
}

void SoundManager::setListenerTransform(const Matrix4x3Base& matrix) {

	if (mFrameStarted == false) return;

	mAudioEngine->setListenerTransform(matrix);
}


void SoundManager::playSound(const TCHAR* name, CtVector3& point, const float& mag, const bool& mergable, float pitch) {

	SoftRef<AudioBuffer> sound = mAudioBufferContainer.getBuffer(name);

	if (sound.isValid()) {

		//log(L"sound: %s", name);

		//String::debug(L"+playS(%u) %u %s\n", GetCurrentThreadId(), timeGetTime(), name);
	
		playSound(sound, point, mag, mergable, pitch);
	}	
}

void SoundManager::playStockSound(const Sound& sound, const CtVector3& point, const float& mag, const bool& mergable, float pitch) {

	if (mSounds[sound].isValid()) {

		//log(L"stock: %d", sound);

		//String::debug(L"+playSS(%u) %u %u\n", GetCurrentThreadId(), timeGetTime(), (unsigned int) sound);

		playSound(mSounds[sound].dref(), point, mag, mergable, pitch);
	}
}

void SoundManager::merge(ActiveSound& sound, const CtVector3& point, const float& mag) {

	Vector3 temp = sound.pos;

	lerp(temp, point, sound.mag / mag, sound.pos);
	sound.mag += mag;
}

void SoundManager::playSound(AudioBuffer& buffer, const CtVector3& point, const float& mag, const bool& _mergable, float pitch) {

	if (mFrameStarted == false) { /*assrt(false);*/ return; }

	//if (alGetError() != AL_NO_ERROR) assrt(false);

	/*
		for some very strange reason i was not able to find!!
		using the merge function causes OpenAL to hang in some 
		time-critical threads with no obvious way to debug it!
		so it's disbaled for now ...
	*/
	bool mergable = _mergable;

	//String::debug(L"+playSound(%u) %u %f\n", GetCurrentThreadId(), timeGetTime());

	AudioBuffer* pBuffer = &buffer;
	AudioEngine& eng = mAudioEngine.dref();

	if (mergable && mMatch->mTweakAudioEnableSoundMerge) {

		bool merged = false;

		for (ActiveSounds::Index i = 0; i < mFrameSounds.count; ++i) {

			ActiveSound& sound = dref(mFrameSounds[i]);

			if ((sound.buffer.ptr() == pBuffer) 
				&& (distanceSq(sound.pos, point) <= mMergeDistSq)) {

				//merge(sound, point, mag);
				
				merged = true;
				break;
			}
		}

		if (merged) {

			//String::debug(L"-playSound1 %u %f\n", timeGetTime());
			return;
		}

		for (ActiveSounds::Index i = 0; i < mUsedSounds.count; ++i) {

			ActiveSound& sound = dref(mUsedSounds[i]);

			if ((sound.buffer.ptr() == pBuffer)
				&& (mFrameTime - sound.startTime < mMatch->mTweakAudioSoundMergeTime) 
				&& (distanceSq(sound.pos, point) <= mMergeDistSq)) {

				//merge(sound, point, mag);

				merged = true;
				break;
			}
		}

		if (merged) {

			//String::debug(L"-playSound2 %u %f\n", timeGetTime());
			return;
		}
	}

	if (mFreeSounds.count != 0) {

		ActiveSound& sound = dref(mFreeSounds[0]);

		sound.buffer = pBuffer;
		eng.bind(sound.source.ptr(), sound.buffer.ptr());

		sound.startTime = mFrameTime;
		sound.pos = point;
		sound.mag = mag;

		if (sound.buffer->maxShockImpulse > 0.0f) {

			sound.mag /= sound.buffer->maxShockImpulse;
		} 

		sound.mag *= sound.buffer->gainMultiplier * mMagScaler;
		sound.mag = tmin(1.0f, sound.mag);
		
		sound.mag = powf(sound.mag, sound.buffer->responseCurvature);
		
		//log(L"mag %f", sound.mag);

		if (pitch <= 0.0f) {

			if (sound.buffer->defaultMinPitch != sound.buffer->defaultMaxPitch) {

				sound.pitch = trand2<float>(sound.buffer->defaultMinPitch, sound.buffer->defaultMaxPitch);

			} else {

				sound.pitch = sound.buffer->defaultMinPitch;
			}

			//log(L"pitch1 %f", sound.pitch);

		} else {

			//log(L"pitch2 %f", sound.pitch);
			sound.pitch = pitch;
		}

		mFrameSounds.addOne(&sound);
		mFreeSounds.removeSwapWithLast(0, true);
	}

	//if (alGetError() != AL_NO_ERROR) assrt(false);

	++mCounter;

	//String::debug(L"-playSound %u %f\n", timeGetTime());
}

void SoundManager::endFrame() {


	//if (alGetError() != AL_NO_ERROR) assrt(false);

	if (mFrameStarted && mFrameSounds.count) {

		//String::debug(L"+endFrame(%u) %u %f\n", GetCurrentThreadId(), timeGetTime());

		AudioEngine& eng = mAudioEngine.dref();

		for (ActiveSounds::Index i = 0; i < mFrameSounds.count; ++i) {

			ActiveSound& sound = dref(mFrameSounds[i]);

			//the source gain, depending on sound model, might need to be left
			//in game coord sys or converted to another 
			
			eng.setSourcePos(sound.source.ptr(), sound.pos.el);
			eng.setSourceGain(sound.source.ptr(), sound.mag);
			eng.setSourcePitch(sound.source.ptr(), sound.pitch);
			eng.setSourceRolloff(sound.source.ptr(), mMatch->mTweakAudioRolloff);
			eng.start(sound.source.ptr());
		
			mUsedSounds.addOne(mFrameSounds[i]);
		}

		mFrameSounds.count = 0;
	

		mFrameStarted = false;

		//String::debug(L"-endFrame %u %f\n", timeGetTime());
	}

	//if (alGetError() != AL_NO_ERROR) assrt(false);

	if (mIsInited) {

		AudioEngine& eng = mAudioEngine.dref();

		if (mAmbientTrack.isValid()) {

			eng.updateTrackBuffers(mAmbientTrack.ptr());

			if (eng.isPlaying(mAmbientTrack.ptr()) == false) {

				eng.start(mAmbientTrack.ptr());
			}
		}

		if (mGoalTrack.isValid()) {

			eng.updateTrackBuffers(mGoalTrack.ptr());

			if (eng.isPlaying(mGoalTrack.ptr()) == false) {

				mGoalTrack.destroy();
			}
		}

		/*
		for (Tracks::Index i = 0; i < mTracks.count; ++i) {

			eng.updateTrackBuffers(mTracks[i]);

			if (eng.isPlaying(mTracks[i]) == false) {

				mTracks.removeSwapWithLast(i, true);
				--i;
			}
		}
		*/
	}

	//if (alGetError() != AL_NO_ERROR) assrt(false);
}


} }