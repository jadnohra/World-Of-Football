#ifndef _WEMem_h
#define _WEMem_h

#include "WEAssert.h"
#include "memory.h"

namespace WE {

void _memCheckSetAddress(void* address);
void _memCheck(void* address);

#ifdef _DEBUG
	#define MMemNew(ptr, ctor) ptr = new ctor; WE::_memCheck(ptr)
	#define MMemAlloc(ptr, bytes, typeCast) ptr = (typeCast) malloc(bytes), WE::_memCheck(ptr)
#else
	#define MMemNew(ptr, ctor) ptr = new ctor; 
	#define MMemAlloc(ptr, bytes, typeCast) ptr = (typeCast) malloc(bytes)
#endif


#define MMemAllocVoid(ptr, bytes) MMemAlloc(ptr, bytes, void*)

#ifdef _DEBUG

	#define MMemFree(ptr) if (ptr) {WE::_memCheck(ptr); free(ptr); (ptr) = NULL;}
	#define MMemDelete(ptr) if (ptr) {WE::_memCheck(ptr); delete (ptr); (ptr) = NULL;}
	#define MMemDeleteArray(ptr) if (ptr) {WE::_memCheck(ptr); delete[] (ptr); (ptr) = NULL;}
	#define MMemCtDelete(ptr) if (ptr) {WE::_memCheck(ptr); delete (ptr); }
	#define MMemCtDeleteArray(ptr) if (ptr) {WE::_memCheck(ptr); delete[] (ptr); }

#else
	#define MMemFree(ptr) if (ptr) {free(ptr); (ptr) = NULL;}
	#define MMemDelete(ptr) if (ptr) {delete (ptr); (ptr) = NULL;}
	#define MMemDeleteArray(ptr) if (ptr) {delete[] (ptr); (ptr) = NULL;}
	#define MMemCtDelete(ptr) if (ptr) { delete (ptr); }
	#define MMemCtDeleteArray(ptr) if (ptr) { delete[] (ptr); }
#endif


#define MMemFreeVoid(ptr) MMemFree(ptr)

}

#define WE_MMemNew MMemNew
#define WE_MMemAlloc MMemAlloc

/*
#ifdef _DEBUG
	#define WE_MMemNew(ptr, ctor) ptr = new ctor; WE::_memCheck(ptr)
	#define WE_MMemAlloc(ptr, bytes, typeCast) ptr = (typeCast) malloc(bytes), WE::_memCheck(ptr)
#else
	#define WE_MMemNew(ptr, ctor) ptr = new ctor; 
	#define WE_MMemAlloc(ptr, bytes, typeCast) ptr = (typeCast) malloc(bytes)
#endif
	*/

#define WE_MMemAllocVoid MMemAllocVoid

#define WE_MMemFree MMemFree
#define WE_MMemFreeVoid MMemFreeVoid
#define WE_MMemDelete MMemDelete
#define WE_MMemDeleteArray MMemDeleteArray

#endif