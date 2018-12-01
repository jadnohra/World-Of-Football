#ifndef _WEMeshFaceArray_h
#define _WEMeshFaceArray_h

#include "WEMeshFace.h"
#include "WEMeshDataTypes.h"

namespace WE {


	typedef WETL::StaticArray<MeshFace, true, MeshFaceIndex::Type> StaticMeshFaceArray;
	typedef WETL::ArrayBase<MeshFace, MeshFaceIndex::Type> MeshFaceArrayBase;
	typedef MeshFaceArrayBase MeshFaces;

	typedef WETL::StaticArray<MeshFace2, true, MeshFaceIndex::Type> StaticMeshFace2Array;

}

#endif
