#ifndef _WEDataSourceManager_h
#define _WEDataSourceManager_h

#include "WEDataSource.h"
#include "WEDataSourceChunk.h"
#include "../WEString.h"
#include "../WETL/WETLAddOnlyHashMap.h"

namespace WE {


	class DataSourceManager {
	protected:

		struct File;
		struct Folder;

		enum ObjectType {
			OT_Invalid = -1, OT_Folder, OT_File, OT_Chunk
		};

		struct Object {

			ObjectType type;

			union {

				struct { Folder* pFolder; };
				struct { File* pFile; };
				struct { DataSourceChunk* pChunk; };
				struct { void* pPtr; };
			};

			Object();

			void setNull();
			void set(const Object& ref);
			void set(Folder& folder);
			void set(File& file);
			void set(DataSourceChunk& chunk);

			bool findObject(const StringKey& name, DataSourceManager::Object& object);
		};

		typedef WETL::IndexShort ObjectIndex;

		typedef WETL::AddOnlyPtrHashMap<StringKey, Folder*, ObjectIndex::Type, WETL::ResizeDouble, 2> FolderMap;
		typedef WETL::AddOnlyPtrHashMap<StringKey, File*, ObjectIndex::Type, WETL::ResizeDouble, 2> FileMap;

		struct File {
			
			RefWrap<DataSource> dataSource;

			File(const TCHAR* filePath = NULL, DataTranslator* pTranslator = NULL);
			~File();

			bool getChunk(const TCHAR* chunkPath, DataSourceChunk& chunk);
			bool getChunkObject(const TCHAR* chunkPath, Object& object, DataSourceChunk& chunk);
		};

		struct Folder {

			FolderMap mFolderMap;
			FileMap mFileMap;


			Folder();
			~Folder();

			DataSourceManager::Folder* addFolder(const StringKey& name);
			DataSourceManager::File* addFile(const StringKey& name, const TCHAR* filePath, DataTranslator* pTranslator);

			DataSourceManager::Folder* findFolder(const StringKey& name);
			DataSourceManager::File* findFile(const StringKey& name);
			bool findObject(const StringKey& name, DataSourceManager::Object& object);
		};

		Folder mRoot;
		RefWrap<DataTranslator> mTranslator;

		bool getCurrPath(const TCHAR* filePath, StringPartBase& lastPart, String& currPath);
		bool getRemainingPath(const TCHAR* filePath, StringPartBase& lastPartInclusive, String& remainPathStr);
		bool isValidObject(const TCHAR* filePath, bool& isFolder);

		bool findObject(const TCHAR* path, DataSourceManager::Object& object, DataSourceChunk* pChunk);
		bool putDataSource(const TCHAR* filePath, DataSourceManager::Object& added);
		bool putChunk(const TCHAR* path, DataSourceManager::Object& added, DataSourceChunk& chunk);

	public:

		DataSourceManager();
		~DataSourceManager();

		static bool splitChunkPath(const TCHAR* absoluteChunkPath, StringPartBase& filePart, StringPartBase& chunkPart);

		void setTranslator(DataTranslator* pTranslator);

		DataSource* getDataSource(const TCHAR* absoluteFilePath);
		DataSource* getDataSource(const TCHAR* filePath, PathResolver* pPathResolver); 
		
		//the old way
		bool getChunk(const TCHAR* absoluteChunkPath, DataSourceChunk& chunk);
		bool getChunk(const TCHAR* chunkPath, DataSourceChunk& chunk, PathResolver* pPathResolver); 

		//the new way
		DataChunk* getChunk(const TCHAR* absoluteChunkPath);
		DataChunk* getChunk(const TCHAR* chunkPath, PathResolver* pPathResolver); 
	};

}

#endif