#ifndef _WEStringPart_h
#define _WEStringPart_h

#include "WEString.h"

namespace WE {

	struct StringPartBase {

		int startIndex;
		int count;

		StringPartBase();

		void destroy();

		int lastIndex() const;

		int length() const;
		StringHash hash(const TCHAR* buff) const;

		void assignTo(const TCHAR* buff, String& string) const;
		void appendTo(const TCHAR* buff, String& string) const;

		bool equals(const TCHAR* buff, const TCHAR* comp, int len = 0) const;

		void getRemainingPart(const TCHAR* buff, StringPartBase& remain, int len = 0) const;
		void getFirstPart(const TCHAR* buff, StringPartBase& first) const;
	};

	struct StringPart : StringPartBase {

		const String* pRef;
		
		StringPart(const String* pRef);

		void destroy();

		int length() const;
		StringHash hash() const;

		void assignTo(String& string) const;
		void appendTo(String& string) const;

		bool equals(const TCHAR* comp, int len = 0) const;
	};

}


#endif