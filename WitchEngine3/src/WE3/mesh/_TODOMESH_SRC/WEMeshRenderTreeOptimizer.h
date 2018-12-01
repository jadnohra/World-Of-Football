#ifndef _WEMeshRenderTreeOptimizer_h
#define _WEMeshRenderTreeOptimizer_h

#include "WEMeshMatrixGroupArray.h"
#include "WEMeshMaterialArray.h"
#include "WEMeshGeometryBuffer.h"
#include "WEMeshFaceArray.h"

#include "../renderer/WERenderer.h"
#include "../WEPtr.h"

namespace WE {

	class MeshRenderTreeOptimizer {
	public:


		static bool createRenderTree(Renderer& renderer, 
							MeshGeometryBuffer& VB, MeshGeometryBuffer& IB, MeshFaces& FB,
							MeshMatrixGroups& matrixGroups, MeshMaterials& materials);

	};

}

#endif