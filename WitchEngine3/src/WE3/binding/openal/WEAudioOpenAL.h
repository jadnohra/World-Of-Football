#ifndef _WEAudioOpenAL_h
#define _WEAudioOpenAL_h

#include "../../audio/WEAudioEngine.h"
#include "../../WETL/WETLSizeAlloc.h"

#include "framework/win32/Framework.h"
#include "Vorbis\vorbisfile.h"

namespace WE {


	
	class AudioEngineOpenAL : public AudioEngine {
	public:

		AudioEngineOpenAL();
		~AudioEngineOpenAL();

		virtual void getCoordSys(CoordSys& coordSys);

		virtual bool init(const TCHAR* dynamicLibPath = NULL);
		virtual void setCoordSys(CoordSysConvPool* pConvPool, const CoordSys* pGeomCoordSys);
		virtual void destroy();
		
		virtual void setGain(const float& gain);

		virtual bool createBuffer(const TCHAR* path, AudioBuffer*& pBuffer);
		virtual bool createSource(AudioSource*& pSource, float rangeMin, float rangeMax);
		virtual bool bind(AudioSource* pSource, AudioBuffer* pBuffer);

		virtual void setSourcePos(AudioSource* pSource, const float* pos);
		virtual void setSourceGain(AudioSource* pSource, const float& gain);
		virtual void setSourcePitch(AudioSource* pSource, const float& pitch);
		virtual void setSourceRolloff(AudioSource* pSource, const float& rolloff);
		virtual void setListenerTransform(const Matrix4x3Base& mat);
		
		virtual bool start(AudioSource* pSource);
		virtual void stop(AudioSource* pSource);
		virtual bool isPlaying(AudioSource* pSource);

		virtual bool createTrack(const TCHAR* path, AudioTrack*& pTrack);
		virtual void setTrackLooping(AudioTrack* pTrack, const bool& looping);

		virtual void setTrackGain(AudioTrack* pTrack, const float& gain);

		virtual bool start(AudioTrack* pTrack, bool preload = false);
		virtual void stop(AudioTrack* pTrack);
		virtual bool isPlaying(AudioTrack* pTrack);

		virtual void updateTrackBuffers(AudioTrack* pTrack);

	protected:

		class OpenALAudioBuffer : public AudioBuffer {
		public:

			ALuint uiBuffer;

			virtual ~OpenALAudioBuffer();
		};

		class OpenALAudioSource : public AudioSource {
		public:

			ALuint uiSource;  

			virtual ~OpenALAudioSource();
		};

	protected:

		SoftRef<CoordSysConv> mGeomConv;
		ALCcontext* mContext;
		bool mMultiEngineMode;

		ALenum mError;

		Vector3 mTempVector;

	protected:

		
		struct VorbisDLLs {

			HINSTANCE vorbisFile;
			HINSTANCE ogg;
			HINSTANCE vorbis;

			VorbisDLLs();
			~VorbisDLLs();

			void load(const TCHAR* dynamicLibPath);
			void unload();
		};

		VorbisDLLs g_hVorbisDLLs;

		// Functions
		void InitVorbisFile(const TCHAR* dynamicLibPath);
		void ShutdownVorbisFile();
		
		// Function pointers
		typedef int (*LPOVCLEAR)(OggVorbis_File *vf);
		typedef long (*LPOVREAD)(OggVorbis_File *vf,char *buffer,int length,int bigendianp,int word,int sgned,int *bitstream);
		typedef ogg_int64_t (*LPOVPCMTOTAL)(OggVorbis_File *vf,int i);
		typedef vorbis_info * (*LPOVINFO)(OggVorbis_File *vf,int link);
		typedef vorbis_comment * (*LPOVCOMMENT)(OggVorbis_File *vf,int link);
		typedef int (*LPOVOPENCALLBACKS)(void *datasource, OggVorbis_File *vf,char *initial, long ibytes, ov_callbacks callbacks);
		typedef int (*LPOVPCMSEEK)(OggVorbis_File *vf,ogg_int64_t pos);

		// Variables
		LPOVCLEAR			fn_ov_clear;
		LPOVREAD			fn_ov_read;
		LPOVPCMTOTAL		fn_ov_pcm_total;
		LPOVINFO			fn_ov_info;
		LPOVCOMMENT			fn_ov_comment;
		LPOVOPENCALLBACKS	fn_ov_open_callbacks;
		LPOVPCMSEEK			fn_ov_pcm_seek;

		bool g_bVorbisInit;
		bool mLooping;

		class OpenALAudioTrack : public AudioTrack {
		public:

			typedef WETL::SizeAllocT<ALuint, int, true> Buffers;

			//ALuint		    uiBuffers[NUMBUFFERS];
			Buffers			uiBuffers;
			ALuint		    uiSource;
			ALuint			uiBuffer;
			ALint			iState;
			ALint			iLoop;
			ALint			iBuffersProcessed, iTotalBuffersProcessed, iQueuedBuffers;
			unsigned long	ulFrequency;
			unsigned long	ulFormat;
			unsigned long	ulChannels;
			unsigned long	ulBufferSize;
			unsigned long	ulBytesWritten;
			char			*pDecodeBuffer;

			bool isStarted;
			bool isLooping;

			ov_callbacks	sCallbacks;
			OggVorbis_File	sOggVorbisFile;
			vorbis_info		*psVorbisInfo;

			FILE *pOggVorbisFile;

			AudioEngineOpenAL* pParent;

			OpenALAudioTrack(AudioEngineOpenAL& parent, int bufferCount);
			~OpenALAudioTrack();

			bool openTrack(const TCHAR* filePath);
			void closeTrack();

			void startTrack(bool preload);
			void stopTrack();

			bool isPlaying();
			void bufferTrack();

			void setLooping(const bool& looping);

			void preloadTrack(int bufferCount);

			static size_t ov_read_func(void *ptr, size_t size, size_t nmemb, void *datasource);
			static int ov_seek_func(void *datasource, ogg_int64_t offset, int whence);
			static int ov_close_func(void *datasource);
			static long ov_tell_func(void *datasource);

			unsigned long DecodeOggVorbis(OggVorbis_File *psOggVorbisFile, char *pDecodeBuffer, unsigned long ulBufferSize, unsigned long ulChannels);
		};

	};

}

#endif