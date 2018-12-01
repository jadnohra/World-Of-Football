#ifndef _WETLSizeAlloc_h
#define _WETLSizeAlloc_h

#include "../WEMem.h"
#include "../WEMacros.h"
#include "../WEAssert.h"

#include "WETLArrayBase.h"

namespace WETL {

	
	template <class T, class IdxT, bool exactSize = false> 
	class SizeAllocT : public ArrayBase<T, IdxT> {
	public:

		SizeAllocT() {
		}

		~SizeAllocT() {

			destroy();
		}

		void memCloneTo(SizeAllocT<T, IdxT, false>& copy) const {

			copy.resize(size);
			memcpy(copy.el, el, size * sizeof(T));
		}

		void memCloneTo(SizeAllocT<T, IdxT, true>& copy) const {

			copy.resize(size);
			memcpy(copy.el, el, size * sizeof(T));
		}

		void memSet(int val) {

			memset(el, val, size * sizeof(T));
		}

		IdxT indexOf(T* arrayElement) {

			return (arrayElement - el) / sizeof(T);
		}
				

		void resize(IdxT newSize) {

			if (exactSize) {

				if (newSize == 0) {

					destroy();
					return;

				} else if (newSize == size) {
					
					return;
				}

			} else {

				if (newSize <= size) {

					size = newSize;

					return;
				}
			}

			el = (T*) realloc(el, newSize * sizeof(T));

			size = newSize;
		}

		void destroy() {

			if (size) {

				WE_MMemFree(el);
				size = 0;
			}
		}

	};


	template <class StrideT, class IdxT> 
	struct MemAllocBase : ArrayBase<char, IdxT> {

		StrideT stride;

		MemAllocBase() {

			stride = 0;
		}
	};


	template <class StrideT, class IdxT, bool exactSize = false> 
	class MemAllocT : public MemAllocBase<StrideT, IdxT> {
	public:

		MemAllocT() {

			WE::assrt(sizeof(char) == 1);
		}

		~MemAllocT() {

			destroy();
		}

		void memCloneTo(MemAllocT<StrideT, IdxT, false>& copy) const {

			copy.setStride(stride, false);
			copy.resize(size);
			memcpy(copy.el, el, size);
		}

		void memCloneTo(MemAllocT<StrideT, IdxT, true>& copy) const {

			copy.setStride(stride, false);
			copy.resize(size);
			memcpy(copy.el, el, size);
		}

		void memSet(int val) {

			memset(el, val, size);
		}

		void setStride(const StrideT& _stride, bool autoResizeBuffer) {

			if (autoResizeBuffer) {

				if (stride != _stride) {

					IdxT newSizeBytes;
					
					if (toSizeBytesCheck(elementCount(), newSizeBytes, _stride) == false) {

						WE::assrt(false);
					} else {
					
						resize(newSizeBytes);
						stride = _stride;
					}
				}

			} else {

				if (el != NULL) {

					WE::assrt(false);
				} else {

					stride = _stride;
				}
			}
			
		}

		inline const StrideT& getStride() const {

			return stride;
		}
		

		IdxT indexOf(void* arrayElement) const {

			return (arrayElement - el) / stride;
		}

		IdxT toSizeBytes(IdxT elementCount) const {

			return (elementCount) * stride;
		}

		bool toSizeBytesCheck(IdxT elementCount, IdxT& sizeBytes) const {

			return toSizeBytesCheck(elementCount, sizeBytes, stride);
		}

		static bool toSizeBytesCheck(IdxT elementCount, IdxT& sizeBytes, const StrideT& stride) {

			sizeBytes = elementCount;

			return WE::checkMul<IdxT>(sizeBytes, stride);
		}
		
		IdxT toElementCount(IdxT byteSize) const {

			if (stride == 0) {

				WE::assrt(byteSize == 0);
				return 0;
			}

			return (byteSize) / stride;
		}

		inline IdxT elementCount() const {

			return toElementCount(size);
		}
				
		void setElementCount(IdxT newElementCount) {

			resize(newElementCount * stride);
		}

		void resize(IdxT newSizeBytes) {

			if (exactSize) {

				if (newSizeBytes == 0) {

					destroy();
					return;

				} else if (newSizeBytes == size) {
					
					return;
				}

			} else {

				if (newSizeBytes <= size) {

					return;
				}
			}

			el = (char*) realloc(el, newSizeBytes);

			size = newSizeBytes;
		}

		void destroy() {

			if (size) {

				WE_MMemFree(el);
				size = 0;
			}
		}

	};

}

#endif