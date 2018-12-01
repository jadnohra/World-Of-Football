#ifndef _WEMeshMatrixGroupHelper_h
#define _WEMeshMatrixGroupHelper_h

#include "WEMeshMatrixGroupArray.h"
#include "WEMeshMaterialArray.h"
#include "WEMeshGeometryBuffer.h"
#include "WEMeshFaceArray.h"

#include "../renderer/WERenderer.h"
#include "../WEPtr.h"

namespace WE {


	class MeshMatrixGroupHelper {
	public:

		static bool prepareMatrixGroupsForRenderer(
							Renderer& renderer, MeshGeometryVertexBuffer& VB, MeshGeometryIndexBuffer& IB, MeshFaces& FB,
							bool& hasBlending, bool& isIndexedBlending, bool& hasModifiedBuffers, 
							StaticMeshMatrixGroupArray& resultMatrixGroups, 
							MeshGeometryVertexBuffer*& pModifiedDestVB, MeshGeometryIndexBuffer*& pModifiedDestIB, 
							bool allowSoftwareBlending);

		static bool getRenderReadyBuffers(Renderer& renderer, CoordSysConvPool& convFactory, const CoordSysSemantics& srcSemantics,
							MeshGeometryVertexBuffer& VB, MeshGeometryIndexBuffer& IB, MeshFaces& FB,
							Ptr<MeshGeometryVertexBuffer>& renderReadyVB, Ptr<MeshGeometryIndexBuffer>& renderReadyIB, 
							StaticMeshMatrixGroupArray& renderMatrixGroups);
	};
}

#endif