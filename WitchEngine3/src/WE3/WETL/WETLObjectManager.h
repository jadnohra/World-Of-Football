#ifndef _WETLObjectManager_h
#define _WETLObjectManager_h

#include "../WEMem.h"
#include "../WEMacros.h"

namespace WETL {

	template <class T, class IdxT> 
	struct ObjectManager {
		
		static void destroyElement(T& element) {
		}

		static void destroyElements(T* pArray, IdxT count) {
		}

		static void moveElements(IdxT count, T* pSrcArray, T* pDestArray) {

			memcpy(pDestArray, pSrcArray, count * sizeof(T));
		}

		inline static void onAdded(T& element) {
		}

		inline static void onRemoved(T& element/*, T& passBackElement*/) {

			//passBackElement = element;
		}

		inline static void onFound(T& element) {
		}
	};

	template <class T, class IdxT> 
	struct ObjectManagerPtr {
		
		static void destroyElement(T& element) {

			MMemDelete(element);
		}

		static void destroyElements(T* pArray, const IdxT& count) {

			for (IdxT i = 0; i < count; i++) {

				MMemDelete(pArray[i]);
			}
		}

		static void moveElements(IdxT count, T* pSrcArray, T* pDestArray) {

			memcpy(pDestArray, pSrcArray, count * sizeof(T));
		}

		inline static void onAdded(T& element) {
		}

		inline static void onRemoved(T& element/*, T& passBackElement*/) {

			//passBackElement = element;
			element = NULL;
		}

		inline static void onFound(T& element) {
		}

	};

	template <class T, class IdxT> 
	struct ObjectManagerRef {
		
		static void destroyElement(T& element) {

			MSafeRelease(element);
		}

		static void destroyElements(T* pArray, IdxT count) {

			for (IdxT i = 0; i < count; i++) {

				MSafeRelease(pArray[i]);
			}
		}

		static void moveElements(IdxT count, T* pSrcArray, T* pDestArray) {

			memcpy(pDestArray, pSrcArray, count * sizeof(T));
		}

		inline static void onAdded(T& element) {

			MSafeAddRef(element);
		}

		inline static void onRemoved(T& element/*, T& passBackElement*/) {

			MSafeRelease(element);
			//passBackElement = element;
		}

		inline static void onFound(T& element) {

			MSafeAddRef(element);
		}

	};

	/*
	template <class T, class IdxT> 
	struct ObjectManagerCopiable {
		
		static void destroyArrayElement(T& element) {
		}

		static void destroyArrayElements(T* pArray, IdxT count) {
		}

		static void moveArrayElements(IdxT count, T* pSrcArray, T* pDestArray) {

			for(IdxType i = 0; i < count; i++) {
					pDestArray[i] = pSrcArray[i];
				}
		}

	};
	*/

}

#endif