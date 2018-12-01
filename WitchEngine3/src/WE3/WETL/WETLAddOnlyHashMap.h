#ifndef _WETLAddOnlyHashMap_h
#define _WETLAddOnlyHashMap_h

#include "WETLDataTypes.h"
#include "WETLResize.h"
#include "WETLArray.h"
#include "../WEMacros.h"
#include "../WEString.h"



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
	class AddOnlyHashMapT {
	protected:

		struct HashMapCell {

			IdxT index;
			
			KeyT key;
			ValueT value;

			HashMapCell* next;

			HashMapCell() {

				next = NULL;
			};

			HashMapCell(IdxT idx, KeyT k, ValueT v) {

				index = idx;

				key = k;
				value = v;

				next = NULL;
			};
		};

		typedef WETL::CountedPtrArray<HashMapCell*, false, IdxT> CellPtrArray;
		CellPtrArray mCells;
		
		typedef WETL::StaticArray<HashMapCell*, true, IdxT> HashArray;
		HashArray mHashArray;

		IdxT mActiveCount;
		IdxT mMainCellCount;
		IdxT mChainedCellCount;

		inline IdxT hashIndex(const KeyT& key) {

			return ((IdxT) key % mHashArray.size);
		}

		
		void _putCell(HashMapCell& cell) {
			
			IdxT index = hashIndex(cell.key);
						
			if (mHashArray.el[index]) {

				HashMapCell* pCell = mHashArray.el[index];
				assert ((pCell->key == cell.key) == false);

				while (pCell->next) {

					pCell = pCell->next;
					assert ((pCell->key == cell.key) == false);
				}

				pCell->next = &cell;
				mChainedCellCount++;

			} else {

				mHashArray.el[index] = &cell;
				mMainCellCount++;
			}

			mActiveCount++;
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
		

	public:

		typedef KeyT Key;
		typedef ValueT Value;
		typedef IdxT Index;

		AddOnlyHashMapT() {

			_resetCounts();
		}

		~AddOnlyHashMapT() {

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
					String::debug(L" > ]%s\n", mCells.el[i]->next ? L"*": L"");
				}
			}

		}
#else
		inline void debug(TCHAR* formatKey, TCHAR* formatValue);
#endif

		void reserve(IdxT size) {

			mCells.reserve(size);

			_resizeHashArray(size);
		}
		
		
		void removeAll() {
			
			assrt(false);
		}
				
		void insert(KeyT key, ValueT val) {

			IdxT index;
			HashMapCell* pCell;

			{

				if (mActiveCount == mCells.size) {

					unsigned long newSize = ResizeT::resize(mCells.size, mCells.size + 1);
					assrt(newSize == (unsigned long) ((IdxT) newSize));
					mCells.reserve((IdxT) newSize);

				}
				_resizeHashArrayIfNeeded();

				index = mCells.addOneIndex();
				MMemNew(pCell, HashMapCell(index, key, val));
				mCells.el[index] = pCell;
			}
			
			
			ManagerT::onAdded(val);
			_putCell(*pCell);
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

				if (mCells.el[iter]) {
					
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

				if (mCells.el[iter]) {
					
					ManagerT::onFound(mCells.el[iter]->value);
					key = mCells.el[iter]->key;
					value = mCells.el[iter]->value;
					iter++;
					return true;
				}
			}

			return false;
		}

	};

	template<class KeyT, class ValueT, class IdxT, class ResizeT = ResizeDouble, long StartHashArraySize = 4>
	class AddOnlyHashMap : public AddOnlyHashMapT<KeyT, ValueT, IdxT, ObjectManager<ValueT, IdxT>,  ResizeT, StartHashArraySize> {
	};

	template<class KeyT, class ValueT, class IdxT, class ResizeT = ResizeDouble, long StartHashArraySize = 4>
	class AddOnlyPtrHashMap : public AddOnlyHashMapT<KeyT, ValueT, IdxT, ObjectManagerPtr<ValueT, IdxT>,  ResizeT, StartHashArraySize> {
	};

	template<class KeyT, class ValueT, class IdxT, class ResizeT = ResizeDouble, long StartHashArraySize = 4>
	class AddOnlyRefHashMap : public AddOnlyHashMapT<KeyT, ValueT, IdxT, ObjectManagerRef<ValueT, IdxT>,  ResizeT, StartHashArraySize> {
	};
	

}

#endif