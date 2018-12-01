#ifndef _WEMeshMatrixGroupArray_h
#define _WEMeshMatrixGroupArray_h

#include "../WETL/WETLArray.h"
#include "WEMeshMatrixGroup.h"
#include "WEMeshDataTypes.h"

namespace WE {

	typedef WETL::ArrayBase<MeshMatrixGroup, MeshMatrixGroupIndex::Type> MeshMatrixGroupArrayBase;
	typedef WETL::StaticArray<MeshMatrixGroup, false, MeshMatrixGroupIndex::Type> StaticMeshMatrixGroupArray;
	typedef WETL::CountedArray<MeshMatrixGroup, false, MeshMatrixGroupIndex::Type> MeshMatrixGroupArray;

	typedef MeshMatrixGroupArrayBase MeshMatrixGroups;

	class MeshMatrixGroupArrayHelper {
	public:

		static SceneTransformIndex::Type extractMaxMatrixIndex(MeshMatrixGroups& groups);
	};
}

#endif