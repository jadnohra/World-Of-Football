#ifndef _WESpatialObjects_h
#define _WESpatialObjects_h

#include "WESpatialDataTypes.h"
#include "../object/WEObject.h"
#include "../object/WEObjectData.h"
#include "../WEPtr.h"
#include "../WEDataTypes.h"
#include "../math/WEVolume.h"
#include "../geometry/WEGeometryTriBufferContainer.h"

namespace WE {

	struct ObjectBinding {

		SoftPtr<Object> owner;
		ObjectData data;

		ObjectBinding(Object* pOwner = NULL) : owner(pOwner) {};

		void bindObject(Object* pOwner) { owner = pOwner; };
		void unbindObject() { owner.destroy(); };

		bool isBoundObject() { return owner.isValid(); }

		void bindData(void* pData) { data.ptr = pData; };
		void unbindData() { data.destroy(); };

		bool isBoundData() { return data.isValid(); }

		inline Object* getObject() { return owner; }
		inline const ObjectType& getObjectType() { return owner->objectType; }
	};

	struct SpatialVolumeBinding : ObjectBinding {

		Volume volume;
	};

	typedef GeometryTriBuffer SpatialTriContainer;

	class SpatialObjectGroup {
	public:

		typedef SpatialIndex Index;

		class TriContainer {
		public:

			virtual ObjectBinding* getBinding() = 0;
			virtual VolumeStruct* getBoundingVolume(VolumeRef& holder) { return NULL; } //if NULL no bouding volume for the tris has been calculated
		};

		//return false to interrupt
		typedef bool (*FctTriProcess) (const TriangleEx1& tri, void* context);

	public:

		virtual ~SpatialObjectGroup() {}

		virtual void reset() = 0;
		virtual void destroy() = 0;

		virtual Index getTriContainerCount() = 0;
		virtual TriContainer& getTriContainer(const Index& index) = 0;
		virtual	void getTris(const TriContainer& container, FctTriProcess fct, void* context) = 0;

		virtual Index getVolumeCount() = 0;
		virtual SpatialVolumeBinding* getVolume(const Index& index, VolumeStruct*& pVolumeOut) = 0;

		virtual Index getDynamicVolumeCount() = 0;
		virtual SpatialVolumeBinding* getDynamicVolume(const Index& index, VolumeStruct*& pVolumeOut) = 0;
	};
	
}

#endif