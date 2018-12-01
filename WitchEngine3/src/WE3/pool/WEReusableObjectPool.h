#ifndef _WEReusableObjectPool_h
#define _WEReusableObjectPool_h

#include "../WETL/WETLArray.h"
#include "../WEMem.h"

namespace WE {

	template <class ReusableObjectType, class IndexT>
	class ReusableObjectPool {
	protected:

		typedef WETL::CountedPtrArray<ReusableObjectType*, true, IndexT> AvailableObjPtrArray;
		AvailableObjPtrArray availableObjects;

	public:

		ReusableObjectPool() {
		}
		
		~ReusableObjectPool() {

			destroy();
		}

		void init(IndexT initObjectCount) {
			
			availableObjects.reserve(initObjectCount);
	
			for (availableObjects.count = 0; availableObjects.count < initObjectCount; availableObjects.count++) {

				MMemNew(availableObjects.el[availableObjects.count], ReusableObjectType());
			}
		}

		void destroy() {

			//we do this becuase of the way StrongPtrPackedArray destroys elements
			availableObjects.count = availableObjects.size;
			availableObjects.destroy();
		}
			
		
		ReusableObjectType* get() {

			if (availableObjects.count == 0) {

				IndexT oldSize = availableObjects.size;

				availableObjects.count = oldSize;
				availableObjects.reserve(availableObjects.size * 2);
				
				for (IndexT i = oldSize; i < availableObjects.size; i++) {

					MMemNew(availableObjects.el[i], ReusableObjectType());

				}
				
			}

			IndexT index = availableObjects.size - availableObjects.count;
			availableObjects.count--;

			ReusableObjectType* ret = availableObjects.el[index];
			availableObjects.el[index] = NULL;

			return ret;
		}

		void put(ReusableObjectType* pObject) {


			availableObjects.count++;
			IndexT index = availableObjects.size - availableObjects.count;
	

			availableObjects.el[index] = pObject;
		}
	};

}

#endif