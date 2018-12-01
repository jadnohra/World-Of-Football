#ifndef _WEMeshGeometryPartBindings_h
#define _WEMeshGeometryPartBindings_h

#include "WEMeshDataTypes.h"
#include "../WETL/WETLArray.h"

namespace WE {

	//use this for now, later use per face info ? ...
	struct MeshGeometryPartBindings {

		MeshMaterialIndex mMaterialIndex;
		//MeshMatrixGroupIndex mMatrixGroupIndex;

		MeshGeometryPartBindings();
	};
	
	typedef WETL::StaticArray<MeshGeometryPartBindings, false, MeshGeometryPartIndex::Type> StaticMeshGeometryPartBindingsArray;
}

#endif