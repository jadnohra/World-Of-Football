#include "WEAudioEngine.h"


namespace WE {


AudioBufferContainer::AudioBufferContainer() {
}

AudioBufferContainer::~AudioBufferContainer() {
}

void AudioBufferContainer::setEngine(AudioEngine* pEngine) {

	mEngine = pEngine;
}

AudioBufferContainer::Key AudioBufferContainer::bufferKey(const TCHAR*  name) {

	return String::hash(name);
}

bool AudioBufferContainer::loadBuffer(const TCHAR* path, const TCHAR* name) {

	SoftRef<AudioBuffer> buffer;

	if (mEngine.isNull() || mEngine->createBuffer(path, buffer.ptrRef()) == false) {

		return false;
	}

	return putBuffer(name, buffer.ptr());
}

bool AudioBufferContainer::putBuffer(const TCHAR*  name, AudioBuffer* pBuffer) {

	return mBufferMap.insert(bufferKey(name), pBuffer);
}

AudioBuffer* AudioBufferContainer::getBuffer(const TCHAR*  name) {
	
	AudioBuffer* pRet = NULL;

	mBufferMap.find(bufferKey(name), pRet);
	return pRet;
}

bool AudioBufferContainer::removeBuffer(const TCHAR*  name) {

	return mBufferMap.remove(bufferKey(name));
}

void AudioBufferContainer::releaseUnused() {

	mBufferMap.removeOneRefObjects();
}

AudioBufferContainer::Index AudioBufferContainer::getObjectCount() {

	return mBufferMap.getCount();
}

}