#ifndef _WEMeshPartGeometryArray_h
#define _WEMeshPartGeometryArray_h

#include "WEMeshGeometryPart.h"
#include "WEMeshDataTypes.h"
#include "../WETL/WETLArray.h"

namespace WE {

	
	typedef WETL::ArrayBase<MeshGeometryPart*, MeshGeometryPartIndex::Type> MeshGeometryPartArrayBase;
	typedef WETL::PtrStaticArray<MeshGeometryPart*, true, MeshGeometryPartIndex::Type> StaticMeshGeometryPartArray;

	typedef MeshGeometryPartArrayBase MeshGeometryParts;
}

#endif