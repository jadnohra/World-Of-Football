#ifndef _WEMeshMatrixGroupHelper_h
#define _WEMeshMatrixGroupHelper_h

#include "WEMeshMatrixGroupArray.h"
#include "WEMeshMaterialArray.h"
#include "WEMeshGeometryBuffer.h"
#include "WEMeshFaceBuffer.h"

#include "../coordSys/WECoordSysConvPool.h"
#include "../render/WERenderer.h"
#include "../WEPtr.h"


namespace WE {


	class MeshMatrixGroupHelper {
	public:

		static bool prepareMatrixGroupsForRenderer(
							Renderer& renderer, MeshGeometryVertexBuffer& VB, MeshGeometryIndexBuffer& IB, MeshFaceBuffer& FB,
							bool& hasBlending, bool& isIndexedBlending, bool& hasModifiedBuffers, 
							StaticMeshMatrixGroupArray& resultMatrixGroups, 
							MeshGeometryVertexBuffer*& pModifiedDestVB, MeshGeometryIndexBuffer*& pModifiedDestIB, 
							bool allowSoftwareBlending);

		static bool getRenderReadyBuffers(Renderer& renderer, CoordSysConvPool& convPool,
							MeshGeometryVertexBuffer& VB, MeshGeometryIndexBuffer& IB, MeshFaceBuffer& FB,
							FlexiblePtr<MeshGeometryVertexBuffer>& renderReadyVB, FlexiblePtr<MeshGeometryIndexBuffer>& renderReadyIB, 
							StaticMeshMatrixGroupArray& renderMatrixGroups);


		static void groupByMatrixGroup(MeshGeometryVertexBuffer& VB, MeshGeometryIndexBuffer& IB, MeshFaceBuffer& FB, MeshMatrixGroups& matrixGroups, bool updateMatrixGroupPart, bool updateFaceBufferBaseIndex);
		static void createMatrixGroupsByMatrixCountLimit(MeshGeometryVertexBuffer& VB, MeshGeometryIndexBuffer& IB, MeshFaceBuffer& FB, int totalMatrixCount, int maxMatrixCount, StaticMeshMatrixGroupArray& matrixGroups);
	};
}

#endif