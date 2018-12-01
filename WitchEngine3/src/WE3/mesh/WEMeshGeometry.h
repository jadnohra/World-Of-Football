#ifndef _WEMeshGeometry_h
#define _WEMeshGeometry_h

#include "../WECoordSys.h"

#include "WEMeshGeometryPartArray.h"
#include "WEMeshGeometryPartBindings.h"

#include "../geometry/WEGeometryTransform.h"
#include "../geometry/WEGeometryAnchor.h"

namespace WE {


	class MeshGeometry {
	public:

		MeshGeometry();
		~MeshGeometry();

		bool hasParts();

		
		bool hasUnifiedCoordSys();
		const CoordSys& getUnifiedCoordSys();
		void unifyCoordSys(CoordSysConvPool& convPool, const CoordSys* pCoordSys);

		//apply to current parts
		void applyCoordSys(CoordSysConvPool& convPool, const CoordSys& coordSys);
		void applyTransform(GeometryVertexTransform* pTrans);
		void anchorGetOffset(const GeometryAnchorType* anchor, Vector3& anchorOffset);
		void reanchor(float* anchoringOffset);

		//void transform(MeshGeometryTransform& transform);
		void extractVolume(AAB& volume, bool cache);

		void destroyCache(bool volumeCache);

		bool setPartCount(MeshGeometryPartIndex::Type count);
		MeshGeometryPartIndex::Type getPartCount();
		
		bool createPart(MeshGeometryPartIndex::Type index, MeshGeometryPart** ppPart = NULL);
		MeshGeometryPart& getPart(MeshGeometryPartIndex::Type index);
		MeshGeometryPartBindings& getPartBindings(MeshGeometryPartIndex::Type index);

	protected:

		CoordSys mUnifiedCoordSys;
		StaticMeshGeometryPartArray mParts;
		StaticMeshGeometryPartBindingsArray mPartBindings;
		HardPtr<AAB> mCachedVolume;
	};
}

#endif