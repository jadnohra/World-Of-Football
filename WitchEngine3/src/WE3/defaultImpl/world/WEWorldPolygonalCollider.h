#ifndef _WEWorldPolygonalCollider_h
#define _WEWorldPolygonalCollider_h

#include "../../spatialManager/WESpatialManager.h"
#include "WEWorldMaterialManager.h"
#include "../../mesh/WEMesh.h"
#include "../../WETL/WETLArray.h"

namespace WE {


	class WorldPolygonalCollider {
	public:

		WorldPolygonalCollider();

		bool createFromMesh(Object* pOwner, Mesh& mesh, WorldMaterialManager& matManager, const Matrix4x3Base* pTransform, bool simplifiyIfEnabled, bool* pResultIsEmpty = NULL);

		void transform(const Matrix4x3Base& trans);
		void render(Renderer& renderer, const RenderColor* pColor = NULL);

		bool add(SpatialManagerBuildPhase& buildPhase);
		void remove(SpatialManager& manager);

		void destroy(SpatialManager* pManager = NULL);

		bool isAddedToManager();

		inline const Object& getObject() { return mOwner; }
		inline const ObjectType& getObjectType() { return mOwner->objectType; }

		WorldMaterialID getMaterial(const ObjectBinding* pBinding);
		bool searchMaterial(const ObjectBinding* pBinding, WorldMaterialID& matID);

	protected:

		void destroyNotBound();

	protected:

		typedef SpatialTriContainer Buffer;

		struct Binding : ObjectBinding {

			WorldMaterialID materialID;
		};

	protected:

		typedef WETL::StaticArray<Binding, false, unsigned int> Bindings;
		typedef WETL::PtrStaticArray<SpatialTriContainer*, true, unsigned int> Buffers;

		SoftPtr<Object> mOwner;

		Bindings mBindings;
		HardPtr<Buffers> mBuffers;
	};
	
	
}

#endif