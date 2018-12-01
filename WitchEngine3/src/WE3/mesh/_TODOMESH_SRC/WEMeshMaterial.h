#ifndef _WEMeshMaterial_h
#define _WEMeshMaterial_h

#include "WEMeshRenderState.h"
#include "WEMeshDataTypes.h"

#include "../WETL/WETLDataTypes.h"
#include "../renderer/WERenderObjects.h"


namespace WE {

	
	class MeshMaterial {
	public:

		RenderBufferPart mPart;

	public:

		MeshMaterial();
		~MeshMaterial();

		MeshRSIndex::Type getStateCount();
		const MeshRenderState* getState(MeshRSIndex::Type index);

		//use MeshRSIndexNull to move to end
		void moveStateToIndex(MeshRSIndex::Type index, MeshRSIndex::Type targetIndex);

	};

}

#endif