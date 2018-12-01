#ifndef _WEAudioEngine_h
#define _WEAudioEngine_h

#include "../WEDataTypes.h"
#include "../coordSys/WECoordSys.h"
#include "../coordSys/WECoordSysConvPool.h"
#include "../WEPtr.h"
#include "../string/WETCHAR.h"
#include "../math/WEMatrix.h"
#include "../scene/WESceneTransform.h"
#include "../WETL/WETLHashMap.h"


namespace WE {

	class AudioBuffer : public Ref {
	public:

		AudioBuffer() : defaultMinPitch(1.0f), defaultMaxPitch(1.0f), maxShockImpulse(0.0f), gainMultiplier(1.0f), responseCurvature(1.0f) {}
		virtual ~AudioBuffer() {};

	public:

		float defaultMinPitch;
		float defaultMaxPitch;
		float maxShockImpulse;
		float gainMultiplier;
		float responseCurvature;
	};

	class AudioSource {
	public:

		virtual ~AudioSource() {};
	};

	class AudioTrack {
	public:

		virtual ~AudioTrack() {};
	};

	class AudioEngine {
	public:

		virtual ~AudioEngine() {};

		virtual void getCoordSys(CoordSys& coordSys) = 0;

		virtual bool init(const TCHAR* dynamicLibPath = NULL) = 0;
		virtual void setCoordSys(CoordSysConvPool* pConvPool, const CoordSys* pGeomCoordSys) = 0;
		virtual void destroy() = 0;
		
		virtual void setGain(const float& gain) = 0;

		virtual bool createBuffer(const TCHAR* path, AudioBuffer*& pBuffer) = 0;
		virtual bool createSource(AudioSource*& pSource, float rangeMin, float rangeMax) = 0;
		virtual bool bind(AudioSource* pSource, AudioBuffer* pBuffer) = 0;

		virtual void setSourcePos(AudioSource* pSource, const float* pos) = 0;
		virtual void setSourceGain(AudioSource* pSource, const float& gain) = 0;
		virtual void setSourcePitch(AudioSource* pSource, const float& pitch) = 0;
		virtual void setSourceRolloff(AudioSource* pSource, const float& rolloff) = 0;
		virtual void setListenerTransform(const Matrix4x3Base& mat) = 0;
		//virtual void setListenerOrient(const float* orient) = 0;
		
		virtual bool start(AudioSource* pSource) = 0;
		virtual void stop(AudioSource* pSource) = 0;
		virtual bool isPlaying(AudioSource* pSource) = 0;


		virtual bool createTrack(const TCHAR* path, AudioTrack*& pTrack) = 0;

		virtual void setTrackGain(AudioTrack* pTrack, const float& gain) = 0;
		virtual void setTrackLooping(AudioTrack* pTrack, const bool& looping) = 0;

		virtual bool start(AudioTrack* pTrack, bool preload = false) = 0;
		virtual void stop(AudioTrack* pTrack) = 0;
		virtual bool isPlaying(AudioTrack* pTrack) = 0;

		virtual void updateTrackBuffers(AudioTrack* pTrack) = 0;
	};


	class AudioBufferContainer {
	public:

		typedef unsigned int Index;

	public:

		AudioBufferContainer();
		~AudioBufferContainer();

		void setEngine(AudioEngine* pEngine);

		bool loadBuffer(const TCHAR* path, const TCHAR* name);
		bool putBuffer(const TCHAR* name, AudioBuffer* pBuffer);

		AudioBuffer* getBuffer(const TCHAR* name); //ref bumped

		bool removeBuffer(const TCHAR* name);

		void releaseUnused();

		Index getObjectCount();

	protected:

		typedef StringHash Key;
		typedef WETL::RefHashMap<Key, AudioBuffer*, Index, WETL::ResizeDouble, 16> BufferMap;

		SoftPtr<AudioEngine> mEngine;
		BufferMap mBufferMap;

		inline Key bufferKey(const TCHAR* name);
	};

	
}

#endif