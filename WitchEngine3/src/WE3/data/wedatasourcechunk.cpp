#include "WEDataSourceChunk.h"

#include "WEDataSourceCreator.h"

namespace WE {


DataSourceChunk::DataSourceChunk() {
}

DataSourceChunk::DataSourceChunk(const DataSourceChunk& ref) {

	set(ref);
}

DataSourceChunk::DataSourceChunk(const FilePath& path, const TCHAR* pDataPath) {

	init(path, pDataPath);
}

DataSourceChunk::DataSourceChunk(const DataSourceChunk& sourceChunk, DataChunk* pChunk) {

	mSource.set(sourceChunk.pSource(), true);
	mChunk.set(pChunk, true);
}

DataSourceChunk::DataSourceChunk(const DataSourceChunk& sourceChunk, const RefWrap<DataChunk>& chunkRef) {

	mSource.set(sourceChunk.pSource(), true);
	mChunk.set(chunkRef.ptr(), true);
}

DataSourceChunk::DataSourceChunk(DataSource* pSource, DataChunk* pChunk) {

	mSource.set(pSource, true);
	mChunk.set(pChunk, true);
}

DataSourceChunk::DataSourceChunk(DataChunk& ref) {

	mSource.set(ref.getSource(), false);
	mChunk.set(&ref, true);
}

DataSourceChunk::~DataSourceChunk() {
}

void DataSourceChunk::destroy() {

	mChunk.destroy();
	mSource.destroy();
}

bool DataSourceChunk::init(const FilePath& path, const TCHAR* pDataPath) {

	mSource.set(DataSourceCreator::create(path.c_tstr()), false);

	if (mSource.isNull()) {
	
		//file not found
		return false;
	}


	if (set(pDataPath) == false) {

		mSource.destroy();
		//chunk not found
		return false;
	}

	return true;
}

bool DataSourceChunk::set(const DataSourceChunk& ref) {

	mSource.set(ref.pSource(), true);
	mChunk.set(ref.pChunk(), true);

	return isValid();
}

bool DataSourceChunk::set(DataSource* pSource) {

	mSource.set(pSource, true);

	return mSource.isValid();
}

bool DataSourceChunk::set(const TCHAR* pDataPath) {

	mChunk.set(mSource.dref().getChildByPath(pDataPath), false);

	return mChunk.isValid();
}


bool DataSourceChunk::set(DataSource* pSource, DataChunk* pChunk) {

	mSource.set(pSource, true);
	mChunk.set(pChunk, true);

	return isValid();
}

bool DataSourceChunk::set(const DataSourceChunk* pSourceChunk, DataChunk* pChunk) {

	return set(pSourceChunk ? pSourceChunk->pSource() : NULL, pChunk);
}

bool DataSourceChunk::set(const DataSourceChunk& sourceChunk, DataChunk* pChunk) {

	return set(sourceChunk.pSource(), pChunk);
}

bool DataSourceChunk::set(const DataSourceChunk& sourceChunk, const RefWrap<DataChunk>& chunk) {

	return set(sourceChunk.pSource(), chunk.ptr());
}

/*
bool DataSourceChunk::init(DataSource& source, DataChunk* pChunk) {

	mSource.set(&source, true);
	mChunk.set(pChunk, true);

	return isValid();
}
*/

/*
bool DataSourceChunk::init(DataSource* pSource, const TCHAR* pDataPath) {

	mSource.set(pSource, true);

	if (pSource == NULL) {

		return false;
	}

	mChunk.set(mSource.dref().getChildByPath(pDataPath), false);

	return isValid();
}
*/

bool DataSourceChunk::isValid()const  {

	return mChunk.isValid();
}

bool DataSourceChunk::isNull()const  {

	return mChunk.isNull();
}



bool DataSourceChunk::setChunk(DataChunk* pChunk, bool addRef) {
	
	mChunk.set(pChunk, addRef);

	return mChunk.isValid();
}



bool DataSourceChunk::resloveRemoteChunk(DataSourceChunk& target, const TCHAR* fileAttr, const TCHAR* chunkAttr) {

	if (isNull()) {

		return false;
	}

	if (mChunk->hasChildren()) {

		target.set(mSource.ptr(), mChunk.ptr());

	} else {

		bool hasAttrFile;
		String attrFile;

		RefWrap<DataSource> _src;


		hasAttrFile = mChunk->getAttribute(fileAttr, attrFile);

		if (hasAttrFile == false) {

			hasAttrFile = mChunk->getAttribute(L"source", attrFile);
		}

		if (hasAttrFile) {

			if (mChunk->resolveFilePath(attrFile) == false) {

				assrt(false);
			}

			_src.set(DataSourceCreator::create(attrFile.c_tstr()), false);

			if (_src.isNull()) {

				return false;
			}

		} else {

			_src.set(mSource.ptr(), true);
		}

		

		RefWrap<DataChunk> _chunk;

		String attrChunk;
		mChunk->getAttribute(chunkAttr, attrChunk);
		DataPath chunkPath(attrChunk.c_tstr());

		_chunk.set(_src->getChild(&chunkPath), false);

		target.set(_src.ptr(), _chunk.ptr());
	}


	return target.isValid();
}

}