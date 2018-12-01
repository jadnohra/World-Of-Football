#ifndef _SourceHolder_h_WE_Input
#define _SourceHolder_h_WE_Input

#include "Source.h"

namespace WE { namespace Input {
	

	class SourceHolder : public HardRef<Source> {
	public:
		
		SourceHolder(Source* pObj = NULL) : HardRef(pObj) {}

		inline bool is(Source* pVar) { return pVar == ptr(); }

	public:

		//ensure correct copying
		SourceHolder(const SourceHolder& ref) : HardRef(ref.ptr()) {}
		inline SourceHolder& operator=(const SourceHolder& ref) { set(ref.ptr()); return *this; }
	};
	

} }

#endif