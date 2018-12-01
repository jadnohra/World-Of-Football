#include "WEDataSourceManager.h"

#include "WEDataSourceCreator.h"
#include "../binding/win/includeWindows.h"

namespace WE {


DataSourceManager::File::File(const TCHAR* filePath, DataTranslator* pTranslator) {

	if (filePath) {

		dataSource.set(DataSourceCreator::create(filePath, pTranslator), false);
	}
}

DataSourceManager::File::~File() {
}

DataSourceManager::Folder::Folder() {
}

DataSourceManager::Folder::~Folder() {
}

DataSourceManager::Folder* DataSourceManager::Folder::findFolder(const StringKey& name) {

	Folder* pRet = NULL;
	
	mFolderMap.find(name, pRet);

	return pRet;
}

DataSourceManager::File* DataSourceManager::Folder::findFile(const StringKey& name) {

	File* pRet = NULL;
	
	mFileMap.find(name, pRet);

	return pRet;
}

bool DataSourceManager::Folder::findObject(const StringKey& name, DataSourceManager::Object& object) {

	if (object.pFolder = findFolder(name)) {

		object.type = OT_Folder;

	} else if (object.pFile = findFile(name)) {

		object.type = OT_File;

	} else {

		object.type = OT_Invalid;
	}

	return (object.type != OT_Invalid);
}

DataSourceManager::Folder* DataSourceManager::Folder::addFolder(const StringKey& name) {

	Folder* pRet;

	MMemNew(pRet, Folder());

	mFolderMap.insert(name, pRet);

	return pRet;
}


DataSourceManager::File* DataSourceManager::Folder::addFile(const StringKey& name, const TCHAR* filePath, DataTranslator* pTranslator) {

	File* pRet;

	MMemNew(pRet, File(filePath, pTranslator));

	mFileMap.insert(name, pRet);

	return pRet;
}

DataSourceManager::Object::Object() {

	type = OT_Invalid;
}

void DataSourceManager::Object::setNull() {

	type = OT_Invalid;
}

void DataSourceManager::Object::set(const Object& ref) {

	pPtr = ref.pPtr;
	type = ref.type;
}

void DataSourceManager::Object::set(Folder& folder) {

	pFolder = &folder;
	type = OT_Folder;
}

void DataSourceManager::Object::set(File& file) {

	pFile = &file;
	type = OT_File;
}

void DataSourceManager::Object::set(DataSourceChunk& chunk) {

	pChunk = &chunk;
	type = OT_Chunk;
}

bool DataSourceManager::Object::findObject(const StringKey& name, DataSourceManager::Object& object) {

	switch (type) {
		case OT_Folder:
			return pFolder->findObject(name, object);
		break;
	}

	return false;
}

DataSourceManager::DataSourceManager() {
}

DataSourceManager::~DataSourceManager() {
}

void DataSourceManager::setTranslator(DataTranslator* pTranslator) {

	mTranslator.set(pTranslator, true);
}

bool DataSourceManager::getCurrPath(const TCHAR* filePath, StringPartBase& lastPart, String& currPathStr) {

	StringPartBase currPath;
	currPath.startIndex = 0;
	currPath.count = lastPart.startIndex + lastPart.count;

	if (currPath.count == 0) {

		return false;
		
	} 
	currPath.assignTo(filePath, currPathStr);

	return true;
}

bool DataSourceManager::getRemainingPath(const TCHAR* filePath, StringPartBase& lastPartInclusive, String& remainPathStr) {

	
	StringPartBase remPath;
	remPath.startIndex = lastPartInclusive.startIndex;
	remPath.count = String::length(filePath, false) - lastPartInclusive.startIndex;

	if (remPath.count == 0) {

		return false;
		
	} 

	remPath.assignTo(filePath, remainPathStr);

	return true;

}

bool DataSourceManager::isValidObject(const TCHAR* filePath, bool& isFolder) {

	if (Win::isValidFile(filePath, isFolder)) {

		return true;
	}

	log(L"File/Folder not found [%s]", filePath);

	return false;
}

bool DataSourceManager::putDataSource(const TCHAR* path, DataSourceManager::Object& added) {
	
	StringKey name;
	int reserved = 0;
	StringPartBase part;
	bool isFolder;
	String currPath;

	DataSourceManager::Object curr;
	DataSourceManager::Object next;

	curr.set(mRoot);
	
	while (PathTokenizer::nextToken(path, reserved, part)) {
		
		name.assign(path, part);
		
		if (curr.findObject(name, next) == false) {

			if (curr.type == OT_File) {

				assrt(false);
				return false;

			} else {

				if (getCurrPath(path, part, currPath)) {

					if (isValidObject(currPath.c_tstr(), isFolder)) {

						//put new Folder
						if (isFolder) {

							curr.set(*(curr.pFolder->addFolder(name)));

						} else {
						//put new File
							
							curr.set(*(curr.pFolder->addFile(name, currPath.c_tstr(), mTranslator.ptr())));
						}

					} else {

						return false;
					}

				} else {

					return false;
				}
			}

		} else {

			curr.set(next);
		}
	}

	added.set(curr);

	return true;
}


bool DataSourceManager::putChunk(const TCHAR* path, DataSourceManager::Object& added, DataSourceChunk& chunk) {
	
	StringKey name;
	int reserved = 0;
	StringPartBase part;
	bool isFolder;
	String currPath;

	DataSourceManager::Object curr;
	DataSourceManager::Object next;

	curr.set(mRoot);
	
	while (PathTokenizer::nextToken(path, reserved, part)) {
		
		name.assign(path, part);
		
		if (curr.findObject(name, next) == false) {

			if (curr.type == OT_File) {

				if(getRemainingPath(path, part, currPath)) {

					return (curr.pFile->getChunk(currPath.c_tstr(), chunk));

				} else {

					return false;
				}

			} else {

				if (getCurrPath(path, part, currPath)) {

					if (isValidObject(currPath.c_tstr(), isFolder)) {

						//put new Folder
						if (isFolder) {

							curr.set(*(curr.pFolder->addFolder(name)));

						} else {
						//put new File
								
							curr.set(*(curr.pFolder->addFile(name, currPath.c_tstr(), mTranslator.ptr())));
						}

					} else {

						return false;
					}

				} else {
				
					return false;
				}

			} 

		} else {

			curr.set(next);
		}
	}

	added.set(curr);

	if (chunk.isValid()) {

		return true;
	}	

	if (curr.type == OT_File) {

		return (curr.pFile->getChunk(NULL, chunk));
	}

	return false;
}


bool DataSourceManager::File::getChunk(const TCHAR* chunkPath, DataSourceChunk& chunk) {

	if (dataSource.isNull()) {

		return false;
	}
	
	chunk.set(dataSource.ptr());
	
	bool ret = chunk.set(chunkPath);

	return ret;
}

bool DataSourceManager::File::getChunkObject(const TCHAR* chunkPath, DataSourceManager::Object& object, DataSourceChunk& chunk) {

	if (getChunk(chunkPath, chunk)) {

		object.set(chunk);
	} else {

		object.setNull();
	}

	return (object.type != OT_Invalid);
}

bool DataSourceManager::findObject(const TCHAR* path, DataSourceManager::Object& object, DataSourceChunk* pChunk) {

	StringKey name;
	int reserved = 0;
	StringPartBase part;
	bool found = true;

	DataSourceManager::Object& curr = object;
	DataSourceManager::Object next;

	curr.set(mRoot);
	
	while (PathTokenizer::nextToken(path, reserved, part)) {
		
		name.assign(path, part);
		
		if (curr.type == OT_File) {

			String remainPath;

			if(pChunk && getRemainingPath(path, part, remainPath)) {

				return (curr.pFile->getChunkObject(remainPath.c_tstr(), object, *pChunk));

			} else {

				return false;
			}

		} else {

			if (curr.findObject(name, next) == false) {

				found = false;
				break;

			} else {

				curr.set(next);
			}
		}
	}

	if (found && curr.type == OT_File && pChunk && !pChunk->isValid()) {

		return curr.pFile->getChunkObject(L"", object, *pChunk);
	}

	return found && (curr.type != OT_Invalid);
}

DataSource* DataSourceManager::getDataSource(const TCHAR* filePath) {

	DataSourceManager::Object obj;

	bool found = findObject(filePath, obj, NULL);

	if (found == false) {

		putDataSource(filePath, obj);
	}

	if (obj.type == OT_File) {

		return obj.pFile->dataSource.bumpedPtr();
	}

	return NULL;
}

DataSource* DataSourceManager::getDataSource(const TCHAR* filePath, PathResolver* pPathResolver) {

	const TCHAR* absPath;
	String temp;

	if (pPathResolver) {

		pPathResolver->resolvePath(filePath, temp);
		absPath = temp.c_tstr();

	} else {

		absPath = filePath;
	}
	
	return getDataSource(absPath);
}

bool DataSourceManager::getChunk(const TCHAR* chunkPath, DataSourceChunk& chunk) {

	DataSourceManager::Object obj;

	bool found = findObject(chunkPath, obj, &chunk);

	
	if (found == false) {

		if (putChunk(chunkPath, obj, chunk) == false) {

			chunk.destroy();
		}
	}

	return chunk.isValid();
}

bool DataSourceManager::getChunk(const TCHAR* chunkPath, DataSourceChunk& chunk, PathResolver* pPathResolver) {

	const TCHAR* absPath;
	String temp;

	if (pPathResolver) {

		pPathResolver->resolvePath(chunkPath, temp);
		absPath = temp.c_tstr();

	} else {

		absPath = chunkPath;
	}

	assrt(FilePath::isAbsolutePath(absPath));

	return getChunk(absPath, chunk);
}

DataChunk* DataSourceManager::getChunk(const TCHAR* chunkPath) {

	DataSourceManager::Object obj;
	DataSourceChunk tempChunk;

	bool found = findObject(chunkPath, obj, &tempChunk);

	
	if (found == false) {

		if (putChunk(chunkPath, obj, tempChunk) == false) {

			tempChunk.destroy();
			return NULL;
		}

		int temp = 0;
	}


	//AddRef is only becuase of using tempChunk
	//which at destruction would also release the 
	//pChunk
	tempChunk.pChunk()->AddRef();
	return tempChunk.pChunk();
}


DataChunk* DataSourceManager::getChunk(const TCHAR* chunkPath, PathResolver* pPathResolver) {

	const TCHAR* absPath;
	String temp;

	if (pPathResolver) {

		pPathResolver->resolvePath(chunkPath, temp);
		absPath = temp.c_tstr();

	} else {

		absPath = chunkPath;
	}

	assrt(FilePath::isAbsolutePath(absPath));

	return getChunk(absPath);
}


bool DataSourceManager::splitChunkPath(const TCHAR* absoluteChunkPath, StringPartBase& filePart, StringPartBase& chunkPart) {


	String currPath;
	int reserved = 0;
	StringPartBase part;
	bool isFolder;
	
	filePart.startIndex = 0;

	//could optimize by trying to search searching for file first (detect '.')

	while (PathTokenizer::nextToken(absoluteChunkPath, reserved, part)) {
				
		filePart.count = part.lastIndex() + 1;

		filePart.assignTo(absoluteChunkPath, currPath);

		if (Win::isValidFile(currPath.c_tstr(), isFolder)) {

			if (isFolder == false) {
				
				//we got file path in filePart
				filePart.getRemainingPart(absoluteChunkPath, chunkPart);
				++(chunkPart.startIndex);
				
				return true;
			}

		} else {

			return false;
		}

	}

	return true;
}

}