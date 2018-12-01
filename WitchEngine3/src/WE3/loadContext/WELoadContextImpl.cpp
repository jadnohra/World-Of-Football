#include "WELoadContextImpl.h"

namespace WE {

LoadContextImpl::LoadContextImpl() {

	mMeshSourceImpl.setPool(&mMeshPool);
	mDataSourcePoolImpl.setTranslator(&mScriptVarTable);

	setDataSourcePool(&mDataSourcePoolImpl);
	setMeshSource(&mMeshSourceImpl);
	setRender(&mRenderLoadContextImpl);
}

LoadContextImpl::~LoadContextImpl() {
}

ScriptVarTable* LoadContextImpl::varTablePtr() {

	return &mScriptVarTable;
}

ScriptVarTable& LoadContextImpl::varTable() {

	return mScriptVarTable;
}

}