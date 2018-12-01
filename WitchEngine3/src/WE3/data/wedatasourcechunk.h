#ifndef _WEDataSourceChunk_h
#define _WEDataSourceChunk_h

#include "WEDataSource.h"

namespace WE {

	/*

		BIG TODO !!!!! This is waste of time and space since
		DataChunk now automatically has ref to its source!!

		revamp the whole data classes to use this!

		Also, maybe move translator enabling/diabling into 
		DataChunk/DataSourceChunk, removing the need for
		context and making finer control of the flag

		also use the 'new way' developed in 'DataSourcePool'
		inside the whole engine
	*/


	class DataSourceChunk {
	public:

		RefWrap<DataSource> mSource;
		RefWrap<DataChunk> mChunk;


		DataSourceChunk();
		DataSourceChunk(const DataSourceChunk& ref);
		DataSourceChunk(const FilePath& path, const TCHAR* pDataPath);
		DataSourceChunk(const DataSourceChunk& sourceChunk, DataChunk* pChunk);
		DataSourceChunk(const DataSourceChunk& sourceChunk, const RefWrap<DataChunk>& chunkRef);
		DataSourceChunk(DataSource* pSource, DataChunk* pChunk);
		DataSourceChunk(DataChunk& ref);
		
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
	};
}

#endif