#ifndef _WEDataTranslator_h
#define _WEDataTranslator_h

#include "../string/WEBufferString.h"
#include "../WERef.h"

namespace WE {

	class DataTranslator : public Ref {
	public:

		virtual bool inject(const TCHAR* variable, const TCHAR* value) = 0;

		virtual void translate(const TCHAR* source, BufferString& dest) = 0;
		virtual void translate(BufferString& inPlace) = 0;

	public:

		static void dummyTranslate(const TCHAR* source, BufferString& dest);
		static void dummyTranslate(BufferString& inPlace);
	};

}

#endif