#ifndef _WEString_h
#define _WEString_h

#include "../binding/win/includeWindows.h"
#include "../WEPtr.h"
#include "wchar.h"

/*

	String Lengths mess,
	for a String class, mSize includes NULL
	for TCHAR/char buffers len doesnt include NULL unless otherwize specified (as in findNext)

*/


namespace WE {

	struct StringPart;

	typedef unsigned long StringHash;
	static const StringHash StringHashNull = ((StringHash) 0);

	class String {
	public:

		//bool mIsRef;

		int mSize; //including \0
		
		union {

			struct {
				TCHAR* mBuffer;
			};
			/*
			struct {
				const TCHAR* mBufferRef;
			};
			*/
		};
		
	public:

		String();
		~String();

		String(const TCHAR* string);
		String(const String& string);
/*
		String(const StringPart& string);
*/
/*
#ifdef  UNICODE
		String(const char* string);
#endif
*/

		TCHAR* setSize(int len); //including /0

		int length() const;
		bool isEmpty() const;
		bool isValid() const;
		static int length(const TCHAR* string, bool countNull);

		void replace(TCHAR c, TCHAR replacement);

		int findNext(TCHAR c, int start, String& part, bool includeChar) const;
		int findNext(TCHAR c, int start, bool& found, int& startIndex, int& count, bool includeChar) const;
		int findNext(String& key, int start) const;
		int findNextIgnoreCase(String& key, int start) const;
		
		static int findNext(const TCHAR* buffer, TCHAR c, int start, bool& found, int& startIndex, int& count, bool includeChar, int& lenIncNull);
		static int findNext(const TCHAR* buffer, TCHAR c, int start, bool& found, int& startIndex, int& lenIncNull);

		//use start = -2 to start from the end of the string
		int findPrevious(TCHAR c, int start, bool& found, int& startIndex, int& count, bool includeChar) const;
		
		static int count(const TCHAR* buffer, TCHAR c);
		int count(TCHAR c) const;

		void destroy();

		//void unref(); //creates own buffer copy if isRef

		void assign(const String& string, int count = 0);
		//void assign(const StringPart& string);
		void assign(const TCHAR* string, int count = 0);
/*
#ifdef UNICODE
		void assign(const char* string, int count = 0);
#endif
*/
		/*
		void assignRef(const String& string, int count = 0);
		void assignRef(const StringPart& string);
		void assignRef(const TCHAR* string, int count = 0);
#ifdef UNICODE
		void assignRef(const char* string, int count = 0);
#endif
		*/

		void assignEx(const TCHAR *format, ...);
		void assignEx(const TCHAR *format, va_list& ap);

		void append(const String& string, int count = 0);
		//void append(StringPart& string);
		void append(const TCHAR* string, int count = 0);

		//inserts at beginning
		void insert(const TCHAR* string, int count = 0);
		void insert(const String& string, int count = 0);

/*
#ifdef UNICODE
		void append(const char* string, int count = 0);
#endif
*/

		const TCHAR* c_tstr() const;

/*
#ifdef UNICODE
		void toCharPtr(Ptr<char>& charPtr) const;
		static void toCharPtr(Ptr<char>& charPtr, const TCHAR* buff, int len = 0);
#endif
*/

		bool equals(const String& comp) const;
		bool equalsIgnoreCase(const String& comp) const;

		bool equals(const TCHAR* comp) const;
		bool equalsIgnoreCase(const TCHAR* comp) const;
		
		StringHash hash() const;
		void hash(unsigned int& hash) const;

		static StringHash hash(const TCHAR* string, int len = 0);
		static void hash(unsigned int& hash, const TCHAR* string, int len = 0);
#ifdef UNICODE
		static StringHash hash(const char* string, int len = 0);
		static void hash(unsigned int& hash, const char* string, int len = 0);
#endif

#ifdef _DEBUG
		static void debug(const TCHAR *format, ...);
#else
		inline static void debug(const TCHAR *format, ...) {};
#endif

		bool fromString(bool& convert);
		bool fromString(const TCHAR* format, void* pConvert);

		static bool fromString(const TCHAR* string, bool& convert);
		static bool fromString(const TCHAR* string, const TCHAR* format, void* pConvert);
/*
#ifdef UNICODE
		static bool fromString(const char* string, bool& convert);
#endif
*/
	};


	//typedef StringHash StringKey;

	/*
	class StringRef : String {
	public:

		StringRef(const TCHAR* string);
		StringRef(const String& ref);
		StringRef(const StringPart& string);

		#ifdef  UNICODE
			StringRef(const char* string);
		#endif
	};
	*/

	/*
	struct Path {

		String completeFilePath;
		StringPart path;
		StringPart file;

		Path() {};
		Path(const TCHAR* string);
		~Path();

		void assign(const TCHAR* string);

		inline const TCHAR* c_tstr() { return completeFilePath.c_tstr();}
	};
	*/

	/*
	struct StringKey {
		ULONG hash;

		StringKey();
		StringKey(const StringKey& ref);
		StringKey(const TCHAR* string);
#ifdef UNICODE
		StringKey(const char* string);
#endif
		StringKey(const String& ref);
		

		void operator=(const StringKey& ref);
		bool operator<(const StringKey& comp) const;
	};
	*/


}

template<class T>
T merge_StringHash(const T& hash1, const T& hash2) {
	
	return (hash1 << 6) + (hash2 << 16) - (hash1 + hash2);
}



#endif