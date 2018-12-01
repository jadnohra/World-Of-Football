#ifndef _WEMeshMaterial_h
#define _WEMeshMaterial_h

#include "WEMeshDataTypes.h"
//#include "WEMeshRenderState.h"

#include "../WETL/WETLDataTypes.h"
#include "../render/material/WERenderMaterial.h"
#include "../render/material/WERenderMaterialSource.h"


namespace WE {

	
	class MeshMaterial {
	public:

		MeshMaterial();
		~MeshMaterial();

		StringHash getMaterialName();

	public:
		
		StringHash mNameHash;
		RefWrap<RenderMaterial> mRenderMaterial;
	};

}

#endif