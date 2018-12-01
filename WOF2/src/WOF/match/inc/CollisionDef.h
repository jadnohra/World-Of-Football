#ifndef h_WOF_match_CollisionDef
#define h_WOF_match_CollisionDef

#include "../DataTypes.h"
#include "Index.h"

#include "float.h"

#include "WE3/defaultImpl/world/WEWorldMaterial.h"
#include "WE3/spatialManager/WESpatialObjects.h"
using namespace WE;

namespace WE {

	struct CollMaskMatrix;
	class WorldMaterialManager;
}

namespace WOF { namespace match {

	const float kSweptFractionInf = FLT_MAX;
	
	enum SweptFractionType;
	
	struct SweptIntersection;

	enum SweptContactDynamicType;

	struct SweptContactDynamics;

	struct SweptIntersectionArray;
	struct SweptIntersectionPtrArray;

	class ContactGeometry;
	class CollContact;

	class VolumeCollider;
	class DynamicVolumeCollider;
	class PolygonalCollider;

	typedef PolygonalCollider MeshCollider;

	struct CollRecord;
	struct CollRecordTri;
	struct CollRecordVol;

	class CollRecordPool2;

	class CollEngine;

} }

#endif