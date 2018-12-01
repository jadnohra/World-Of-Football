#ifndef _WEMeshDataTypes_h
#define _WEMeshDataTypes_h

#include "../WEDataTypes.h"
#include "../WETL/WETLDataTypes.h"
#include "../render/WERenderDataTypes.h"
#include "../scene/WESceneDataTypes.h"

namespace WE {

	typedef IndexT<RenderUINT> MeshFaceIndex;
	typedef WETL::IndexByte	MeshGeometryPartIndex;
	typedef WETL::IndexByte	MeshShaderIndex;
	typedef WETL::IndexByte MeshMaterialIndex;
	typedef SceneTransformIndex MeshMatrixGroupIndex;
	typedef WETL::IndexShort MeshRenderStateIndex;

	typedef MeshRenderStateIndex MeshRSIndex;
}

#endif