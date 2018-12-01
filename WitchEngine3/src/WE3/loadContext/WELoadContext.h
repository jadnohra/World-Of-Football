#ifndef _WELoadContext_h
#define _WELoadContext_h

#include "../WEPtr.h"

namespace WE {

	class RenderLoadContext;
	class MeshSource;
	class DataSourcePool;
	class CoordSysConvPool;
	class ScriptVarTable;

	class LoadContext {
	public:

		LoadContext();
		~LoadContext();

		void destroy();

		DataSourcePool* dataSourcePoolPtr();
		MeshSource* meshSourcePtr();
		RenderLoadContext* renderPtr();

		DataSourcePool& dataSourcePool();
		MeshSource& meshSource();
		RenderLoadContext& render();
		
		void setDataSourcePool(DataSourcePool* pDataSourcePool);
		void setMeshSource(MeshSource* pSource);
		void setRender(RenderLoadContext* pRenderLoadContext);
	
		CoordSysConvPool& convPool();
		virtual ScriptVarTable* varTablePtr() = 0;

	protected:

		SoftPtr<RenderLoadContext> mRenderLoadContext;
		SoftPtr<DataSourcePool> mDataSourcePool;
		SoftPtr<MeshSource> mMeshSource;
	};

}

#endif