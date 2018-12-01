#ifndef _WEMeshMaterialGroup_h
#define _WEMeshMaterialGroup_h

#include "../render/loadContext/WERenderLoadContext.h"

#include "WEMeshMaterialArray.h"
#include "WEMeshMaterialInstance.h"


namespace WE {

	class MeshInstance;

	class MeshMaterialGroup {
	public:


		MeshMaterialGroup();
		~MeshMaterialGroup();

		bool setMaterialCount(MeshMaterialIndex::Type count);
		MeshMaterialIndex::Type getMaterialCount();
		
		bool createMaterial(MeshMaterialIndex::Type index, MeshMaterial** ppMaterial = NULL);
		MeshMaterial& getMaterial(MeshMaterialIndex::Type index);
		
		MeshMaterialIndex::Type findMaterialIndex(const TCHAR* name);
		
		void prepareForRenderer(RenderLoadContext& renderLoadContext, MeshMaterialIndex::Type index);
		bool set(Renderer& renderer, MeshMaterialIndex::Type index, MeshMaterialInstance* pMatInstance); //returns true if unset needed
		void unset(Renderer& renderer, MeshMaterialIndex::Type index, MeshMaterialInstance* pMatInstance);

		void createInstanceMaterials(RenderLoadContext& renderLoadContext, MeshInstance& inst);

	protected:

		StaticMeshMaterialArray mMaterials;
	};
}

#endif