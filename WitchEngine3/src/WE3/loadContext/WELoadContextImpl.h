#ifndef _WELoadContextImpl_h
#define _WELoadContextImpl_h

#include "WELoadContext.h"
#include "../render/loadContext/WERenderLoadContextImpl.h"
#include "../mesh/WEMeshSourceImpl.h"
#include "../data/WEDataSourcePoolImpl.h"
#include "../script/WEScriptVarTable.h"

namespace WE {

	class LoadContextImpl : public LoadContext {
	public:

		LoadContextImpl();
		~LoadContextImpl();

		virtual ScriptVarTable* varTablePtr();

		//affects all data sources
		ScriptVarTable& varTable();

	protected:

		RenderLoadContextImpl mRenderLoadContextImpl;
		DefaultDataSourcePool mDataSourcePoolImpl;
		MeshSourceImpl mMeshSourceImpl;
		MeshPool mMeshPool;

		ScriptVarTable mScriptVarTable;
	};

}

#endif