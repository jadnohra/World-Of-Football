#ifndef h_WOF_match_CollRecord
#define h_WOF_match_CollRecord

#include "WE3/WEPtr.h"
#include "WE3/object/WEObject.h"
#include "WE3/pool/WEReusableObjectPool.h"
#include "WE3/math/WEVolume.h"
#include "WE3/spatialManager/WESpatialObjects.h"
#include "WE3/render/WERenderer.h"
#include "WE3/math/WEContactDepth.h"
using namespace WE;

#include "../DataTypes.h"

namespace WOF { namespace match {

	struct CollRecord {
	};

	struct CollRecordTri : CollRecord {

		SoftPtr<ObjectBinding> binding;
		const TriangleEx1* tri;

		void render(Renderer& renderer, const RenderColor* pTriColor, bool triWireFrame = true);
	};

	struct CollRecordVol : CollRecord {

		SoftPtr<SpatialVolumeBinding> binding;
		SoftPtr<VolumeStruct> vol;

		void render(Renderer& renderer, bool renderVolumes, const RenderColor* pVolColor);

		inline const ObjectType& getObjectType() { return binding->getObjectType(); }
		inline Object* getOwner() { return binding->owner; }
	};

	typedef ReusableObjectPool<CollRecordTri, UINT> CollRecordPoolTri;
	typedef ReusableObjectPool<CollRecordVol, UINT> CollRecordPoolVol;

	class CollRecordPool2 {
	public:

		CollRecordPoolTri tris;
		CollRecordPoolVol vols;
	};

} }

#endif