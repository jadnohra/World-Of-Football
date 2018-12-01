#ifndef _WESpatialManager_KDTreeTriBufferManager_h
#define _WESpatialManager_KDTreeTriBufferManager_h

#include "../WESpatialManager.h"
#include "../../geometry/WEGeometryTriBuffer.h"

namespace WE {

	class SpatialManager_KDTreeTriBufferManager {
	public:

		typedef SpatialIndex Index;

		class TriBuffer : public GeometryTriBuffer {
		public:

			SoftPtr<ObjectBinding> binding;
		};

	public:

		void reserve(const Index& size) { mTriBuffers.reserve(size); }
		void add(SpatialTriContainer& spatial, ObjectBinding* pBinding);

		inline const Index& getBufferCount() { return mTriBuffers.count; } 
		inline TriBuffer& getBuffer(const Index& i) { return dref(mTriBuffers.el[i]); }

	protected:

		typedef WETL::CountedPtrArrayEx<TriBuffer, Index> TriBuffers;

		TriBuffers mTriBuffers;
	};

}

#endif