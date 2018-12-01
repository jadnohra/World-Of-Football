#ifndef _WEMeshMaterialArray_h
#define _WEMeshMaterialArray_h

#include "../WETL/WETLArray.h"
#include "WEMeshDataTypes.h"
#include "WEMeshMaterial.h"

namespace WE {

	typedef WETL::ArrayBase<MeshMaterial*, MeshMaterialIndex::Type> MeshMaterialArrayBase;
	typedef WETL::PtrStaticArray<MeshMaterial*, true, MeshMaterialIndex::Type> StaticMeshMaterialArray;
	typedef WETL::CountedPtrArray<MeshMaterial*, false, MeshMaterialIndex::Type> MeshMaterialArray;

	typedef MeshMaterialArrayBase MeshMaterials;
}

#endif