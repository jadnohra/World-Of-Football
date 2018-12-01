#ifndef _WERef_h
#define _WERef_h

#include "WEMacros.h"

namespace WE {

	class RefBase {
	protected:
		int mRefCount;

	public:
		RefBase(int refCount = 1);


		int AddRef();
		int RemoveRef(); 

		int GetRefCount();
	};

	class Ref : public RefBase {
	public:
		Ref(int refCount = 1);

		int Release(); 

		virtual ~Ref();
	};

	class VRef {
	public:

		virtual ~VRef() {}
		virtual int Release() = 0;
		virtual void AddRef() = 0;
	};


	template <class T>
	struct RefWrap {

		T* pObject;

		RefWrap() {

			pObject = NULL;
		}

		RefWrap(T* _pObject, bool addRef) {

			pObject = _pObject;
			if (addRef) MSafeAddRef(pObject);
		}

		~RefWrap() {

			MSafeRelease(pObject);
		}

		void destroy() {

			MSafeRelease(pObject);
		}

		bool set(T* pRef, bool addRef) {

			MSafeRelease(pObject);
			pObject = pRef;

			if (addRef) MSafeAddRef(pObject);

			return (pObject != NULL);
		}

		inline T& dref() const {

			assrt(pObject != NULL);
			return *pObject;
		}

		T* bumpedPtr() {

			if (pObject) {
				pObject->AddRef();
			}

			return pObject;
		}

		inline T* ptr() const {

			return pObject;
		}

		inline T*& ptrRef() {

			return pObject;
		}

		inline T** pptr() {

			return &pObject;
		}

		inline operator T& () {

			return dref();
		}

		inline operator T* () {

			return ptr();
		}

		inline T* operator->() {

			assrt(pObject != NULL);
			return pObject;
		}

		inline bool isNull() const {

			return (pObject == NULL);
		}

		inline bool isValid() const {

			return (pObject != NULL);
		}
	};

	//RefWrap with addRef always true
	//u can then use operator= it works correctly
	template<class T, bool HardT> 
	struct AutoRefWrap : RefWrap<T> {

		AutoRefWrap() : RefWrap<T>() {
		}

		AutoRefWrap(T* _pObject) : RefWrap<T>(_pObject, HardT) {
		}

		bool set(T* pRef) {

			return RefWrap<T>::set(pRef, HardT);
		}

		inline AutoRefWrap<T, HardT>& operator=(const AutoRefWrap<T, HardT>& ref) {
		
			set(ref.ptr());

			if (!HardT)
				MSafeAddRef(pObject);

			return *this;
		};
	};

	template<class T>
	struct HardRef : AutoRefWrap<T, true> {

		HardRef(T* _pObject = NULL) : AutoRefWrap(_pObject) {
		}

		inline bool operator=(T* pRef) {

			return set(pRef);
		};

		HardRef(const HardRef<T>& ref) : AutoRefWrap(ref.pObject) {
		}

		inline HardRef<T>& operator=(const HardRef<T>& ref) {
		
			set(ref.ptr());
			return *this;
		};
	};

	template<class T>
	struct SoftRef : AutoRefWrap<T, false> {

		SoftRef(T* _pObject = NULL) : AutoRefWrap(_pObject) {
		}

		inline bool operator=(T* pRef) {

			return set(pRef);
		};

		SoftRef(const SoftRef<T>& ref) : SoftRef(ref.pObject) {

			MSafeAddRef(pObject);
		}

		inline SoftRef<T>& operator=(const SoftRef<T>& ref) {
		
			set(ref.ptr());
			MSafeAddRef(pObject);

			return *this;
		};

	};
}

#endif