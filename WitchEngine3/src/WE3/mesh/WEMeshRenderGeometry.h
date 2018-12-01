#ifndef _WEMeshRenderGeometry_h
#define _WEMeshRenderGeometry_h

#include "../render/WERenderer.h"
#include "../render/loadContext/WERenderLoadContext.h"
#include "../math/WEMatrix.h"

#include "WEMeshGeometry.h"
#include "WEMeshMaterialGroup.h"
#include "WEMeshMatrixGroupArray.h"
#include "WEMeshMaterialInstanceArray.h"



namespace WE {

	class MeshRenderGeometry {
	public:

		MeshRenderGeometry();
		~MeshRenderGeometry();

		bool isCreated();
		bool create(RenderLoadContext& renderLoadContext, MeshGeometry& geometry, MeshMaterialGroup* pMatGroup);
		
		void render(Renderer& renderer, MeshMaterialGroup* pMatGroup, MeshMaterialInstances* pMatInstances, const Matrix4x3Base* pTransforms, RenderPassEnum pass);

		SceneTransformIndex::Type extractMaxMatrixIndex();

	protected:

		struct RenderGeometryPart {

			MeshMaterialIndex mMaterialIndex;

			RenderUINT mStride;
			RenderUINT mRenderPrimCount;
			RenderUINT mVertexCount;
			RenderShader mShader;

			RefWrap<RenderBuffer> mVB;
			RefWrap<RenderBuffer> mIB;

			StaticMeshMatrixGroupArray mRenderMatrixGroups;

			bool create(RenderLoadContext& renderLoadContext, MeshGeometryPart& part, MeshGeometryPartBindings& bind, MeshMaterialGroup* pMatGroup);
			bool createBuffers(Renderer& renderer, MeshGeometryVertexBuffer& renderReadyVB, MeshGeometryIndexBuffer& renderReadyIB);
			bool fillBuffers(Renderer& renderer, MeshGeometryVertexBuffer& sourceVB, MeshGeometryIndexBuffer& sourceIB);
			SceneTransformIndex::Type extractMaxMatrixIndex();
			
		};

		typedef  MeshGeometryPartIndex PartIndex;
		typedef WETL::PtrStaticArray<RenderGeometryPart*, true, PartIndex::Type> PartArray;
		
		PartArray mParts;
		SceneTransformIndex::Type mMatrixCount;
		//StaticMeshMatrixGroupArray mRenderMatrixGroups;
	};
}

#endif