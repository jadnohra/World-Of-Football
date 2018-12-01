#ifndef _WEWinVirtualKeyCodeMap_h
#define _WEWinVirtualKeyCodeMap_h
#ifdef WIN32

#include "includeWindows.h"
#include "../../WETL/WETLHashMap.h"
#include "../../string/WEString.h"

namespace WE {

	class WinVirtualKeyCodeMap {
	public:

		static bool getKeyCode(const TCHAR* string, int& code);
		
		static unsigned int getKeyCodeCount();
		static const TCHAR* getKeyCodeAt(const unsigned int& index, int& code);

	protected:

		static void initKeyCodeMap();

	protected:

		struct KeyCode {

			KeyCode() {}
			KeyCode(const TCHAR* _name, int _code) : name(_name), code(_code) {}

			const TCHAR* name;
			int code;
		};

		struct KeyCodeMap : public WETL::HashMap<StringHash, KeyCode, unsigned int, WETL::ResizeExact, 255> {

			void put(const TCHAR* name, const int& code) {

				KeyCode keyCode(name, code);

				insert(String::hash(name), keyCode);
			}
		};

		static KeyCodeMap mKeyCodeMap;
	};

} 

#endif
#endif