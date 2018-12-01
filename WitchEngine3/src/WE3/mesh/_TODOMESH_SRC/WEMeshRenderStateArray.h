#ifndef _WEMeshRenderStateArray_h
#define _WEMeshRenderStateArray_h

#include "../WETL/WETLArray.h"
#include "WEMeshDataTypes.h"
#include "WEMeshRenderState.h"

namespace WE {

	typedef WETL::ArrayBase<const MeshRenderState*, MeshRenderStateIndex::Type> MeshRenderStateArrayBase;
	typedef WETL::StaticArray<const MeshRenderState*, false, MeshRenderStateIndex::Type> StaticMeshRenderStateArray;
	typedef WETL::CountedArray<const MeshRenderState*, false, MeshRenderStateIndex::Type> MeshRenderStateArray;

	typedef MeshRenderStateArrayBase MeshRenderStates;
}

#endif