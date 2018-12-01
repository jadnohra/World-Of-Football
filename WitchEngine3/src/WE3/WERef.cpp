#include "WERef.h"

#include "WEAssert.h"
#include "WEMem.h"

namespace WE {

RefBase::RefBase(int refCount) : mRefCount(refCount) {
}

int RefBase::AddRef() {

	return (++mRefCount);
}

int RefBase::GetRefCount() {

	return mRefCount;
}

int RefBase::RemoveRef() {

	assrt(mRefCount != 0);

	return (--mRefCount);
}



Ref::Ref(int refCount) : RefBase(refCount) {
}

int Ref::Release() {

	if ((void*) this == (void*) 0x014cdfb0)
		int ok = true;

	assrt(mRefCount != 0);

	if ((--mRefCount) <= 0) {

		MMemCtDelete(this);
		return 0;
	}

	return mRefCount;
}

Ref::~Ref() {
}

}