#ifndef _WEDataSourceChunk_h
#define _WEDataSourceChunk_h

#include "WEDataSource.h"

namespace WE {

	class DataSourceChunk {
	public:

		RefWrap<DataSource> mSource;
		RefWrap<DataChunk> mChunk;


		DataSourceChunk();
		DataSourceChunk(const DataSourceChunk& ref);
		DataSourceChunk(const FilePath& path, const TCHAR* pDataPath);
		DataSourceChunk(const DataSourceChunk& sourceChunk, DataChunk* pChunk);
		DataSourceChunk(const DataSourceChunk& sourceChunk, const RefWrap<DataChunk>& chunkRef);
		
		
		~DataSourceChunk();
		void destroy();

		bool init(const FilePath& path, const TCHAR* pDataPath);
		bool set(DataSource* pSource);
		bool set(const TCHAR* pDataPath);
		bool set(DataSource* pSource, DataChunk* pChunk);
		bool set(const DataSourceChunk* pSourceChunk, DataChunk* pChunk);
		bool set(const DataSourceChunk& sourceChunk, DataChunk* pChunk);
		bool set(const DataSourceChunk& sourceChunk, const RefWrap<DataChunk>& chunkRef);
		bool set(const DataSourceChunk& ref);
		

		StringHash getSourcePathId() ;
		StringHash extractId(const TCHAR* attribute = L"id");
		bool extractId(String& id, const TCHAR* attribute = L"id") ;

		bool isValid() const ;
		bool isNull() const ;

		inline DataSource* pSource() const { return mSource.ptr();};
		inline DataSource& source() const { return mSource.dref();};

		inline DataChunk* pChunk() const { return mChunk.ptr();};
		inline DataChunk& chunk() const { return mChunk.dref();};

		bool toNextSibling();
		bool setChunk(DataChunk* pChunk, bool addRef); //must be from same source

		inline DataChunk* operator->() {

			return mChunk.ptr();
		}


		bool resloveRemoteChunk(DataSourceChunk& target, const TCHAR* fileAttr = L"file", const TCHAR* chunkAttr = L"chunk");
		bool resolveFilePath(String& filePath);

	};
}

#endif