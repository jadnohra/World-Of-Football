#ifndef _WENPGeometryCollider_h
#define _WENPGeometryCollider_h

#include "../../../mesh/WEMesh.h"
#include "../WECollFaceBuffer.h"
#include "../WECollMaterialManager.h"
#include "../../../math/coll/WECollContact.h"
#include "../../../render/WERenderer.h"

namespace WE {

	struct MaterialCollFaceBufferGroup {

		typedef WETL::StaticArray<MaterialCollFaceBuffer, false, MeshGeometryPartIndex::Type> FaceBufferArray;
		
		FaceBufferArray mBufferArray;
	};

	class NPGeometryCollider {
	public:

		struct CollIterator {

			MeshGeometryPartIndex::Type partIndex;
			CollPrimIndex faceIndex;
		};

	public:

		bool create(Mesh& mesh, CollMaterialManager* pMatManager, const CollMaterial* pCollMaterialOverride);
	
		bool collIterStart(CollIterator& iter);
		bool collIterNext_sweptContact(CollIterator& iter, const float& sphereRadius, const Vector3& startPos, const Vector3& posDiff, SweptContact& sweptContact, CollMaterial& mat);
		void collIterEnd(CollIterator& iter);

		void render(Renderer& renderer, const RenderColor* pColor);

		void init(Mesh& mesh, const CollFaceTransform* pTransf);
		void transform(const CollFaceTransform& trans);

	public:

		typedef WETL::StaticArray<MeshGeometryPartIndex::Type, false, MeshGeometryPartIndex::Type> MeshPartBindingArray;

		MaterialCollFaceBufferGroup mGroup;
		MeshPartBindingArray mBindArray;

	};

}


#endif