#ifndef _WEICollisionDetector_h
#define _WEICollisionDetector_h

#include "../../object/WEObject.h"
#include "../../object/WEObjectData.h"
#include "../../object/WEEventPointer.h"
#include "../../math/WEVolume.h"
#include "../../WERef.h"
#include "../../WEPtr.h"
#include "../../render/WERenderer.h"

#include "../WECollDataTypes.h"

namespace WE {

	class ICollDetectListener {
	public:

		virtual void collStarted(Object* pObject1, Object* pObject2) = 0;
		virtual void collEnded(Object* pObject1, Object* pObject2) = 0;
	};

	class ICollDetector : public Ref {
	public:

		virtual ~ICollDetector() {};

		virtual void setMaskMatrix(const CollMaskMatrix* pMatrix);

		virtual bool hasBinding(Object& object, ObjectData& data) = 0;
		virtual bool createBinding(Object& object, ObjectData& data, const CollMask& mask) = 0;
		virtual void destroyBinding(Object& object, ObjectData& data) = 0;

		virtual bool extractCollMask(Object& object, ObjectData& data, CollMask& mask) = 0;

		virtual void add(Object& object, ObjectData& data, VolumeStruct& volume) = 0;
		virtual void update(Object& object, ObjectData& data, VolumeStruct& volume) = 0;
		virtual void update(Object& object, ObjectData& data, VolumeStruct& volume, ObjectData& volumeCache) = 0;
		virtual void remove(Object& object, ObjectData& data, bool destroyBinding, bool isRemovingAllObjects) = 0;

		virtual void addRay(Object& object, ObjectData& data, const CollMask& mask, Point& origin) = 0;
		virtual void updateRay(Object& object, ObjectData& data, Point& origin) = 0;
		virtual void fireRay(Object& object, ObjectData& data, Vector& direction, Object*& pHitObject) = 0;
		virtual void removeRay(Object& object, ObjectData& data, bool isRemovingAllObjects) = 0;

		//state objects are reusable, so use that
		virtual void createVolumeCache(ObjectData& cache) = 0;
		virtual void destroyVolumeCache(ObjectData& cache) = 0;
		virtual void initVolumeCache(VolumeStruct& volume, ObjectData& cache) = 0;
		virtual void initVolumeCache(Object& object, ObjectData& data, ObjectData& cache) = 0;

		virtual void setListener(ICollDetectListener* pListener);
		virtual void enableListener(bool enable);

		virtual void render(Renderer& renderer, Object& object, ObjectData& data, const RenderColor* pRenderColor) = 0;

	protected:

		ICollDetector();

		SoftPtr<ICollDetectListener> mListener;
		bool mIsEnabledListener;

		const CollMaskMatrix* mpCollMaskMatrix;
	};

}

#endif