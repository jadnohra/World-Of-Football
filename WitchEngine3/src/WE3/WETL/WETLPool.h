#ifndef _WETLRefPool_h
#define _WETLRefPool_h

#include "../WERef.h"
#include "WETLArray.h"

namespace WETL {

	//indexes returned by pools are always valid 

	//a faster way to reuse empty (NULL) space can be used,
	//using a Vector instead of an array per example
	//but that costs memory, so another version of the pools
	//has to be coded for critical performance tasks

	template<class RefT, class IdxT, class ResizeT = WETL::ResizeDouble>
	class RefPool {
	public:

		RefPool() : mNullCount(0) {
		}

		~RefPool() {

			destroy();
		}

		void destroy() {

			mRefs.destroy();
			mNullCount = 0;
		}

		IdxT getFreeCount() {

			return mRefs.count + mNullCount;
		}

		void reserveCount(IdxT count) {

			IdxT freeCount = getFreeCount();

			if (freeCount >= count) {

				return;
			}

			mRefs.reserveCount(count - freeCount);
		}

		IdxT getFreeIndex() {

			IdxT ret;

			if (mNullCount) {

				for (ret = 0; ret < mRefs.count; ret++) {

					if (mRefs.el[ret] == NULL) {

						mNullCount--;
						return ret;
					}
				}

				assrt(false);
			}

			return mRefs.addOneIndex(NULL);
		}

		IdxT add(RefT* pRef) {
			
			IdxT index;

			pRef->AddRef();
			mRefs.el[(index = getFreeIndex())] = pRef;

			return index;
		}

		void remove(IdxT index) {

			if (index == mRefs.count) {

				mRefs.removeOne();

			} else {

				mRefs.el[index]->Release();
				mRefs.el[index] = NULL;

				mNullCount++;
			}
		}

		RefT* get(IdxT index) {

			mRefs.el[index]->AddRef();
			return mRefs.el[index];
		}

		bool isUnused(IdxT index) {

			return (mRefs.el[index]->mRefCount == 1);
		}

		IdxT removeUnused() {

			IdxT removedCount = 0;

			for (IdxT i = 0; i < mRefs.count; i++) {

				if (isUnused(i)) {
					
					remove(i);
					removedCount++;
				}
			}

			return removedCount;
		}

	protected:

		typedef CountedRefArray<RefT, false, IdxT, ResizeT> RefArray;
		RefArray mRefs;

		IdxT mNullCount;
	};



	template<class PtrT, class IdxT, class ResizeT = WETL::ResizeDouble>
	class PtrPool {
	public:

		PtrPool() : mNullCount(0) {
		}

		~PtrPool() {

			destroy();
		}

		void destroy() {

			mPtrs.destroy();
			mNullCount = 0;
		}

		IdxT getFreeCount() {

			return mPtrs.count + mNullCount;
		}

		void reserveCount(IdxT count) {

			IdxT freeCount = getFreeCount();

			if (freeCount >= count) {

				return;
			}

			mPtrs.reserveCount(count - freeCount);
		}

		IdxT getFreeIndex() {

			IdxT ret;

			if (mNullCount) {

				for (ret = 0; ret < mPtrs.count; ret++) {

					if (mPtrs.el[ret] == NULL) {

						mNullCount--;
						return ret;
					}
				}

				assrt(false);
			}

			return mPtrs.addOneIndex(NULL);
		}

		IdxT add(PtrT pPtr) {
			
			IdxT index;

			mPtrs.el[(index = getFreeIndex())] = pPtr;

			return index;
		}

		void remove(IdxT index) {

			if (index == mPtrs.count) {

				mPtrs.removeOne();

			} else {

				mPtrs.el[index] = NULL;

				mNullCount++;
			}
		}

		PtrT get(IdxT index) {

			return mPtrs.el[index];
		}
		
		IdxT getSize() {

			return mPtrs.count;
		}

	protected:

		typedef CountedPtrArray<PtrT, false, IdxT, ResizeT> PtrArray;
		PtrArray mPtrs;

		IdxT mNullCount;
	};

}


#endif