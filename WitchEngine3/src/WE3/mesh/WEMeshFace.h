#ifndef _WEMeshFace_h
#define _WEMeshFace_h

#include "WEMeshDataTypes.h"
//#include "WEMeshRenderState.h"

#include "WEMeshShaderArray.h"
//#include "WEMeshRenderStateArray.h"
#include "WEMeshMaterialArray.h"
#include "WEMeshMatrixGroupArray.h"

#include "../render/WERenderDataTypes.h"


namespace WE {

	
	struct MeshFace {

		RenderUINT mBaseIndex; 
		MeshMaterialIndex::Type mMaterialIndex;
		MeshMatrixGroupIndex::Type mMatrixGroupIndex;

	};

	struct MeshRSLink {

		//-->MeshRenderState* pState;

		//or 
		//* type
		//* sourceIndex
		//* indexInSource
	};

	struct MeshFaceContext {

		MeshShaders shaders;
		StaticMeshMaterialArray materials;
		MeshMatrixGroups matrixGroups;
	};

	struct MeshFace2 {

		RenderUINT mFaceBaseIndex; //In index buffer //0 RS

		MeshShaderIndex mShaderIndex; //1RS
		MeshGeometryPartIndex mVBIndex; //1 RS
		MeshGeometryPartIndex mIBIndex; //1 RS
				
		MeshMaterialIndex mMaterialIndex; // x RS
		SceneTransformIndex mMatrixGroupIndex; // x RS : set matrix count, setup matrices, set software / hardware

		//typedef WETL::StaticArray<MeshRSLink, false, MeshRSIndex> MeshRSLinkIndexArray;
		//MeshRSLinkIndexArray mLinkArray;

		//MeshRSIndex::Type extractRenderStateCount(const MeshFaceContext& context);
		//const MeshRenderState* extractRenderState(const MeshFaceContext& context, MeshRSIndex index);
		//void moveRenderState(MeshRSIndex currIndex, MeshRSIndex targetIndex);

		//-->void extractRenderStatesTo(const MeshFaceContext& context, MeshRenderStateArray& states);
	};
	
	
}

#endif