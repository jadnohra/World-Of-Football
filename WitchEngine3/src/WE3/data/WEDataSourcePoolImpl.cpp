#include "WEDataSourcePoolImpl.h"

#include "WEDataSourceCreator.h"

namespace WE {

DataSourcePoolImpl::DataSourcePoolImpl(DataSourceManager* pManager) : mpManager(NULL) {

	setManager(pManager);
}

DataSourcePoolImpl::~DataSourcePoolImpl() {

	mpManager = NULL;
}

void DataSourcePoolImpl::setManager(DataSourceManager* pManager) {

	mpManager = pManager;

	if (mpManager) {

		mpManager->setTranslator(mTranslator.ptr());
	}
}

void DataSourcePoolImpl::setTranslator(DataTranslator* pTranslator) {

	DataSourcePool::setTranslator(pTranslator);

	if (mpManager) {

		mpManager->setTranslator(mTranslator.ptr());
	}
}

DataSource* DataSourcePoolImpl::getDataSource(const TCHAR* filePath, bool shareable, PathResolver* pResolver) {

	if (mpManager && shareable) {

		return mpManager->getDataSource(filePath, pResolver);

	}

	return DataSourceCreator::create(filePath, mTranslator.ptr());
}

bool DataSourcePoolImpl::getChunk(const TCHAR* chunkPath, bool shareable, DataSourceChunk& chunk, PathResolver* pResolver, bool autoResolveRemoteChunk) {

	if (mpManager && shareable) {

		if (mpManager->getChunk(chunkPath, chunk, pResolver) == false) {

			return false;
		}

	} else {

		String absChunkPathStr;
		
		resolvePath(pResolver, chunkPath, absChunkPathStr);
		
		const TCHAR* absChunkPath = absChunkPathStr.c_tstr();

		StringPartBase filePart;
		StringPartBase chunkPart;

		if (DataSourceManager::splitChunkPath(absChunkPath, filePart, chunkPart) == false) {

			return false;
		}

		String temp;
		filePart.assignTo(absChunkPath, temp);

		RefWrap<DataSource> source(getDataSource(temp.c_tstr(), shareable, NULL), false);

		if (chunk.set(source.ptr()) == false) {

			return false;
		}

		chunkPart.assignTo(absChunkPath, temp);
		chunk.set(temp.c_tstr());
	}

	if (autoResolveRemoteChunk) {
		
		resolvePossiblyRemoteChunk(chunk, shareable, pResolver);
	}

	return chunk.isValid();
}

DataChunk* DataSourcePoolImpl::getChunk(const TCHAR* chunkPath, bool shareable, PathResolver* pResolver, bool autoResolveRemoteChunk, const TCHAR* remoteAttr) {

	HardRef<DataChunk> ret;

	if (mpManager && shareable) {

		ret = mpManager->getChunk(chunkPath, pResolver);

	} else {

		String absChunkPathStr;
		
		resolvePath(pResolver, chunkPath, absChunkPathStr);
		
		const TCHAR* absChunkPath = absChunkPathStr.c_tstr();

		StringPartBase filePart;
		StringPartBase chunkPart;

		if (DataSourceManager::splitChunkPath(absChunkPath, filePart, chunkPart) == false) {

			return false;
		}

		String temp;
		filePart.assignTo(absChunkPath, temp);

		SoftRef<DataSource> source = getDataSource(temp.c_tstr(), shareable, NULL);
		chunkPart.assignTo(absChunkPath, temp);

		ret = source->getChildByPath(temp.c_tstr());
	}

	if (autoResolveRemoteChunk && ret.isValid()) {

		//for ref correctness
		SoftRef<DataChunk> temp = ret;
		ret = resolveRemoteChunk(ret.dref(), shareable, pResolver, remoteAttr);
	}

	return ret.ptr();
}



DefaultDataSourcePool::DefaultDataSourcePool() {

	MMemNew(mpManager, DataSourceManager());
	setManager(mpManager);
}

DefaultDataSourcePool::~DefaultDataSourcePool() {

	MMemDelete(mpManager);
}

}