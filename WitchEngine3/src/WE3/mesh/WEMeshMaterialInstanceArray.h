#ifndef _WEMeshMaterialInstanceArray_h
#define _WEMeshMaterialInstanceArray_h


#include "../WETL/WETLArray.h"
#include "WEMeshDataTypes.h"
#include "WEMeshMaterialInstance.h"


namespace WE {

	
	typedef WETL::ArrayBase<MeshMaterialInstance*, MeshMaterialIndex::Type> MeshMaterialInstanceArrayBase;
	typedef WETL::PtrStaticArray<MeshMaterialInstance*, true, MeshMaterialIndex::Type> StaticMeshMaterialInstanceArray;

	typedef MeshMaterialInstanceArrayBase MeshMaterialInstances;

	void cloneMaterialInstanceArray(StaticMeshMaterialInstanceArray& from, StaticMeshMaterialInstanceArray& to);
}

#endif