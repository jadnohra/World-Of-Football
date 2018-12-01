#ifndef _WEMesh_h
#define _WEMesh_h

#include "../WERef.h"

#include "WEMeshGeometry.h"
#include "WEMeshRenderGeometry.h"
#include "WEMeshMaterialGroup.h"
#include "../skeleton/WESkeleton.h"
#include "WEMeshPhysicalProxy.h"

namespace WE {

	class MeshInstance;

	class Mesh : public Ref {
	protected:

		HardPtr<MeshGeometry> mGeometry;
		HardPtr<MeshRenderGeometry> mRenderGeometry;
		HardPtr<MeshMaterialGroup> mMaterialGroup;
		HardPtr<Skeleton> mSkeleton;
		HardPtr<MeshPhysicalProxy> mPhysicalProxy;
		
	public:

		Mesh();
		virtual ~Mesh();

		void destroy(bool geometry, bool renderGeometry, bool materialGroup, bool skeleton, bool physicalProxy);

		bool isLoaded();

		bool hasMaterialGroup();
		bool createMaterialGroup();
		MeshMaterialGroup& getMaterialGroup();
		MeshMaterialGroup* getMaterialGroupPtr();

		bool hasGeometry();
		bool createGeometry();
		MeshGeometry& getGeometry();
		

		bool hasRenderGeometry();
		bool createRenderGeometry(RenderLoadContext& renderLoadContext);
		MeshRenderGeometry& getRenderGeometry();
		bool prepareForRendering(RenderLoadContext& renderLoadContext); //create RenderGeomtry if needed

		bool hasSkeleton();
		bool createSkeleton();
		Skeleton& getSkeleton();

		bool hasPhysicalProxy();
		bool createPhysicalProxy();
		MeshPhysicalProxy& getPhysicalProxy();

		bool initInstance(MeshInstance& inst, RenderLoadContext& renderLoadContext);

		void render(Renderer& renderer, MeshInstance& instance, const Matrix4x3Base& worldMatrix, RenderPassEnum pass);
		void render(Renderer& renderer, MeshInstance& instance, SkeletonInstance& skelInst, RenderPassEnum pass);

	protected:
		
	};
}

#endif