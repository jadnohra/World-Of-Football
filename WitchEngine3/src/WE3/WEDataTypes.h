#ifndef _WEDataTypes_h
#define _WEDataTypes_h

/*
#ifndef UBYTE
#define UBYTE unsigned char
#endif
*/


#ifdef WIN32

	#include "binding/win/includeWindows.h"

#else

	#ifndef BYTE
	#define BYTE unsigned char
	#endif

	#define BYTE_MAX ((BYTE) -1)

	#ifndef DWORD
	#define DWORD unsigned long
	#endif

	#define DWORD_MAX ((DWORD) -1)

	#ifndef UINT
	#define UINT unsigned int
	#endif

	#define UINT_MAX ((UINT) -1)

	#ifndef USHORT
	#define USHORT unsigned short
	#endif

	#define USHORT_MAX ((USHORT) -1)

#endif

typedef BYTE UBYTE;

#define USHORT_MAX ((unsigned short) -1)

#define UINT16 unsigned __int16
#define UINT16_MAX ((unsigned __int16) -1)

#define UINT32 unsigned __int32
#define UINT32_MAX ((unsigned __int32) -1)

#define UHALFINT UINT16


#include "WEMacros.h"

namespace WE {

	template<class T>
	inline T NullIndex() {

		return ((T) -1);
	};

	template<class T>
	struct IndexT {

		typedef T Type;
		static const T Null = ((T) -1);

		T value;

		static bool isValidIndex(unsigned long index) {

			return ((unsigned long (Null)) > index);
		}

		static bool isValid(const T& index) {

			return (index != Null);
		}

		static bool isNull(const T& index) {

			return (index == Null);
		}

		IndexT() {
		};

		IndexT(const T& val) : value(val) {
		};

		inline T& operator++() {

			safeOp<T>(value, +1);

			return (value);
		};

		inline void setNull() {

			value = Null;
		}

		inline bool isNull() {

			return (value == Null);
		}

		inline bool isValid() {

			return (value != Null);
		}

		inline operator T& () {

			return value;
		}
	};
	


	typedef IndexT<UINT> Index;
	typedef IndexT<USHORT> IndexShort;
	typedef IndexT<BYTE> IndexByte;
	
}



#endif