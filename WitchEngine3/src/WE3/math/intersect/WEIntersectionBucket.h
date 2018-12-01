#ifndef _WEIntersectionBucket_h
#define _WEIntersectionBucket_h

#include "../../WEDataTypes.h"
#include "../../WETL/WETLSizeAlloc.h"
#include "../../WETL/WETLPool.h"
#include "../../pool/WEReusableObjectPool.h"

namespace WE {

	template<typename ElementTypeT>
	class IntersectionBucket {
	public:

		typedef UINT32 Index;
		typedef ReusableObjectPool<IntersectionBucket<ElementTypeT>, Index> Pool;
	
	public:

		inline void destroy() {

			mElements.destroy();
		}

		//removes all elements without deallocating memory
		inline void reset() {

			mElements.count = 0;
		}

		inline void reserve(Index count) {

			mElements.reserve(count);
		}


		inline ElementTypeT& addOne() {
			
			return mElements.addOne();
		}

		inline void putBackLast() {

			--mElements.count;
		}

		inline const Index& getCount() {

			return mElements.count;
		}

		inline ElementTypeT& getElment(const Index& index) {

			return mElements.el[index];
		}

	protected:

		typedef WETL::CountedArray<ElementTypeT, false, Index> Elements;

		Elements mElements;
	};

}

#endif