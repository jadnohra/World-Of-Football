#ifndef _WETLHashMap_h
#define _WETLHashMap_h

#include "WETLDataTypes.h"
#include "WETLResize.h"
#include "WETLArray.h"
#include "../WEMem.h"
#include "../WEMacros.h"
#include "../WEString.h"
#include "../WEAssert.h"

#include "malloc.h"
#include "stdlib.h"


namespace WETL {

	using namespace WE;

	/*
	KeyT must support:
		* operator= ()
		* opertator (IdxT) () const
		* opertator== () const
	*/

	

	template<class KeyT, class ValueT, class IdxT, class ManagerT, class ResizeT, long StartHashArraySize = 4>
	class HashMapT {
	protected:

		struct HashMapCell {

			bool isRemoved;
			IdxT index;
			
			KeyT key;
			ValueT value;

			HashMapCell* next;

			HashMapCell() {

				isRemoved = false;
				next = NULL;
			};

			HashMapCell(IdxT idx, KeyT k, ValueT v) {

				isRemoved = false;
				index = idx;

				key = k;
				value = v;

				next = NULL;
			};
		};

		typedef IdxT Index;

		typedef WETL::CountedPtrArray<HashMapCell*, false, IdxT> CellPtrArray;
		CellPtrArray mCells;
		typedef WETL::CountedArray<IdxT, false, IdxT> IndexArray;
		IndexArray mRemovedCellIndices;

		typedef WETL::StaticArray<HashMapCell*, true, IdxT> HashArray;
		HashArray mHashArray;

		IdxT mActiveCount;
		IdxT mMainCellCount;
		IdxT mChainedCellCount;

		#pragma warning( push )
		#pragma warning( disable : 4311 )

		inline IdxT hashIndex(const KeyT& key) {

			return (((IdxT) key) % mHashArray.size);
		}

		#pragma warning( pop )
		
		bool _putCell(HashMapCell& cell) {
			
			IdxT index = hashIndex(cell.key);
						
			if (mHashArray.el[index]) {

				HashMapCell* pCell = mHashArray.el[index];
				if ((pCell->key == cell.key)) {

					assrt(false);
					return false;
				}

				while (pCell->next) {

					pCell = pCell->next;

					if (pCell->key == cell.key) {

						assrt (false);
						return false;
					}
				}

				pCell->next = &cell;
				mChainedCellCount++;

			} else {

				mHashArray.el[index] = &cell;
				mMainCellCount++;
			}

			mActiveCount++;
			return true;
		}

		void _rehash() {

			for (IdxT i = 0; i < mCells.count; i++) {
				
				if (mCells.el[i]) {

					mCells.el[i]->next = NULL;
					_putCell(*mCells.el[i]);
				}
			}
		}

		void _resetCounts() {

			mActiveCount = 0;
			mMainCellCount = 0;
			mChainedCellCount = 0;
		}

		void _resizeHashArray(IdxT newSize) {

			if (mHashArray.size >= newSize) {

				return;
			}

			mHashArray.setSize(newSize);
			
			_resetCounts();
			_rehash();
		}

		inline void _resizeHashArrayIfNeeded() {

			if (mHashArray.size == 0) {

				_resizeHashArray(StartHashArraySize);
				return;
			}
			
			if (mChainedCellCount >= mHashArray.size) {

				_resizeHashArray(mHashArray.size * 2);
			}
		}

		bool _remove(const IdxT& index, const KeyT& key/*, ValueT& value*/) {

			//IdxT index = hashIndex(key);
			HashMapCell* pCell = mHashArray.el[index];
			HashMapCell* pPrevCell = NULL;
			
			if (pCell) {

				while ((pCell->key == key) == false) {
					
					pPrevCell = pCell;
					pCell = pCell->next;

					if (pCell == NULL) {

						return false;
					}
				}

				ManagerT::onRemoved(pCell->value/*, value*/);
				//value = pCell->value; done in onRemoved
				pCell->isRemoved = true;
				

				if (pPrevCell) {

					mChainedCellCount--;
					pPrevCell->next = pCell->next;

				} else {

					if (pCell->next) {

						mChainedCellCount--;

					} else {

						mMainCellCount--;
					}

					//mHashArray.el[index] = pCell->next;
					mHashArray.el[index] = NULL;
				}

				pCell->next = NULL;

				mActiveCount--;
				mRemovedCellIndices.addOne(pCell->index);
			}

			return true;
		}

	public:

		typedef KeyT Key;
		typedef ValueT Value;
		typedef IdxT Index;
		typedef IdxT Iterator;

		HashMapT() {

			_resetCounts();
		}

		~HashMapT() {

			destroy();
		}

		IdxT getCount() {

			return mActiveCount;
		}

		void destroy() {

			for (IdxT i = 0; i < mCells.count; i++) {
				
				if (mCells.el[i]) {

					ManagerT::destroyElement(mCells.el[i]->value);
				}
			}

			mHashArray.destroy();
			mRemovedCellIndices.destroy();
			mCells.destroy();

			_resetCounts();
		}

#ifdef _DEBUG

		void debug(TCHAR* formatKey, TCHAR* formatValue) {
			
			IdxT counter = 0;
			for (IdxT i = 0; i < mCells.count; i++) {
				
				if (mCells.el[i]) {

					String::debug(L"%d [ ", counter++);
					String::debug(formatKey, hashIndex(mCells.el[i]->key));
					String::debug(L" < ");
					String::debug(formatKey, mCells.el[i]->key);
					String::debug(L" - ");
					String::debug(formatValue, mCells.el[i]->value);
					String::debug(L" > ]%s %s\n", mCells.el[i]->next ? L"*": L"", mCells.el[i]->isRemoved ? L"R": L"");
				}
			}

		}
#else
		inline void debug(TCHAR* formatKey, TCHAR* formatValue);
#endif

		void reserve(IdxT size) {

			mCells.reserve(size);
			mRemovedCellIndices.reserve(size / 8 > 0 ? size / 8 : 1);

			_resizeHashArray(size);
		}
		
		
		bool remove(KeyT key) {

			return _remove(hashIndex(key), key);
		}

		void removeAll() {
			
			assrt(false);
		}
				
		bool insert(KeyT key, ValueT val) {

			IdxT index;
			HashMapCell* pCell;

			if (mRemovedCellIndices.count) {
			
				index = mRemovedCellIndices.el[--mRemovedCellIndices.count];
				pCell = mCells.el[index];
				
				assrt(pCell->isRemoved == true);
				pCell->isRemoved = false;
				pCell->key = key;
				pCell->value = val;

			} else {

				if (mActiveCount == mCells.size) {

					unsigned long newSize = ResizeT::resize(mCells.size, mCells.size + 1);
					assrt(newSize == (unsigned long) ((IdxT) newSize));
					mCells.reserve((IdxT) newSize);

				}
				_resizeHashArrayIfNeeded();

				index = mCells.addOneIndex();
				WE_MMemNew(pCell, HashMapCell(index, key, val));
				mCells.el[index] = pCell;
			}
			
			
			
			if (_putCell(*pCell) == false) {

				//should we really do this?
				//we do this now because normally this fct. should always
				//succeed and callers can 'count' on this, when it fails
				//assrt is thrown and val is 'deleted' so that the caller
				//doesnt have to do it, for all current uses this is ok.
				ManagerT::onRemoved(val);
				return false;
			}

			ManagerT::onAdded(val);
			return true;
		}

		bool hasKey(KeyT key) {
			
			if (mHashArray.size == 0) {

				return false;
			}

			IdxT index = hashIndex(key);

			assrt(index >= 0);

			HashMapCell* pCell = mHashArray.el[index];
			
			if (pCell) {

				while ((pCell->key == key) == false) {
					
					pCell = pCell->next;

					if (pCell == NULL) {

						return false;
					}
				}

				return true;
			}
			
			return false;
		}

		bool find(KeyT key, ValueT& val) {
			
			if (mHashArray.size == 0) {

				return false;
			}

			IdxT index = hashIndex(key);
			HashMapCell* pCell = mHashArray.el[index];
			
			if (pCell) {

				while ((pCell->key == key) == false) {
					
					pCell = pCell->next;

					if (pCell == NULL) {

						return false;
					}
				}

				ManagerT::onFound(pCell->value);
				val = pCell->value;
				return true;

			}
			
			return false;
		}

		IdxT iterator() {

			return 0;
		}

		bool next(IdxT& iter, ValueT& value) {

			for (; iter < mCells.count; iter++) {

				if (mCells.el[iter] && (mCells.el[iter]->isRemoved == false)) {
					
					ManagerT::onFound(mCells.el[iter]->value);
					value = mCells.el[iter]->value;
					iter++;
					return true;
				}
			}

			return false;
		}

		
		bool next(IdxT& iter, ValueT& value, KeyT& key) {

			for (; iter < mCells.count; iter++) {

				if (mCells.el[iter] && (mCells.el[iter]->isRemoved == false)) {
					
					ManagerT::onFound(mCells.el[iter]->value);
					key = mCells.el[iter]->key;
					value = mCells.el[iter]->value;
					iter++;
					return true;
				}
			}

			return false;
		}

		bool removeAndNext(IdxT& iter, ValueT& value/*, ValueT& remValue*/) {
			
			IdxT& remIndex = iter;
			bool nextRet = next(iter, value);
			
			_remove(remIndex, mCells.el[remIndex]->key/*, remValue*/);

			return nextRet;
		}

		
	};

	template<class KeyT, class ValueT, class IdxT, class ResizeT = ResizeDouble, long StartHashArraySize = 4>
	class HashMap : public HashMapT<KeyT, ValueT, IdxT, ObjectManager<ValueT, IdxT>,  ResizeT, StartHashArraySize> {
	public:

		bool searchByIterator(const IdxT& iter, ValueT& value) {

			IdxT i = iterator();
			IdxT stopIndex = iter + 1;

			while(next(i, value)) {

				if (i == stopIndex) {

					return true;
				}
			}

			return false;
		}

		bool findIterationIndex(const ValueT& value, IdxT& iterIndex) {

			ValueT tempVal;
			IdxT i = iterator();

			while(next(i, tempVal)) {

				if (value == tempVal) {

					iterIndex = i - 1;
					return true;
				}
			}

			return false;
		}

	};

	template<class KeyT, class ValueT, class IdxT, class ResizeT = ResizeDouble, long StartHashArraySize = 4>
	class PtrHashMap : public HashMapT<KeyT, ValueT, IdxT, ObjectManagerPtr<ValueT, IdxT>,  ResizeT, StartHashArraySize> {
	public:	

		bool searchByIterator(const IdxT& iter, ValueT& value) {

			IdxT i = iterator();
			IdxT stopIndex = iter + 1;

			while(next(i, value)) {

				if (i == stopIndex) {

					return true;
				}
			}

			return false;
		}

		bool findIterationIndex(const ValueT& value, IdxT& iterIndex) {

			ValueT tempVal;
			IdxT i = iterator();

			while(next(i, tempVal)) {

				if (value == tempVal) {

					iterIndex = i - 1;
					return true;
				}
			}

			return false;
		}
	};

	template<class KeyT, class ValueT, class IdxT, class ResizeT = ResizeDouble, long StartHashArraySize = 4>
	class RefHashMap : public HashMapT<KeyT, ValueT, IdxT, ObjectManagerRef<ValueT, IdxT>,  ResizeT, StartHashArraySize> {
	public:


		IdxT removeOneRefObjects() {

			IdxT removedCount = 0;
			IdxT i = iterator();
			bool doRemAndNext = false;
			ValueT pObject;
			
			while (true) {

				if (doRemAndNext == false) {

					if (next(i, pObject) == false) {

						return removedCount;
					}

				} else {

					++removedCount;

					if (removeAndNext(i, pObject) == false) {

						return removedCount;
					}
				}

				doRemAndNext = (pObject->GetRefCount() == 1);
			}
		}

	};
	

}

#endif