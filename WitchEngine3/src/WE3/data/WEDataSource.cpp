#include "WEDataSource.h"

#include "../WEMem.h"
#include "tchar.h"

namespace WE {

DataChunk::~DataChunk() {
}

DataSource* DataChunk::getSource() {

	return mSource.bumpedPtr();
}

bool DataChunk::isEnabled() {

	DataSourceTranslationContext ctx;
	ctx.start(this, true);

	if (this->hasAttribute(L"_tagEnabled")) {

		BufferString tempStr;
		bool isEnabled = true;
		this->scanAttribute(tempStr, L"_tagEnabled", isEnabled);

		return isEnabled;

	} else if (this->hasAttribute(L"_tagDisabled")) {

		BufferString tempStr;
		bool isDisabled = false;
		this->scanAttribute(tempStr, L"_tagDisabled", isDisabled);

		return !isDisabled;
	}

	return true;
}

DataChunk* DataChunk::getFirstChild() {

	SoftPtr<DataChunk> chunk = _getFirstChild();

	if (chunk.isValid()) {

		if (!chunk->isEnabled()) {

			chunk->Release();
			chunk.destroy();
		}
	}

	return chunk;
}

DataChunk* DataChunk::getNextSibling() {

	SoftPtr<DataChunk> chunk = _getNextSibling();

	if (chunk.isValid()) {

		if (!chunk->isEnabled()) {

			SoftPtr<DataChunk> nextNextSibling = chunk->getNextSibling();

			chunk->Release();
			chunk.destroy();

			return nextNextSibling;
		}
	}

	return chunk;
}

DataChunk* DataChunk::getChild(const TCHAR* name, int len) {

	SoftPtr<DataChunk> chunk = _getChild(name, len);

	if (chunk.isValid()) {

		if (!chunk->isEnabled()) {

			chunk->Release();
			chunk.destroy();
		}
	}

	return chunk;
}

bool DataChunk::resolveFilePath(String& filePath) {

	if (FilePath::isAbsolutePath(filePath.c_tstr()) == false) {

		if (mSource.isNull() || (mSource->getSourcePath() == NULL)) {

			return false;
		}

		FilePath path(mSource->getSourcePath());
		

		String tempPath;
		path.getPath(tempPath);

		PathResolver::appendPath(tempPath, filePath.c_tstr(), true, true);

		filePath.assign(tempPath);
	}

	return true;
}

StringHash DataChunk::generatePathId(bool includeSourcePath, bool includeChunkPath) {

	StringHash ret = 0;
	StringHash hash1 = 0;
	StringHash hash2 = 0;

	if (includeSourcePath) {

		hash1 = getSourcePathId();

		if (includeChunkPath == false) {

			return hash1;
		}

	}

	if (includeChunkPath) {

		RefWrap<DataChunk> walkBack;
		BufferString tempStr;

		walkBack.set(this, true);

		while (walkBack.isValid()) {

			hash2 = merge_StringHash<StringHash>(walkBack->scanValueHash(tempStr), hash2);

			toParent(walkBack);
		}

		if (includeSourcePath == false) {

			return hash2;
		}
	}

	return merge_StringHash<StringHash>(hash1, hash2);
	
}

StringHash DataChunk::getSourcePathId()  {

	return (mSource.isValid() ? mSource->getSourcePathId() : 0);
}

StringHash DataChunk::extractId(bool autoRevertToSourcePathId, const TCHAR* attribute)  {

	StringHash ret;

	BufferString idStr;
	if (getAttribute(attribute, idStr)) {

		ret = idStr.hash();

	} else {

		ret = 0;
	}

	if (ret != 0) {

		return ret;
	}

	if (autoRevertToSourcePathId) {

		ret = getSourcePathId();
	}

	return ret;
}

bool DataChunk::extractId(String& id, const TCHAR* attribute)  {

	return getAttribute(attribute, id);
}


bool DataChunk::sourceEnableTranslation(bool enable) {

	if (mSource.isValid()) {

		return mSource->enableTranslation(enable);
	}

	return false;
}

void DataChunk::translate(const TCHAR* source, BufferString& dest) {

	if (mSource.isValid()) {

		mSource->translate(source, dest);
	}
}

void DataChunk::translate(BufferString& inPlace) {

	if (mSource.isValid()) {

		mSource->translate(inPlace);
	}
}

StringHash DataChunk::scanValueHash(BufferString& tempStr) {

	if (getValue(tempStr) == false) {

		return 0;
	}

	return tempStr.hash();

}

StringHash DataChunk::scanAttributeHash(BufferString& tempStr, const TCHAR* attribute) {

	if (getAttribute(attribute, tempStr) == false) {

		return 0;
	}

	return tempStr.hash();
}

bool DataChunk::scanAttribute(BufferString& tempStr, const TCHAR* attribute, bool& value) {

	if (getAttribute(attribute, tempStr) == false) {

		return false;
	}

	return tempStr.fromString(value);
}

bool DataChunk::scanAttribute(BufferString& tempStr, const TCHAR* attribute, const TCHAR* format, void* pValue) {

	if (getAttribute(attribute, tempStr) == false) {

		return false;
	}
		
	return (_stscanf(tempStr.c_tstr(), format, pValue) == 1);
}

DataChunk* DataChunk::getChildByPath(const TCHAR* pPath) {

	if (pPath == NULL) {

		AddRef();
		return this;
	}

	int reserved = 0;
	StringPartBase part;
	bool found = true;
	String name;


	DataChunk* pBrowse1 = this;
	DataChunk* pBrowse2 = NULL;
	
	pBrowse1->AddRef();

	int index = 0;

	while (PathTokenizer::nextToken(pPath, reserved, part)) {
		
		part.assignTo(pPath, name);
		
		pBrowse2 = pBrowse1->getChild(name.c_tstr(), name.length());

		if (index == 0) {

			if (pBrowse2 == NULL) {

				if (pBrowse1->equals(name.c_tstr())) {

					pBrowse2 = pBrowse1;
					pBrowse2->AddRef();
				}
			}
		}

		MSafeRelease(pBrowse1);

		if (pBrowse2 == NULL) {

			return NULL;
		}

		pBrowse1 = pBrowse2;
		++index;
	}
	
	return pBrowse1;
}

DataChunk* DataChunk::getChild(DataPath* pPath) {

	if ((pPath == NULL) || (pPath->getPartCount() == 0)) {

		AddRef();
		return this;
	}

	DataChunk* pBrowse1 = this;
	DataChunk* pBrowse2 = NULL;
	String* pPart;
	DataPath::PartIndex partCount = pPath->getPartCount();

	pBrowse1->AddRef();

	for (DataPath::PartIndex i = 0; i < partCount; ++i) {

		pPart = pPath->getPart(i);
		pBrowse2 = pBrowse1->getChild(pPart->c_tstr(), pPart->length());

		MSafeRelease(pBrowse1);

		if (pBrowse2 == NULL) {

			return NULL;
		}

		pBrowse1 = pBrowse2;
	}
	
	return pBrowse1;
}

/*
DataChunk* DataChunk::toNextSibling() {

	DataChunk* pRet = getNextSibling();
	Release();

	return pRet;
}
*/


void toParent(DataChunk*& pChunk) {

	DataChunk* pPar = pChunk->getParent();
	pChunk->Release();
	pChunk = pPar;
}

void toParent(RefWrap<DataChunk>& chunkRef) {

	chunkRef.set(chunkRef->getParent(), false);
}

void toNextSibling(DataChunk*& pChunk) {

	DataChunk* pSib = pChunk->getNextSibling();
	pChunk->Release();
	pChunk = pSib;
}

void toNextSibling(RefWrap<DataChunk>& chunkRef) {

	chunkRef.set(chunkRef->getNextSibling(), false);
}

void toFirstChild(DataChunk*& pChunk) {

	DataChunk* pSib = pChunk->getFirstChild();
	pChunk->Release();
	pChunk = pSib;
}

void toFirstChild(RefWrap<DataChunk>& chunkRef) {

	chunkRef.set(chunkRef->getFirstChild(), false);
}

DataChunk* DataChunk::getSibling(const TCHAR* name) {

	DataChunk* pRet = getNextSibling();

	while (pRet) {

		if (pRet->equals(name)) {

			return pRet;
		}

		toNextSibling(pRet);
	}

	return NULL;
}


int DataChunk::countChildren(BufferString& tempStr, const TCHAR* filterValue, const TCHAR* filterAttrName, const TCHAR* filterAttrVal) {

	int count = 0;
	DataChunk* pChunk = getFirstChild();

	bool filteredOut;

	while (pChunk) {

		filteredOut = false;

		if (filterValue && (filteredOut == false)) {

			filteredOut = (pChunk->equals(filterValue) == false);
		} 

		if (filterAttrVal && (filteredOut == false)) {

			if (pChunk->getAttribute(filterAttrName, tempStr)) {

				filteredOut = (tempStr.equals(filterAttrVal) == false);
			} else {

				filteredOut = false;
			}

		} else if (filterAttrName && (filteredOut == false)) {
	
			filteredOut = pChunk->hasAttribute(filterAttrName);
		}

		if (filteredOut == false) {

			++count;
		}

		toNextSibling(pChunk);
	}

	return count;
}


void DataChunk::buildPath(String& path) {

	String temp;
	RefWrap<DataChunk> prev(this, true);

	while(prev.isValid()) {

		prev->getValue(temp);
		path.insert(temp);
		
		prev.set(prev->getParent(), false);

		if (prev.isValid()) {

			path.insert(L"/");
		}
	}

}


DataSource::DataSource() : mTranslatorEnabled(false) {
}

DataSource::~DataSource() {
}

void DataSource::setTranslator(DataTranslator* pTranslator) {

	mTranslator.set(pTranslator, true);	
}

DataTranslator* DataSource::getTranslator() {

	return mTranslator.bumpedPtr();
}

bool DataSource::hasTranslator() {

	return mTranslator.isValid();
}

bool DataSource::enableTranslation(bool enable) {

	bool ret = mTranslatorEnabled;

	mTranslatorEnabled = enable;

	if (mTranslator.isNull()) {

		mTranslatorEnabled = false;
	}

	return ret;
}


void DataSource::translate(const TCHAR* source, BufferString& dest) {

	if (mTranslatorEnabled) {

		mTranslator->translate(source, dest);
		
	} else {

		DataTranslator::dummyTranslate(source, dest);
	}
}

void DataSource::translate(BufferString& inPlace) {

	if (mTranslatorEnabled) {

		mTranslator->translate(inPlace);
		
	} else {

		DataTranslator::dummyTranslate(inPlace);
	}
}


void DataSource::setSourcePath(const TCHAR* pSourcePath) {

	mSourcePath.assign(pSourcePath);
}

const TCHAR* DataSource::getSourcePath() {

	return mSourcePath.c_tstr();
}

StringHash DataSource::getSourcePathId() {

	return mSourcePath.hash();
}

DataChunk* DataSource::getChild(DataPath* pPath) {

	DataChunk* pRoot = getRoot();

	if (pRoot == NULL) {

		return NULL;
	}

	DataChunk* pChunk = pRoot->getChild(pPath);
	
	MSafeRelease(pRoot);

	return pChunk;
}

DataChunk* DataSource::getChild(const TCHAR* name) {

	DataChunk* pRoot = getRoot();

	if (pRoot == NULL) {

		return NULL;
	}

	DataChunk* pChunk = pRoot->getChild(name);
	
	MSafeRelease(pRoot);

	return pChunk;
}

DataChunk* DataSource::getChildByPath(const TCHAR* pPath) {

	DataChunk* pRoot = getRoot();

	if (pRoot == NULL) {

		return NULL;
	}

	DataChunk* pChunk = pRoot->getChildByPath(pPath);
	
	MSafeRelease(pRoot);

	return pChunk;
}

DataChunk* DataSource::getRootFirstChild() {

	DataChunk* pRoot = getRoot();

	if (pRoot == NULL) {

		return NULL;
	}

	DataChunk* pChunk = pRoot->getFirstChild();
	
	MSafeRelease(pRoot);

	return pChunk;
}

void DataSource::resolvePath(const TCHAR* path, String& absPath) const {

	if (FilePath::isAbsolutePath(path)) {

		absPath.assign(path);

	} else {
	
		StringPartBase pathPart;
		StringPartBase filePart;

		const TCHAR* sourcePath = mSourcePath.c_tstr();

		PathTokenizer::extractPathParts(sourcePath, pathPart, filePart, true);
		
		pathPart.assignTo(sourcePath, absPath);
		PathResolver::appendPath(absPath, path, true, true);
	}
}



DataSourceTranslationContext::DataSourceTranslationContext() : mStarted(false) {
}

DataSourceTranslationContext::~DataSourceTranslationContext() {

	end();
}

void DataSourceTranslationContext::start(DataChunk* pChunk, DataTranslator* pTrans, bool enableTrans) {

	SoftRef<DataSource> source;
	source = pChunk ? pChunk->getSource() : NULL;

	start(source.ptr(), pTrans, enableTrans);
}

void DataSourceTranslationContext::start(DataSource* pSource, DataTranslator* pTrans, bool enableTrans) {

	if (mStarted) {

		end();
	}

	mEnableFlagOnly = false;

	mSource.set(pSource, true);
	
	if (mSource.isValid()) {

		mSavedTrans.set(mSource->getTranslator(), false);
		mSource->setTranslator(pTrans);
		mSavedEnabled = mSource->enableTranslation(enableTrans);
	}

	mStarted = true;
}


void DataSourceTranslationContext::start(DataChunk* pChunk, bool enableTrans) {

	SoftRef<DataSource> source;
	source = pChunk ? pChunk->getSource() : NULL;

	start(source.ptr(), enableTrans);
}

void DataSourceTranslationContext::start(DataSource* pSource, bool enableTrans) {

	if (mStarted) {

		end();
	}

	mEnableFlagOnly = true;

	mSource.set(pSource, true);
	
	if (mSource.isValid()) {

		mSavedEnabled = mSource->enableTranslation(enableTrans);
	}

	mStarted = true;
}

void DataSourceTranslationContext::end() {

	if (mStarted) {

		if (mSource.isValid()) {

			if (mEnableFlagOnly == false) {

				mSource->setTranslator(mSavedTrans.ptr());
			}
			mSource->enableTranslation(mSavedEnabled);

			mSource.destroy();
		}

		mSavedTrans.destroy();

		mStarted = false;
	}
}

}