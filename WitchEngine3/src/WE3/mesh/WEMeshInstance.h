#ifndef _WEMeshInstance_h
#define _WEMeshInstance_h

#include "WEMeshMaterialInstanceArray.h"
#include "../skeleton/WESkeletonInstance.h"
#include "../WETime.h"
#include "WEMesh.h"

namespace WE {


	class MeshInstance {
	public:

		bool init(Mesh& mesh, RenderLoadContext& renderLoadContext);
		
		bool isInited();

		inline void render(Renderer& renderer, const Matrix4x3Base& worldMatrix, RenderPassEnum pass) {

			mMesh->render(renderer, *this, worldMatrix, pass);
		}

		inline void render(Renderer& renderer, SkeletonInstance& skelInst, RenderPassEnum pass) {

			mMesh->render(renderer, *this, skelInst, pass);
		}

		

		Mesh* getMesh(); //ref not bumped

		void cloneTo(MeshInstance& clone, bool cloneMaterial);

	public:

		HardRef<Mesh> mMesh;
		StaticMeshMaterialInstanceArray mMaterialInstances;
	};

}

#endif