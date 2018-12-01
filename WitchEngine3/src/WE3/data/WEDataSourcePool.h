#ifndef _WEDataSourcePool_h
#define _WEDataSourcePool_h

#include "WEDataSourceChunk.h"
#include "../WEFilePath.h"

namespace WE {

	class DataSourcePool {
	public:

		virtual void setTranslator(DataTranslator* pTranslator);

		
		virtual DataSource* getDataSource(const TCHAR* filePath, bool shareable, PathResolver* pResolver) = 0;
		
		//the old way start
		virtual bool getChunk(const TCHAR* chunkPath, bool shareable, DataSourceChunk& chunk, PathResolver* pResolver, bool autoResolveRemoteChunk) = 0;
		bool resolvePossiblyRemoteChunk(DataSourceChunk& chunk, bool shareable, PathResolver* pResolver);
		//the old way end
		

		//the new way start
		virtual DataChunk* getChunk(const TCHAR* chunkPath, bool shareable, PathResolver* pResolver, bool autoResolveRemoteChunk, const TCHAR* remoteAttr = L"remote") = 0;
		DataChunk* getRemoteChild(DataChunk& sourceChunk, bool shareable, const TCHAR* remoteAttr = L"remote");
		DataChunk* resolveRemoteChunk(DataChunk& chunk, bool shareable, PathResolver* pResolver, const TCHAR* remoteAttr = L"remote");
		DataChunk* getRelativeRemoteChunk(DataChunk& chunk, bool shareable, PathResolver* pResolver, const TCHAR* remotePath, const TCHAR* remoteAttr = L"remote");

		bool resolveRemoteChunk(RefWrap<DataChunk>& chunk, bool shareable, PathResolver* pResolver, const TCHAR* remoteAttr = L"remote");
		bool toRemoteChild(RefWrap<DataChunk>& chunk, bool shareable, const TCHAR* remoteAttr = L"remote");
		bool getRelativeRemoteChunk(RefWrap<DataChunk>& chunk, bool shareable, PathResolver* pResolver, const TCHAR* remotePath, const TCHAR* remoteAttr = L"remote");
		//the new way end

	protected:

		void setRemoteScriptTable(BufferString& tempStr, DataChunk* pChunk, const TCHAR* remoteAttr);

	protected:

		RefWrap<DataTranslator> mTranslator;
	};

	}

#endif