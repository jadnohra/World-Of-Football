#include "WEAudioOpenAL.h"

#include "../../WEMem.h"
#include "../../WEMacros.h"
#include "../../WEAssert.h"
#include "../../filePath/WEPathResolver.h"


namespace WE {

AudioEngineOpenAL::AudioEngineOpenAL() {

	mContext = NULL;
	g_hVorbisDLLs.vorbisFile = NULL;

	fn_ov_clear = NULL;
	fn_ov_read = NULL;
	fn_ov_pcm_total = NULL;
	fn_ov_info = NULL;
	fn_ov_comment = NULL;
	fn_ov_open_callbacks = NULL;

	g_bVorbisInit = false;
	mMultiEngineMode = true;
}

AudioEngineOpenAL::~AudioEngineOpenAL() {

	destroy();
}

void AudioEngineOpenAL::destroy() {

	ShutdownVorbisFile();
	ALFWShutdownOpenAL(mContext);
	ALFWShutdown();
}

void AudioEngineOpenAL::getCoordSys(CoordSys& coordSys) {

	coordSys.el0 = CSD_Left;
	coordSys.el1 = CSD_Up;
	coordSys.el2 = CSD_Forward;
	coordSys.rotationLeftHanded = false;
	coordSys.bitValid = 1;
	coordSys.setFloatUnitsPerMeter(0.0f);
}

bool AudioEngineOpenAL::init(const TCHAR* dynamicLibPath) {

	ALFWInit();

	if (!ALFWInitOpenAL(0, mContext))
	{
		destroy();
		return false;
	}

	InitVorbisFile(dynamicLibPath);
	if (!g_bVorbisInit)
	{
		log(LOG_ERROR, L"Failed to find OggVorbis DLLs (vorbisfile.dll, ogg.dll, or vorbis.dll)");
	}

	alDistanceModel(AL_INVERSE_DISTANCE_CLAMPED);

	return true;
}

void AudioEngineOpenAL::setCoordSys(CoordSysConvPool* pConvPool, const CoordSys* pGeomCoordSys) {

	const CoordSys* srcCoordSys;
	CoordSys destSys;
	getCoordSys(destSys);

	if (pGeomCoordSys) {

		srcCoordSys = pGeomCoordSys;

	} else {

		srcCoordSys = &destSys;
	}
		
	if (pConvPool) {

		mGeomConv = pConvPool->getConverterFor(dref(srcCoordSys), destSys, true);

	} else {

		mGeomConv = CoordSysConvPool::createConverterFor(dref(srcCoordSys), destSys, true);
	}
}

AudioEngineOpenAL::OpenALAudioBuffer::~OpenALAudioBuffer() {

	alDeleteBuffers(1, &uiBuffer);
}

AudioEngineOpenAL::OpenALAudioSource::~OpenALAudioSource() {

	alSourceStop(uiSource);
    alDeleteSources(1, &uiSource);
}

void AudioEngineOpenAL::setGain(const float& gain) {

	alListenerf(AL_GAIN, gain);
}


bool AudioEngineOpenAL::createBuffer(const TCHAR* path, AudioBuffer*& pBuffer) {

	if (mMultiEngineMode)
		alcMakeContextCurrent(mContext);

	ALuint      uiBuffer;

	// Generate an AL Buffer
	alGenBuffers( 1, &uiBuffer );

	FlexiblePtr<char> charBuff;
	tcharToChar(path, charBuff);

	// Load Wave file into OpenAL Buffer
	if (!ALFWLoadWaveToBuffer(charBuff.ptr(), uiBuffer)) {
		
		alDeleteBuffers(1, &uiBuffer);
		return false;
	}

	/*
	// Generate a Source to playback the Buffer
    alGenSources( 1, &uiSource );

	// Attach Source to Buffer
	alSourcei( uiSource, AL_BUFFER, uiBuffer );
	*/

	OpenALAudioBuffer* pNativeBuffer;

	MMemNew(pNativeBuffer, OpenALAudioBuffer());

	pNativeBuffer->uiBuffer = uiBuffer;
	//pNativeSource->uiSource = uiSource;

	pBuffer = pNativeBuffer;

	//String::debug(L"createBuffer %u\n", uiBuffer);

	return true;
}


bool AudioEngineOpenAL::createSource(AudioSource*& pSource, float rangeMin, float rangeMax) {

	if (mMultiEngineMode)
		alcMakeContextCurrent(mContext);

	OpenALAudioSource* pNativeSource;

	MMemNew(pNativeSource, OpenALAudioSource());

	alGenSources( 1, &pNativeSource->uiSource );
	//alSourcef(pNativeSource->uiSource, AL_REFERENCE_DISTANCE, 50.0f);
	//alSourcef(pNativeSource->uiSource, AL_MAX_DISTANCE, 50000.0f);
	//alSourcef(pNativeSource->uiSource, AL_ROLLOFF_FACTOR, 100.0f);

	alSourcef(pNativeSource->uiSource, AL_REFERENCE_DISTANCE, rangeMin);
	alSourcef(pNativeSource->uiSource, AL_MAX_DISTANCE, rangeMax);
	alSourcef(pNativeSource->uiSource, AL_ROLLOFF_FACTOR, 1.0f);

	pSource = pNativeSource;

	//String::debug(L"createSource %u\n", pNativeSource->uiSource);

	return true;
}

bool AudioEngineOpenAL::bind(AudioSource* pSource, AudioBuffer* pBuffer) {

	if (mMultiEngineMode)
		alcMakeContextCurrent(mContext);

	OpenALAudioSource& nativeSource = dref((OpenALAudioSource*) pSource);

	if (pBuffer) {

		alSourcei( nativeSource.uiSource, AL_BUFFER, ((OpenALAudioBuffer*) pBuffer)->uiBuffer );

		//String::debug(L"bind %u - %u\n", nativeSource.uiSource, ((OpenALAudioBuffer*) pBuffer)->uiBuffer);

	} else {

		alSourcei( nativeSource.uiSource, AL_BUFFER, NULL );

		//String::debug(L"unbind %u\n", nativeSource.uiSource, ((OpenALAudioBuffer*) pBuffer)->uiBuffer);
	}

	return true;
}

void AudioEngineOpenAL::setSourcePos(AudioSource* pSource, const float* pos) {

	if (mMultiEngineMode)
		alcMakeContextCurrent(mContext);

	OpenALAudioSource& nativeSource = dref((OpenALAudioSource*) pSource);

	mGeomConv->toTargetPoint(pos, mTempVector.el);

	alSourcefv(nativeSource.uiSource, AL_POSITION, mTempVector.el);
}

void AudioEngineOpenAL::setSourceGain(AudioSource* pSource, const float& gain) {

	if (mMultiEngineMode)
		alcMakeContextCurrent(mContext);

	OpenALAudioSource& nativeSource = dref((OpenALAudioSource*) pSource);

	alSourcef(nativeSource.uiSource, AL_GAIN, gain);
	alSourcef(nativeSource.uiSource, AL_MAX_GAIN, gain);
}

void AudioEngineOpenAL::setSourcePitch(AudioSource* pSource, const float& pitch) {

	if (mMultiEngineMode)
		alcMakeContextCurrent(mContext);

	OpenALAudioSource& nativeSource = dref((OpenALAudioSource*) pSource);

	alSourcef(nativeSource.uiSource, AL_PITCH, pitch);
}

void AudioEngineOpenAL::setSourceRolloff(AudioSource* pSource, const float& rolloff) {

	if (mMultiEngineMode)
		alcMakeContextCurrent(mContext);

	OpenALAudioSource& nativeSource = dref((OpenALAudioSource*) pSource);

	alSourcef(nativeSource.uiSource, AL_ROLLOFF_FACTOR, rolloff);
}

/*
void AudioEngineOpenAL::debugLogSourceInfo(AudioSource* pSource) {

	if (mMultiEngineMode)
		alcMakeContextCurrent(mContext);

	OpenALAudioSource& nativeSource = dref((OpenALAudioSource*) pSource);

	alSourcef(pNativeSource->uiSource, AL_REFERENCE_DISTANCE, rangeMin);
	alSourcef(pNativeSource->uiSource, AL_MAX_DISTANCE, rangeMax);
	alSourcef(pNativeSource->uiSource, AL_ROLLOFF_FACTOR, 1.0f);

	alSourcefv(nativeSource.uiSource, AL_POSITION, mTempVector.el);
	alSourcef(nativeSource.uiSource, AL_GAIN, gain);
	alSourcef(nativeSource.uiSource, AL_MIN_GAIN, gain);
	alSourcef(nativeSource.uiSource, AL_MAX_GAIN, gain);
	alSourcef(nativeSource.uiSource, AL_PITCH, pitch);

	alListenerf(AL_GAIN, gain);
	alDistanceModel(AL_INVERSE_DISTANCE_CLAMPED);
}
*/

void AudioEngineOpenAL::setListenerTransform(const Matrix4x3Base& mat) {

	if (mMultiEngineMode)
		alcMakeContextCurrent(mContext);

	Vector3 fwdUp[2];

	mGeomConv->toTargetPoint(mat.row[Scene_Forward].el, fwdUp[0].el);
	mGeomConv->toTargetPoint(mat.row[Scene_Up].el, fwdUp[1].el);
	
	mGeomConv->toTargetPoint(mat.row[3].el, mTempVector.el);

	alListenerfv(AL_POSITION, mTempVector.el);
	alListenerfv(AL_ORIENTATION, (float*) ((void*) (fwdUp)));
}


bool AudioEngineOpenAL::start(AudioSource* pSource) {

	if (mMultiEngineMode)
		alcMakeContextCurrent(mContext);

	OpenALAudioSource& nativeSource = dref((OpenALAudioSource*) pSource);

	alSourceRewind( nativeSource.uiSource );
	alSourcePlay( nativeSource.uiSource );

	//String::debug(L"start %u\n", nativeSource.uiSource );

	return true;
}


void AudioEngineOpenAL::stop(AudioSource* pSource) {

	if (mMultiEngineMode)
		alcMakeContextCurrent(mContext);

	OpenALAudioSource& nativeSource = dref((OpenALAudioSource*) pSource);

	alSourceStop( nativeSource.uiSource );

	//String::debug(L"stop %u\n", nativeSource.uiSource );
}


bool AudioEngineOpenAL::isPlaying(AudioSource* pSource) {

	if (mMultiEngineMode)
		alcMakeContextCurrent(mContext);

	OpenALAudioSource& nativeSource = dref((OpenALAudioSource*) pSource);
	ALint iState;

	alGetSourcei( nativeSource.uiSource, AL_SOURCE_STATE, &iState);

	return iState == AL_PLAYING;
}

bool AudioEngineOpenAL::createTrack(const TCHAR* path, AudioTrack*& pTrack) {

	if (mMultiEngineMode)
		alcMakeContextCurrent(mContext);

	if (g_bVorbisInit == false) { return false; }

	SoftPtr<OpenALAudioTrack> track;
	MMemNew(track.ptrRef(), OpenALAudioTrack(*this, 4));

	if (track->openTrack(path) == false) {

		MMemDelete(track.ptrRef());
		return false;
	}

	 pTrack = track.ptr();
	 return true;
}

void AudioEngineOpenAL::setTrackGain(AudioTrack* pTrack, const float& gain) {

	if (mMultiEngineMode)
		alcMakeContextCurrent(mContext);

	if (g_bVorbisInit == false) { return; }

	OpenALAudioTrack& nativeTrack = dref((OpenALAudioTrack*) pTrack);

	alSourcef(nativeTrack.uiSource, AL_GAIN, gain);
}

void AudioEngineOpenAL::setTrackLooping(AudioTrack* pTrack, const bool& looping) {

	if (mMultiEngineMode)
		alcMakeContextCurrent(mContext);

	if (g_bVorbisInit == false) { return; }

	OpenALAudioTrack& nativeTrack = dref((OpenALAudioTrack*) pTrack);

	nativeTrack.setLooping(looping);
}

bool AudioEngineOpenAL::start(AudioTrack* pTrack, bool preload) {

	if (mMultiEngineMode)
		alcMakeContextCurrent(mContext);

	if (g_bVorbisInit == false) { return true; }

	OpenALAudioTrack& nativeTrack = dref((OpenALAudioTrack*) pTrack);

	nativeTrack.startTrack(preload);

	return true;
}

void AudioEngineOpenAL::stop(AudioTrack* pTrack) {

	if (mMultiEngineMode)
		alcMakeContextCurrent(mContext);

	if (g_bVorbisInit == false) { return; }

	OpenALAudioTrack& nativeTrack = dref((OpenALAudioTrack*) pTrack);

	nativeTrack.stopTrack();
}

bool AudioEngineOpenAL::isPlaying(AudioTrack* pTrack) {

	if (mMultiEngineMode)
		alcMakeContextCurrent(mContext);

	if (g_bVorbisInit == false) { return true; }

	OpenALAudioTrack& nativeTrack = dref((OpenALAudioTrack*) pTrack);

	return nativeTrack.isPlaying();
}

void AudioEngineOpenAL::updateTrackBuffers(AudioTrack* pTrack) {

	if (mMultiEngineMode)
		alcMakeContextCurrent(mContext);

	if (g_bVorbisInit == false) { return; }

	OpenALAudioTrack& nativeTrack = dref((OpenALAudioTrack*) pTrack);

	nativeTrack.bufferTrack();
}

void AudioEngineOpenAL::OpenALAudioTrack::setLooping(const bool& looping) {

	isLooping = looping;
}

bool AudioEngineOpenAL::OpenALAudioTrack::openTrack(const TCHAR* filePath) {

 	closeTrack();

	isLooping = false;

	if (uiBuffers.size == 0) {

		assrt(false);
		return false;
	}

	sCallbacks.read_func = ov_read_func;
	sCallbacks.seek_func = ov_seek_func;
	sCallbacks.close_func = ov_close_func;
	sCallbacks.tell_func = ov_tell_func;

	// Open the OggVorbis file
	pOggVorbisFile = _tfopen(filePath, L"rb");
	if (!pOggVorbisFile) {

		log(LOG_ERROR, L"File Not Found [%s]", filePath);

		return false;
	}

	if (pParent->fn_ov_open_callbacks(pOggVorbisFile, &sOggVorbisFile, NULL, 0, sCallbacks) != 0) {

		return false;
	}


	{
		// Get some information about the file (Channels, Format, and Frequency)
		psVorbisInfo = pParent->fn_ov_info(&sOggVorbisFile, -1);
		if (psVorbisInfo)
		{
			ulFrequency = psVorbisInfo->rate;
			ulChannels = psVorbisInfo->channels;
			if (psVorbisInfo->channels == 1)
			{
				ulFormat = AL_FORMAT_MONO16;
				// Set BufferSize to 250ms (Frequency * 2 (16bit) divided by 4 (quarter of a second))
				ulBufferSize = ulFrequency >> 1;
				// IMPORTANT : The Buffer Size must be an exact multiple of the BlockAlignment ...
				ulBufferSize -= (ulBufferSize % 2);
			}
			else if (psVorbisInfo->channels == 2)
			{
				ulFormat = AL_FORMAT_STEREO16;
				// Set BufferSize to 250ms (Frequency * 4 (16bit stereo) divided by 4 (quarter of a second))
				ulBufferSize = ulFrequency;
				// IMPORTANT : The Buffer Size must be an exact multiple of the BlockAlignment ...
				ulBufferSize -= (ulBufferSize % 4);
			}
			else if (psVorbisInfo->channels == 4)
			{
				ulFormat = alGetEnumValue("AL_FORMAT_QUAD16");
				// Set BufferSize to 250ms (Frequency * 8 (16bit 4-channel) divided by 4 (quarter of a second))
				ulBufferSize = ulFrequency * 2;
				// IMPORTANT : The Buffer Size must be an exact multiple of the BlockAlignment ...
				ulBufferSize -= (ulBufferSize % 8);
			}
			else if (psVorbisInfo->channels == 6)
			{
				ulFormat = alGetEnumValue("AL_FORMAT_51CHN16");
				// Set BufferSize to 250ms (Frequency * 12 (16bit 6-channel) divided by 4 (quarter of a second))
				ulBufferSize = ulFrequency * 3;
				// IMPORTANT : The Buffer Size must be an exact multiple of the BlockAlignment ...
				ulBufferSize -= (ulBufferSize % 12);

			} else {

				
				return false;
			}
		}
	}
	
	if (ulFormat == 0) {

		log(LOG_ERROR, L"Failed to find format information, or unsupported format [$s]", filePath);
		return false;
	}

	{
		// Allocate a buffer to be used to store decoded data for all Buffers
		pDecodeBuffer = (char*)malloc(ulBufferSize);
		if (!pDecodeBuffer)
		{
			log(LOG_ERROR, L"Failed to allocate memory for decoded OggVorbis data");
			return false;
		}

		// Generate some AL Buffers for streaming
		alGenBuffers( uiBuffers.size, uiBuffers.el );

		// Generate a Source to playback the Buffers
		alGenSources( 1, &uiSource );
	}

	iBuffersProcessed = 0;
	iTotalBuffersProcessed = 0;
	iQueuedBuffers = 0;

	return true;
}

void AudioEngineOpenAL::OpenALAudioTrack::preloadTrack(int bufferCount) {

	// Fill all the Buffers with decoded audio data from the OggVorbis file
	for (iLoop = 0; iLoop < bufferCount; iLoop++)
	{
		ulBytesWritten = DecodeOggVorbis(&sOggVorbisFile, pDecodeBuffer, ulBufferSize, ulChannels);
		if (ulBytesWritten)
		{
			alBufferData(uiBuffers[iLoop], ulFormat, pDecodeBuffer, ulBytesWritten, ulFrequency);
			alSourceQueueBuffers(uiSource, 1, &uiBuffers[iLoop]);
		}
	}
}

bool AudioEngineOpenAL::OpenALAudioTrack::isPlaying() {

	return isStarted;
}

void AudioEngineOpenAL::OpenALAudioTrack::bufferTrack() {

	if (isStarted) {

		// Request the number of OpenAL Buffers have been processed (played) on the Source
		iBuffersProcessed = 0;
		alGetSourcei(uiSource, AL_BUFFERS_PROCESSED, &iBuffersProcessed);

		// Keep a running count of number of buffers processed (for logging purposes only)
		iTotalBuffersProcessed += iBuffersProcessed;
		//ALFWprintf("Buffers Processed %d\r", iTotalBuffersProcessed);

		// For each processed buffer, remove it from the Source Queue, read next chunk of audio
		// data from disk, fill buffer with new data, and add it to the Source Queue
		while (iBuffersProcessed)
		{
			// Remove the Buffer from the Queue.  (uiBuffer contains the Buffer ID for the unqueued Buffer)
			uiBuffer = 0;
			alSourceUnqueueBuffers(uiSource, 1, &uiBuffer);

			// Read more audio data (if there is any)
			ulBytesWritten = DecodeOggVorbis(&sOggVorbisFile, pDecodeBuffer, ulBufferSize, ulChannels);
			if (ulBytesWritten)
			{
				alBufferData(uiBuffer, ulFormat, pDecodeBuffer, ulBytesWritten, ulFrequency);
				alSourceQueueBuffers(uiSource, 1, &uiBuffer);
			}


			if (isLooping && ulBytesWritten < ulBufferSize) {

				unsigned long	ulLeft = ulBufferSize - ulBytesWritten;
				unsigned long	ulOffset = ulBytesWritten;

				while (ulLeft) {

					pParent->fn_ov_pcm_seek(&sOggVorbisFile, 0);

					ulBytesWritten = DecodeOggVorbis(&sOggVorbisFile, pDecodeBuffer + ulOffset, ulLeft, ulChannels);
					if (ulBytesWritten)
					{
						alBufferData(uiBuffer, ulFormat, pDecodeBuffer + ulOffset, ulBytesWritten, ulFrequency);
						//alSourceQueueBuffers(uiSource, 1, &uiBuffer);
					}

					ulOffset += ulBytesWritten;
					ulLeft -= ulBytesWritten;
				}

			}

			iBuffersProcessed--;
		}

		// Check the status of the Source.  If it is not playing, then playback was completed,
		// or the Source was starved of audio data, and needs to be restarted.
		alGetSourcei(uiSource, AL_SOURCE_STATE, &iState);
		if (iState != AL_PLAYING)
		{
			// If there are Buffers in the Source Queue then the Source was starved of audio
			// data, so needs to be restarted (because there is more audio data to play)
			alGetSourcei(uiSource, AL_BUFFERS_QUEUED, &iQueuedBuffers);
			if (iQueuedBuffers)
			{
				alSourcePlay(uiSource);
			}
			else
			{
				isStarted = false;
				// Finished playing
				//break;
			}
		}
	}
}

void AudioEngineOpenAL::OpenALAudioTrack::closeTrack() {

	if (pOggVorbisFile) {

		if (psVorbisInfo) {
			
			// Stop the Source and clear the Queue
			if (uiSource) {
				
				alSourceStop(uiSource);
				alSourcei(uiSource, AL_BUFFER, 0);

				uiSource = 0;
			}

			if (pDecodeBuffer)
			{
				free(pDecodeBuffer);
				pDecodeBuffer = NULL;
			}

			// Clean up buffers and sources
			if (uiSource) {
				alDeleteSources( 1, &uiSource );
				alDeleteBuffers( uiBuffers.size, uiBuffers.el );
			}

			pParent->fn_ov_clear(&sOggVorbisFile);
			psVorbisInfo = NULL;
		}

		fclose(pOggVorbisFile);
		pOggVorbisFile = NULL;
	}

}

void AudioEngineOpenAL::OpenALAudioTrack::startTrack(bool preload) {

	alGetError();

	pParent->fn_ov_pcm_seek(&sOggVorbisFile, 0);

	iBuffersProcessed = 0;
	iTotalBuffersProcessed = 0;
	iQueuedBuffers = 0;

	preloadTrack(preload? uiBuffers.size : MMin(uiBuffers.size, 2));
	
	alSourcePlay(uiSource);
	isStarted = true;

	assrt ((pParent->mError = alGetError()) == AL_NO_ERROR);
}

void AudioEngineOpenAL::OpenALAudioTrack::stopTrack() {

	alSourceStop(uiSource);
	//alSourcei(uiSource, AL_BUFFER, 0); ??

	isStarted = false;
}

AudioEngineOpenAL::OpenALAudioTrack::OpenALAudioTrack(AudioEngineOpenAL& parent, int bufferCount) {

	pParent = &parent;
	uiBuffers.resize(bufferCount);

	ulFrequency = 0;
	ulFormat = 0;
	ulChannels = 0;

	uiSource = 0;
	pDecodeBuffer = NULL;
	psVorbisInfo = NULL;
	pOggVorbisFile = NULL;

	isStarted = false;
}

AudioEngineOpenAL::OpenALAudioTrack::~OpenALAudioTrack() {

	closeTrack();
}

AudioEngineOpenAL::VorbisDLLs::VorbisDLLs() 
	: vorbisFile(NULL), vorbis(NULL), ogg(NULL) {
}

AudioEngineOpenAL::VorbisDLLs::~VorbisDLLs() {

	unload();
}

void AudioEngineOpenAL::VorbisDLLs::load(const TCHAR* dynamicLibPath) {

	if (vorbisFile == NULL) {

		const TCHAR* vorbisFileDLLName = L"vorbisfile.dll";
		const TCHAR* vorbisDLLName = L"vorbis.dll";
		const TCHAR* oggDLLName = L"ogg.dll";

		BufferString tempStr;
		
		/*
			We preload the ogg and vorbis dll's so that 
			they are loaded from the correct dynamic path
			and not from system default or other paths
		*/

		{
			if (dynamicLibPath) {

				tempStr.assign(dynamicLibPath);

				PathResolver::appendPath(tempStr, oggDLLName, true, false);

				ogg = LoadLibrary(tempStr.c_tstr());

			} else {

				ogg = LoadLibrary(oggDLLName);
			}
		}

		if (ogg != NULL)
		{
			if (dynamicLibPath) {

				tempStr.assign(dynamicLibPath);

				PathResolver::appendPath(tempStr, vorbisDLLName, true, false);

				vorbis = LoadLibrary(tempStr.c_tstr());

			} else {

				vorbis = LoadLibrary(vorbisDLLName);
			}
		}

		if (vorbis != NULL)
		{
			if (dynamicLibPath) {

				tempStr.assign(dynamicLibPath);

				PathResolver::appendPath(tempStr, vorbisFileDLLName, true, false);

				vorbisFile = LoadLibrary(tempStr.c_tstr());

			} else {

				vorbisFile = LoadLibrary(vorbisFileDLLName);
			}
		}
		
	} else {

		assrt(false);
	}
}

void AudioEngineOpenAL::VorbisDLLs::unload() {

	if (vorbisFile) {

		FreeLibrary(vorbisFile);
		vorbisFile = NULL;
	}

	if (vorbis) {

		FreeLibrary(vorbis);
		vorbis = NULL;
	}

	if (ogg) {

		FreeLibrary(ogg);
		ogg = NULL;
	}
}

void AudioEngineOpenAL::InitVorbisFile(const TCHAR* dynamicLibPath)
{	
	if (g_bVorbisInit)
		return;

	g_hVorbisDLLs.load(dynamicLibPath);

	if (g_hVorbisDLLs.vorbisFile)
	{
		fn_ov_clear = (LPOVCLEAR)GetProcAddress(g_hVorbisDLLs.vorbisFile, "ov_clear");
		fn_ov_read = (LPOVREAD)GetProcAddress(g_hVorbisDLLs.vorbisFile, "ov_read");
		fn_ov_pcm_total = (LPOVPCMTOTAL)GetProcAddress(g_hVorbisDLLs.vorbisFile, "ov_pcm_total");
		fn_ov_info = (LPOVINFO)GetProcAddress(g_hVorbisDLLs.vorbisFile, "ov_info");
		fn_ov_comment = (LPOVCOMMENT)GetProcAddress(g_hVorbisDLLs.vorbisFile, "ov_comment");
		fn_ov_open_callbacks = (LPOVOPENCALLBACKS)GetProcAddress(g_hVorbisDLLs.vorbisFile, "ov_open_callbacks");
		fn_ov_pcm_seek = (LPOVPCMSEEK)GetProcAddress(g_hVorbisDLLs.vorbisFile, "ov_pcm_seek");

		if (fn_ov_clear && fn_ov_read && fn_ov_pcm_total && fn_ov_info &&
			fn_ov_comment && fn_ov_open_callbacks && fn_ov_pcm_seek)
		{
			g_bVorbisInit = true;
		}
	}
}

void AudioEngineOpenAL::ShutdownVorbisFile()
{
	g_hVorbisDLLs.unload();
	g_bVorbisInit = false;
}

void Swap(short &s1, short &s2)
{
	short sTemp = s1;
	s1 = s2;
	s2 = sTemp;
}

unsigned long AudioEngineOpenAL::OpenALAudioTrack::DecodeOggVorbis(OggVorbis_File *psOggVorbisFile, char *pDecodeBuffer, unsigned long ulBufferSize, unsigned long ulChannels)
{
	int current_section;
	long lDecodeSize;
	unsigned long ulSamples;
	short *pSamples;

	unsigned long ulBytesDone = 0;
	while (1)
	{
		lDecodeSize = pParent->fn_ov_read(psOggVorbisFile, pDecodeBuffer + ulBytesDone, ulBufferSize - ulBytesDone, 0, 2, 1, &current_section);
		if (lDecodeSize > 0)
		{
			ulBytesDone += lDecodeSize;

			if (ulBytesDone >= ulBufferSize)
				break;
		}
		else
		{
			break;
		}
	}

	// Mono, Stereo and 4-Channel files decode into the same channel order as WAVEFORMATEXTENSIBLE,
	// however 6-Channels files need to be re-ordered
	if (ulChannels == 6)
	{		
		pSamples = (short*)pDecodeBuffer;
		for (ulSamples = 0; ulSamples < (ulBufferSize>>1); ulSamples+=6)
		{
			// WAVEFORMATEXTENSIBLE Order : FL, FR, FC, LFE, RL, RR
			// OggVorbis Order            : FL, FC, FR,  RL, RR, LFE
			Swap(pSamples[ulSamples+1], pSamples[ulSamples+2]);
			Swap(pSamples[ulSamples+3], pSamples[ulSamples+5]);
			Swap(pSamples[ulSamples+4], pSamples[ulSamples+5]);
		}
	}

	return ulBytesDone;
}


size_t AudioEngineOpenAL::OpenALAudioTrack::ov_read_func(void *ptr, size_t size, size_t nmemb, void *datasource)
{
	return fread(ptr, size, nmemb, (FILE*)datasource);
}

int AudioEngineOpenAL::OpenALAudioTrack::ov_seek_func(void *datasource, ogg_int64_t offset, int whence)
{
	return fseek((FILE*)datasource, (long)offset, whence);
}

int AudioEngineOpenAL::OpenALAudioTrack::ov_close_func(void *datasource)
{
   return fclose((FILE*)datasource);
}

long AudioEngineOpenAL::OpenALAudioTrack::ov_tell_func(void *datasource)
{
	return ftell((FILE*)datasource);
}

}