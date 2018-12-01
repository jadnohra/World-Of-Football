#ifndef _WEPtr_h
#define _WEPtr_h

#include "WEMem.h"
#include "WEAssert.h"

namespace WE {

	enum PtrType {

		Ptr_SoftRef = 0, Ptr_HardRef, Ptr_HardRefArray
	};

	template <class T>
	struct Ptr {

		T* pObject;

		Ptr(T* _pObject = NULL) {

			pObject = _pObject;
		}
	

		~Ptr() {

			assrt(pObject == NULL);
		}

		void destroy(PtrType type) {

			switch (type) {
				case Ptr_HardRef:
					MMemDelete(pObject);
					break;
				case Ptr_HardRefArray:
					MMemDeleteArray(pObject);
					break;
				default:
					pObject = NULL;
					break;
			}
		}

		void conditionalDestroy(bool doDestroy, PtrType type) {

			if (doDestroy) {

				destroy(type);
			}
		}

		void setNoDestroy(T* _pObject) {

			pObject = _pObject;
		}

		inline T& dref() {

			assrt(pObject != NULL);

			return *pObject;
		}

		inline T* ptr() const {

			return pObject;
		}

		inline operator T& () {

			return dref();
		}

		inline operator T* () {

			return ptr();
		}

		inline T*& ptrRef() {

			return pObject;
		}


		inline T* operator->() {

			assrt(pObject != NULL);

			return pObject;
		}

		inline const T* operator->() const {

			assrt(pObject != NULL);

			return pObject;
		}

		inline bool isNull() {

			return (pObject == NULL);
		}

		inline bool isValid() const {

			return (pObject != NULL);
		}
	};

	template<class T, PtrType typeT>
	struct TypedPtr : Ptr<T> {

		TypedPtr(T* _pObject = NULL) : Ptr<T>(_pObject){
		}

		~TypedPtr() {

			destroy();
		}

		inline void destroy() {

			Ptr<T>::destroy(typeT);
		}

		inline void conditionalDestroy(bool doDestroy) {

			Ptr<T>::conditionalDestroy(doDestroy, typeT);
		}

		void set(T* _pObject) {

			Ptr<T>::destroy(typeT);

			pObject = _pObject;
		}

		inline T* operator=(T* pObj) {
		
			set(pObj);
			return pObj;
		};
	};

	template<class T>
	struct HardPtr : TypedPtr<T, Ptr_HardRef> {

		HardPtr(T* _pObject = NULL) : TypedPtr(_pObject){
		}

		inline T* operator=(T* pObj) {
		
			set(pObj);
			return pObj;
		};

		HardPtr(const HardPtr<T>& ref) : TypedPtr(ref.pObject) {
		}

		inline HardPtr<T>& operator=(const HardPtr<T>& ref) {
		
			set(ref.pObject);
			return *this;
		};
	};

	template<class T>
	struct TransparentPtr {

		T object;

		inline T* operator->() { return &object; }
	};

	template<class T>
	struct SoftPtr : TypedPtr<T, Ptr_SoftRef> {

		SoftPtr(T* _pObject = NULL) : TypedPtr(_pObject){
		}

		inline T* operator=(T* pObj) {
		
			set(pObj);
			return pObj;
		};
	};

	template<class T>
	struct HardArrayPtr : TypedPtr<T, Ptr_HardRefArray> {

		HardArrayPtr(T* _pObject = NULL) : TypedPtr(_pObject){
		}

		inline T* operator=(T* pObj) {
		
			set(pObj);
			return pObj;
		};

		HardArrayPtr(const HardArrayPtr<T>& ref) : TypedPtr(ref.pObject) {
		}

		inline HardArrayPtr<T>& operator=(const HardArrayPtr<T>& ref) {
		
			set(ref.ptr());
			return *this;
		};
	};

	template<class T>
	struct FlexiblePtr : Ptr<T> {

		PtrType type;

		FlexiblePtr(T* _pObject = NULL, PtrType _type = Ptr_SoftRef) : Ptr<T>(_pObject){

			type = _type;
		}

		~FlexiblePtr() {

			destroy();
		}

		inline void destroy() {

			Ptr<T>::destroy(type);
		}

		inline void conditionalDestroy(bool doDestroy) {

			Ptr<T>::conditionalDestroy(doDestroy, type);
		}

		void setType(PtrType _type, bool destroyObject = true) {

			if (destroyObject && pObject) {

				destroy();
			}

			type = _type;
		}


		void set(T* _pObject) {

			Ptr<T>::destroy(type);

			pObject = _pObject;
		}

		void set(T* _pObject, PtrType _type) {

			destroy();
			type = _type;
			pObject = _pObject;
		}

		inline T* operator=(T* pObj) {
		
			set(pObj);
			return pObj;
		}

		FlexiblePtr(const FlexiblePtr<T>& ref) : FlexiblePtr(ref.pObject) {
		}

		inline FlexiblePtr<T>& operator=(const FlexiblePtr<T>& ref) {
		
			set(ref.ptr());
			return *this;
		};
		
	};

}

#endif