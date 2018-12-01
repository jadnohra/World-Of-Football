#ifndef _WEDataSourcePoolImpl_h
#define _WEDataSourcePoolImpl_h

#include "WEDataSourcePool.h"
#include "WEDataSourceManager.h"

namespace WE {

	class DataSourcePoolImpl : public DataSourcePool {
	protected:

		DataSourceManager* mpManager;

	public:

		DataSourcePoolImpl(DataSourceManager* pManager = NULL);
		~DataSourcePoolImpl();

		void setManager(DataSourceManager* pManager);
		virtual void setTranslator(DataTranslator* pTranslator);

		virtual DataSource* getDataSource(const TCHAR* filePath, bool shareable, PathResolver* pResolver);

		//the old way
		virtual bool getChunk(const TCHAR* chunkPath, bool shareable, DataSourceChunk& chunk, PathResolver* pResolver, bool autoResolveRemoteChunk);

		//the new way
		virtual DataChunk* getChunk(const TCHAR* chunkPath, bool shareable, PathResolver* pResolver, bool autoResolveRemoteChunk, const TCHAR* remoteAttr = L"remote");
	};

	class DefaultDataSourcePool : public DataSourcePoolImpl {
	public:

		DefaultDataSourcePool();
		~DefaultDataSourcePool();

	protected:

		DataSourceManager* mpManager;
	};


}

#endif