#ifndef _WEStringKey_h
#define _WEStringKey_h

#include "WEString.h"
#include "WEStringPart.h"
#include "../WETL/WETLDataTypes.h"

namespace WE {

	class StringKey {
	protected:

		String mString;
		StringHash mHash;

	public:

		StringKey();
		StringKey(const TCHAR* key);
		StringKey(const String& key);
		StringKey(const TCHAR* str, const StringPartBase& key);
		StringKey(const StringPart& key);

		void assign(const TCHAR* key);
		void assign(const String& key);
		void assign(const TCHAR* str, const StringPartBase& key);
		void assign(const StringPart& key);

		~StringKey();

		const String& getString();

		operator WETL::Index::Type () const;
		operator WETL::IndexShort::Type () const;
		bool operator==(const StringKey& comp) const;
		StringKey& operator=(const StringKey& ref);
	};

}

#endif