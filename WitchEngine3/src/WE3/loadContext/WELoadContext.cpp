#include "WELoadContext.h"

#include "../render/loadContext/WERenderLoadContext.h"
#include "../data/WEDataSourcePool.h"
#include "../mesh/WEMeshSource.h"


namespace WE {

LoadContext::LoadContext() {
}

LoadContext::~LoadContext() {
}

void LoadContext::destroy() {


	mMeshSource.destroy();
	mDataSourcePool.destroy();
	mRenderLoadContext.destroy();
}


DataSourcePool* LoadContext::dataSourcePoolPtr() {

	return mDataSourcePool.ptr();
}

MeshSource* LoadContext::meshSourcePtr() {

	return mMeshSource.ptr();
}

RenderLoadContext* LoadContext::renderPtr() {

	return mRenderLoadContext.ptr();
}

DataSourcePool& LoadContext::dataSourcePool() {

	return mDataSourcePool.dref();
}

MeshSource& LoadContext::meshSource() {

	return mMeshSource.dref();
}

RenderLoadContext& LoadContext::render() {

	return mRenderLoadContext.dref();
}
		
void LoadContext::setDataSourcePool(DataSourcePool* pDataSourcePool) {

	mDataSourcePool = pDataSourcePool;
}

void LoadContext::setMeshSource(MeshSource* pSource) {

	mMeshSource = pSource;
}

void LoadContext::setRender(RenderLoadContext* pRenderLoadContext) {

	mRenderLoadContext = pRenderLoadContext;
}

CoordSysConvPool& LoadContext::convPool() {

	return mRenderLoadContext->convPool();
}

}