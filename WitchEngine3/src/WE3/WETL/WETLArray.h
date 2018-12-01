#ifndef _WETLArray_h
#define _WETLArray_h

#include "../WEMem.h"
#include "../WEMacros.h"
#include "../WEAssert.h"


#include "WETLDataTypes.h"
#include "WETLObjectManager.h"
#include "WETLResize.h"
#include "WETLArrayBase.h"


namespace WETL {


	//TODO add memory allocator in template params
	//also use calloc when zeroNew is set??

	#define IdxTNull ((IdxT) -1)

	
	
	template <class T, bool zeroNew, class IdxT, class ManagerT> 
	class StaticArrayT : public ArrayBase<T, IdxT> {
	public:

		StaticArrayT() {
		}

		~StaticArrayT() {

			destroy();
		}

		/*
		void memoryCopy(StaticArrayT<T, zeroNew, IdxT, ManagerT>& copy) {

			copy.setSize(size);
			memcpy(copy.el, el, size * sizeof(T));
		}
		*/

		void memoryCopyFrom(ArrayBase<T, IdxT>& ref) {

			setSize(ref.size);
			memcpy(el, ref.el, size * sizeof(T));
		}

		IdxT indexOf(T* arrayElement) {

			return arrayElement - el;
		}
		
		void setSize(IdxT newSize) {

			destroy();

			if (newSize == 0) {
				return;
			}

			T* pNewArray;
			WE_MMemNew(pNewArray, T[newSize]);

			if (zeroNew) {
				memset(pNewArray + size, 0, (newSize - size) * sizeof(T));
			}
			el = pNewArray;

			size = newSize;
		}

		void destroy() {

			if (size) {

				ManagerT::destroyElements(el, size);

				WE_MMemDeleteArray(el);
				size = 0;
			}
		}

	};

	template <class T, bool zeroNew, class IdxT> 
	class StaticArray : public StaticArrayT<T, zeroNew, IdxT, ObjectManager<T, IdxT> > {
	};

	template <class PtrT, bool zeroNew, class IdxT> 
	class PtrStaticArray : public StaticArrayT<PtrT, zeroNew, IdxT, ObjectManagerPtr<PtrT, IdxT> > {
	};

	template <class RefT, bool zeroNew, class IdxT> 
	class RefStaticArray : public StaticArrayT<RefT, zeroNew, IdxT, ObjectManagerRef<RefT, IdxT> > {
	};

	

	template <class T, bool zeroNew, class IdxT, class ManagerT> 
	class ArrayT : public ArrayBase<T, IdxT> {
	public:

		ArrayT() {
		}

		~ArrayT() {
			destroy();
		}


		IdxT indexOf(T* arrayElement) {
			return arrayElement - el;
		}
		

		void reserve(IdxT newSize) {

			if (newSize == 0 || newSize <= size) {
				return;
			}

			T* pNewArray;
			WE_MMemNew(pNewArray, T[newSize]);
			ManagerT::moveElements(size, el, pNewArray);
			if (zeroNew) {
				memset(pNewArray + size, 0, (newSize - size) * sizeof(T));
			}
			WE_MMemDeleteArray(el);
			el = pNewArray;

			size = newSize;
		}

		T& addOne() {

			IdxT index = size;

			reserve(size + 1);

			return el[index];
		}

		const T& addOne(const T& obj) {

			IdxT index = size;

			reserve(size + 1);

			el[index] = obj;

			return el[index];
		}

		void destroy() {

			if (size) {

				ManagerT::destroyElements(el, size);
				WE_MMemDeleteArray(el);
				size = 0;
			}
		}

	};


	template <class T, bool zeroNew, class IdxT> 
	class Array : public ArrayT<T, zeroNew, IdxT, ObjectManager<T, IdxT> > {
	};

	template <class PtrT, bool zeroNew, class IdxT> 
	class PtrArray : public ArrayT<PtrT, zeroNew, IdxT, ObjectManagerPtr<PtrT, IdxT> > {
	};

	
	template <class T, class IdxT, bool PtrReuse = false> 
	class PtrArrayEx : public PtrArray<T*, PtrReuse ? true : false, IdxT> {
	public:

		typedef T* PtrT;

	public:

		T*& addNewOne() {

			T*& pEl = addOne();

			if(PtrReuse) {

				if (!pEl)
					WE_MMemNew(pEl, T());

			} else {

				WE_MMemNew(pEl, T());
			}
			
			return pEl;
		}
	};

	template <class RefT, bool zeroNew, class IdxT> 
	class RefArray : public ArrayT<RefT, zeroNew, IdxT, ObjectManagerRef<RefT, IdxT> > {
	};
	

	
	
	
	template <class T, bool zeroNew, class IdxT, class ManagerT, class ResizeT> 
	class CountedArrayT : public CountedArrayBase<T, IdxT> {
	public:


		CountedArrayT() {
		}

		CountedArrayT(IdxT reserved) {

			reserve(reserved);
		}

		~CountedArrayT() {

			destroy();
		}

		IdxT indexOf(T* arrayElement) {
			return arrayElement - el;
		}
		

		IdxT lastIndex() { assrt(count > 0); return count - 1; }

		void reserveCount(IdxT freeCount) {
			
			IdxT free = size - count;

			if (freeCount > free) {

				WE::assertSafeOp<IdxT>(count, +freeCount);

				reserve(count + freeCount);
			}
		}

		T& makeSpaceForOne() {
			
			if (size == count) {

				WE::assertSafeOp<IdxT>(count, +1);

				reserve(count + 1);
			}

			return el[count];
		}

		void reserve(IdxT newSize) {

			if (newSize == 0 || newSize <= size) {

				return;
			}

			T* pNewArray;
			WE_MMemNew(pNewArray, T[newSize]);
			ManagerT::moveElements(count, el, pNewArray);

			if (zeroNew) {

				memset(pNewArray + size, 0, (newSize - size) * sizeof(T));
			}

			WE_MMemDeleteArray(el);
			el = pNewArray;

			size = newSize;
		}

		
		void destroy() {

			if (size != 0) {

				ManagerT::destroyElements(el, count);
				WE_MMemDeleteArray(el);
				size = 0;
				count = 0;
			}
		}

		IdxT add(IdxT num = 1, bool autoReserve = true) {

			WE::assertSafeOp<IdxT>(count, +num);

			if(count + num > size) {
				
				if (autoReserve) {

					IdxT newSize;
					newSize = ResizeT::resize(size, count + num);
					
					reserve(newSize);

				} else {

					return (IdxTNull);
				}
			}

			IdxT index = count;
			count += num;

			return index;
		};

		IdxT addOneIndexConservativeResize() {

			WE::assertSafeOp<IdxT>(count, +1);

			if(count + 1 > size) {

				reserve(size == 0 ? 1 : size + 1);
			}

			return count++;
		};

		IdxT addOneIndex() {

			WE::assertSafeOp<IdxT>(count, +1);

			if(count + 1 > size) {

				unsigned long newSize = ResizeT::resize(size, size + 1);
				WE::assrt(newSize == (unsigned long) ((IdxT) newSize));
				reserve((IdxT) newSize);
			}

			return count++;
		};

		IdxT addOneIndex(const T& value) {

			WE::assertSafeOp<IdxT>(count, +1);

			if(count + 1 > size) {

				unsigned long newSize = ResizeT::resize(size, size + 1);
				WE::assrt(newSize == (unsigned long) ((IdxT) newSize));
				reserve((IdxT) newSize);
			}

			el[count] = value;
			return count++;
		};

		T& addOne() {
			
			WE::assertSafeOp<IdxT>(count, +1);

			if(count + 1 > size) {

				unsigned long newSize = ResizeT::resize(size, size + 1);
				WE::assrt(newSize == (unsigned long) ((IdxT) newSize));
				reserve((IdxT) newSize);
			}

			return el[count++];
		}

		const T& addOne(const T& value) {
			
			WE::assertSafeOp<IdxT>(count, +1);

			if(count + 1 > size) {

				unsigned long newSize = ResizeT::resize(size, size + 1);
				WE::assrt(newSize == (unsigned long) ((IdxT) newSize));
				reserve((IdxT) newSize);
			}

			el[count++] = value;
			return value;
		}

		//slow overwrite
		const T& addOneAt(const IdxT& atIndex, const T& value) {
			
			WE::assertSafeOp<IdxT>(count, +1);

			if(count + 1 > size) {

				unsigned long newSize = ResizeT::resize(size, size + 1);
				WE::assrt(newSize == (unsigned long) ((IdxT) newSize));
				reserve((IdxT) newSize);
			}

			//using old count
			ManagerT::moveElements((count - atIndex), el + atIndex, el + atIndex + 1);

			el[atIndex] = value;
			++count;
			return value;
		}
		
		//slow overwrite
		const T& addOneAtReserved(const IdxT& atIndex, const T& value) {
			
			//using old count
			ManagerT::moveElements((count - atIndex), el + atIndex, el + atIndex + 1);

			++count;
			el[atIndex] = value;
			return value;
		}

		T& addOneReserved() {

			return el[count++];
		}

		const T& addOneReserved(const T& value) {

			el[count++] = value;
			return value;
		}

		void remove(IdxT num = 1) {

			if (num == ((IdxT) - 1)) {
				num = count;
			}

			//WE::assertSafeOp<IdxT>(count, -num);

			count -= num;
			ManagerT::destroyElements(el + count, num);
		};

		void removeOne() {

			WE::assertSafeOp<IdxT>(count, -1);

			count -= 1;
			ManagerT::destroyElements(el + count, 1);
		};

		/*  ****Slow Overwrite**** */
		void removeIndex(IdxT index) {

			WE::assertSafeOp<IdxT>(count, -1);

			if (index >= count) {

				return;
			}

			ManagerT::destroyElement(el[index]);
			ManagerT::moveElements((count - index) - 1, el + index + 1, el + index);

			count--;
		}

		void removeSwapWithLast(IdxT index, bool destroyRemoved) {

			if (destroyRemoved) {

				ManagerT::destroyElement(el[index]);
			}

			if (count > 1) {

				el[index] = el[--count];
			} else {

				--count;
			}
		}

		void removeSwapWithLast(IdxT index, IdxT& lastIndexRef, bool destroyRemoved) {

			assert (index != count - 1);
				
			if (destroyRemoved) {

				ManagerT::destroyElement(el[index]);
			}

			lastIndexRef = index;
			el[index] = el[--count];
		}
		
		bool searchRemoveSwapWithLast(const T& search, bool destroyRemoved, const IdxT& from, const IdxT& count) {

			IdxT index;

			if (searchFor(search, from, count, index)) {

				removeSwapWithLast(index, destroyRemoved);

				return true;
			}

			return false;
		}

		inline bool searchRemoveSwapWithLast(const T& search, bool destroyRemoved = true) {

			return searchRemoveSwapWithLast(search, destroyRemoved, 0, count);
		}
	};

	template <class T, bool zeroNew, class IdxT, class ResizeT = ResizeDouble> 
	class CountedArray : public CountedArrayT<T, zeroNew, IdxT, ObjectManager<T, IdxT>, ResizeT > {
	};

	template <class PtrT, bool zeroNew, class IdxT, class ResizeT = ResizeDouble> 
	class CountedPtrArray : public CountedArrayT<PtrT, zeroNew, IdxT, ObjectManagerPtr<PtrT, IdxT>, ResizeT > {
	};

	template <class T, class IdxT, class ResizeT = ResizeDouble, bool PtrReuse = false> 
	class CountedPtrArrayEx : public CountedPtrArray<T*, PtrReuse ? true : false, IdxT, ResizeT> {
	public:

		typedef T* PtrT;

	public:

		T*& addNewOne() {

			T*& pEl = addOne();

			if(PtrReuse) {

				if (!pEl)
					WE_MMemNew(pEl, T());

			} else {

				WE_MMemNew(pEl, T());
			}
			
			return pEl;
		}
	};

	template <class RefT, bool zeroNew, class IdxT, class ResizeT = ResizeDouble> 
	class CountedRefArray : public CountedArrayT<RefT, zeroNew, IdxT, ObjectManagerRef<RefT, IdxT>, ResizeT > {
	};



	
	template <class T, class IdxT> 
	struct LinkedArray : LinkArray<IdxT>, CountedArrayBase<T, IdxT> {

		typedef IdxT Index;
		//Xstatic const IdxT IndexNull = ((IdxT) -1);
	};

	

	template <class T, bool zeroNew, class IdxT, class ManagerT, class ResizeT> 
	class VectorT : public LinkedArray<T, IdxT> {
	protected:
		
		typedef Link<IdxT> Link;

		void link(IdxT newLink) {

			if (_first == IdxTNull) {
				_first = newLink;
			}

			if (_last != IdxTNull) {
				pLinks[_last].next = newLink;
			}

			pLinks[newLink].previous = _last;
			pLinks[newLink].next = IdxTNull;

			_last = newLink;
		}

		void unlink(IdxT oldLink) {

			if (_first == oldLink) {
				_first = pLinks[oldLink].next;
			}

			if (_last == oldLink) {
				_last = pLinks[oldLink].previous;
			}

			if (pLinks[oldLink].previous != IdxTNull) {
				pLinks[pLinks[oldLink].previous].next = pLinks[oldLink].next;
			}

			if (pLinks[oldLink].next != IdxTNull) {
				pLinks[pLinks[oldLink].next].previous = pLinks[oldLink].previous;
			}
		}

		IdxT addLink() {

			WE::assertSafeOp<IdxT>(count, +1);

			IdxT newLink = pLinks[count].freeLink;
			link(newLink);

			count++;
			return newLink;
		}

		void removeLink(IdxT oldLink) {

			WE::assertSafeOp<IdxT>(count, -1);

			if (oldLink == IdxTNull) {
				return;
			}

			count--;
			pLinks[count].freeLink = oldLink;
					
			unlink(oldLink);
		}

	public:

		VectorT() {
		}

		~VectorT() {
			destroy();
		}

		IdxT indexOf(T* arrayElement) {
			return arrayElement - el;
		}
		

		void reserveCount(IdxT freeCount) {
			
			IdxT free = size - count;

			if (freeCount > free) {

				WE::assertSafeOp<IdxT>(count, +freeCount);

				reserve(count + freeCount);
			}
		}

		void reserve(IdxT newSize) {

			if (newSize == 0 || newSize <= size) {
				return;
			}

			T* pNewArray;
			WE_MMemNew(pNewArray, T[newSize]);
			ManagerT::moveElements(count, el, pNewArray);
			if (zeroNew) {
				memset(pNewArray + size, 0, (newSize - size) * sizeof(T));
			}
			WE_MMemDeleteArray(el);
			el = pNewArray;

			Link* pNewLinks;
			WE_MMemNew(pNewLinks, Link[newSize]);
			memcpy(pNewLinks, pLinks, size * sizeof(Link));
			for (IdxT i = size; i < newSize; i++) {
				pNewLinks[i].freeLink = i;
			}
			WE_MMemDeleteArray(pLinks);
			pLinks = pNewLinks;

			size = newSize;
		}

		void destroy() {

			if (size) {

				ManagerT::destroyElements(el, count);
				WE_MMemDeleteArray(el);
				WE_MMemDeleteArray(pLinks);
				size = 0;
				count = 0;
			}
		}

		IdxT add(IdxT num = 1, bool autoReserve = true) {

			WE::assertSafeOp<IdxT>(count, +num);

			if(count + num > size) {

				if (autoReserve) {

					unsigned long newSize = ResizeT::resize(size, count + num);
					WE::assrt(newSize == (unsigned long) ((IdxT) newSize));
					reserve(newSize);

				} else {
					return IdxTNull;
				}
			}

			if (num == 0) {
				return IdxTNull;
			}
			
			IdxT firstIndex = addLink();
			num--;
			while(num) {
				addLink();
				num--;
			}
			
			return firstIndex;
		};

		IdxT addOneIndex() {

			WE::assertSafeOp<IdxT>(count, +1);

			if(count + 1 > size) {

				unsigned long newSize = ResizeT::resize(size, size + 1);
				WE::assrt(newSize == (unsigned long) ((IdxT) newSize));
				reserve((IdxT) newSize);

			}

			return addLink();
		};

		IdxT addOneIndex(const T& val) {

			WE::assertSafeOp<IdxT>(count, +1);

			if(count + 1 > size) {
				
				unsigned long newSize = ResizeT::resize(size, size + 1);
				WE::assrt(newSize == (unsigned long) ((IdxT) newSize));
				reserve((IdxT) newSize);
			}

			IdxT index = addLink();
			el[index] = val;

			return index;
		};

				
		inline void removeIndex(IdxT index) {

			removeLink(index);
		}

	};

	template <class T, bool zeroNew, class IdxT, class ResizeT = ResizeDouble> 
	class Vector : public VectorT<T, zeroNew, IdxT, ObjectManager<T, IdxT>, ResizeT > {
	};

	template <class PtrT, bool zeroNew, class IdxT, class ResizeT = ResizeDouble> 
	class PtrVector : public VectorT<PtrT, zeroNew, IdxT, ObjectManagerPtr<PtrT, IdxT>, ResizeT > {
	};

	template <class RefT, bool zeroNew, class IdxT, class ResizeT = ResizeDouble> 
	class RefVector : public VectorT<RefT, zeroNew, IdxT, ObjectManagerRef<RefT, IdxT>, ResizeT > {
	};

	
}

#endif