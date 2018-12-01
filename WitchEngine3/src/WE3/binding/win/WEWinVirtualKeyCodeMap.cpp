#include "WEWinVirtualKeyCodeMap.h"
#ifdef WIN32

namespace WE {

WinVirtualKeyCodeMap::KeyCodeMap WinVirtualKeyCodeMap::mKeyCodeMap;

bool WinVirtualKeyCodeMap::getKeyCode(const TCHAR* string, int& code) {

	initKeyCodeMap();

	KeyCode keyCode;

	if (mKeyCodeMap.find(String::hash(string), keyCode)) {

		code = keyCode.code;

		return true;
	}

	return false;;
}

unsigned int WinVirtualKeyCodeMap::getKeyCodeCount() {

	initKeyCodeMap();

	return mKeyCodeMap.getCount();
}

const TCHAR* WinVirtualKeyCodeMap::getKeyCodeAt(const unsigned int& index, int& code) {

	initKeyCodeMap();

	if (index >= mKeyCodeMap.getCount())
		return NULL;

	KeyCodeMap::Iterator i = mKeyCodeMap.iterator();
	unsigned int idx = 0;

	KeyCode keyCode;

	while (mKeyCodeMap.next(i, keyCode)) {

		if (idx == index) {

			code = keyCode.code;
			return keyCode.name;
		}

		++idx;
	}

	return NULL;
}

void WinVirtualKeyCodeMap::initKeyCodeMap() {

	if (mKeyCodeMap.getCount() != 0) {

		return;
	}

	mKeyCodeMap.put(L"Esc", VK_ESCAPE);
	mKeyCodeMap.put(L"F1", VK_F1);
	mKeyCodeMap.put(L"F2", VK_F2);
	mKeyCodeMap.put(L"F3", VK_F3);
	mKeyCodeMap.put(L"F4", VK_F4);
	mKeyCodeMap.put(L"F5", VK_F5);
	mKeyCodeMap.put(L"F6", VK_F6);
	mKeyCodeMap.put(L"F7", VK_F7);
	mKeyCodeMap.put(L"F8", VK_F8);
	mKeyCodeMap.put(L"F9", VK_F9);
	mKeyCodeMap.put(L"F10", VK_F10);
	mKeyCodeMap.put(L"F11", VK_F11);
	mKeyCodeMap.put(L"F12", VK_F12);
	mKeyCodeMap.put(L"F13", VK_F13);
	mKeyCodeMap.put(L"F14", VK_F14);
	mKeyCodeMap.put(L"F15", VK_F15);
	mKeyCodeMap.put(L"F16", VK_F16);
	mKeyCodeMap.put(L"F17", VK_F17);
	mKeyCodeMap.put(L"F18", VK_F18);
	mKeyCodeMap.put(L"F19", VK_F19);
	mKeyCodeMap.put(L"F20", VK_F20);

	mKeyCodeMap.put(L"Print", VK_SNAPSHOT);
	mKeyCodeMap.put(L"Scroll", VK_SCROLL);
	mKeyCodeMap.put(L"Pause", VK_PAUSE);
	mKeyCodeMap.put(L"`", VK_OEM_3);

	mKeyCodeMap.put(L"1", '1');
	mKeyCodeMap.put(L"2", '2');
	mKeyCodeMap.put(L"3", '3');
	mKeyCodeMap.put(L"4", '4');
	mKeyCodeMap.put(L"5", '5');
	mKeyCodeMap.put(L"6", '6');
	mKeyCodeMap.put(L"7", '7');
	mKeyCodeMap.put(L"8", '8');
	mKeyCodeMap.put(L"9", '9');
	mKeyCodeMap.put(L"0", '0');

	mKeyCodeMap.put(L"-", VK_OEM_MINUS);
	mKeyCodeMap.put(L"=", VK_OEM_NEC_EQUAL);
	mKeyCodeMap.put(L"Back", VK_BACK);

	mKeyCodeMap.put(L"Tab", VK_TAB);
	mKeyCodeMap.put(L"q", 'Q');
	mKeyCodeMap.put(L"w", 'W');
	mKeyCodeMap.put(L"e", 'E');
	mKeyCodeMap.put(L"r", 'R');
	mKeyCodeMap.put(L"t", 'T');
	mKeyCodeMap.put(L"y", 'Y');
	mKeyCodeMap.put(L"u", 'U');
	mKeyCodeMap.put(L"i", 'I');
	mKeyCodeMap.put(L"o", 'O');
	mKeyCodeMap.put(L"p", 'P');
	mKeyCodeMap.put(L"[", VK_OEM_4);
	mKeyCodeMap.put(L"]", VK_OEM_6);
	mKeyCodeMap.put(L"Enter", VK_RETURN);
	mKeyCodeMap.put(L"Caps", VK_CAPITAL);

	mKeyCodeMap.put(L"a", 'A');
	mKeyCodeMap.put(L"s", 'S');
	mKeyCodeMap.put(L"d", 'D');
	mKeyCodeMap.put(L"f", 'F');
	mKeyCodeMap.put(L"g", 'G');
	mKeyCodeMap.put(L"h", 'H');
	mKeyCodeMap.put(L"j", 'J');
	mKeyCodeMap.put(L"k", 'K');
	mKeyCodeMap.put(L"l", 'L');
	mKeyCodeMap.put(L";", VK_OEM_1);
	mKeyCodeMap.put(L"'", VK_OEM_7);
	mKeyCodeMap.put(L"\\", VK_OEM_5);

	mKeyCodeMap.put(L"LeftShift", VK_LSHIFT);
	mKeyCodeMap.put(L"z", 'Z');
	mKeyCodeMap.put(L"x", 'X');
	mKeyCodeMap.put(L"c", 'C');
	mKeyCodeMap.put(L"v", 'V');
	mKeyCodeMap.put(L"b", 'B');
	mKeyCodeMap.put(L"n", 'N');
	mKeyCodeMap.put(L"m", 'M');
	mKeyCodeMap.put(L",", VK_OEM_COMMA);
	mKeyCodeMap.put(L".", VK_OEM_PERIOD);
	mKeyCodeMap.put(L"/", VK_OEM_2);
	mKeyCodeMap.put(L"RightShift", VK_LSHIFT);

	mKeyCodeMap.put(L"LeftCtrl", VK_LCONTROL);
	mKeyCodeMap.put(L"LeftWin", VK_LWIN);
	mKeyCodeMap.put(L"LeftAlt", VK_LMENU);
	mKeyCodeMap.put(L" ", VK_SPACE);
	mKeyCodeMap.put(L"RightAlt", VK_RMENU);
	mKeyCodeMap.put(L"RightWin", VK_RWIN);
	mKeyCodeMap.put(L"Context", VK_APPS);
	mKeyCodeMap.put(L"RightCtrl", VK_RCONTROL);


	mKeyCodeMap.put(L"Insert", VK_INSERT);
	mKeyCodeMap.put(L"Home", VK_HOME);
	mKeyCodeMap.put(L"PageUp", VK_PRIOR);
	mKeyCodeMap.put(L"Delete", VK_DELETE);
	mKeyCodeMap.put(L"End", VK_END);
	mKeyCodeMap.put(L"PageDown", VK_NEXT);

	mKeyCodeMap.put(L"Up", VK_UP);
	mKeyCodeMap.put(L"Left", VK_LEFT);
	mKeyCodeMap.put(L"Down", VK_DOWN);
	mKeyCodeMap.put(L"Right", VK_RIGHT);

	mKeyCodeMap.put(L"Num", VK_NUMLOCK);
	mKeyCodeMap.put(L"Div", VK_DIVIDE);
	mKeyCodeMap.put(L"Mul", VK_MULTIPLY);
	mKeyCodeMap.put(L"Sub", VK_SUBTRACT);
	mKeyCodeMap.put(L"Add", VK_ADD);
	mKeyCodeMap.put(L"Exec", VK_EXECUTE);
	
	mKeyCodeMap.put(L"Num0", VK_NUMPAD0);
	mKeyCodeMap.put(L"Num1", VK_NUMPAD1);
	mKeyCodeMap.put(L"Num2", VK_NUMPAD2);
	mKeyCodeMap.put(L"Num3", VK_NUMPAD3);
	mKeyCodeMap.put(L"Num4", VK_NUMPAD4);
	mKeyCodeMap.put(L"Num5", VK_NUMPAD5);
	mKeyCodeMap.put(L"Num6", VK_NUMPAD6);
	mKeyCodeMap.put(L"Num7", VK_NUMPAD7);
	mKeyCodeMap.put(L"Num8", VK_NUMPAD8);
	mKeyCodeMap.put(L"Num9", VK_NUMPAD9);
	mKeyCodeMap.put(L"Num.", VK_DECIMAL);

	mKeyCodeMap.put(L"Sleep", VK_SLEEP);

	mKeyCodeMap.put(L"Clear", VK_CLEAR);
	mKeyCodeMap.put(L"Conv", VK_CONVERT);
	mKeyCodeMap.put(L"NonConv", VK_NONCONVERT);

	mKeyCodeMap.put(L"Accept", VK_ACCEPT);
	mKeyCodeMap.put(L"ModeChange", VK_MODECHANGE);
	mKeyCodeMap.put(L"Help", VK_HELP);

	mKeyCodeMap.put(L"Attn", VK_ATTN);
	mKeyCodeMap.put(L"CrSel", VK_CRSEL);
	mKeyCodeMap.put(L"ExeSel", VK_EXSEL);
	mKeyCodeMap.put(L"ErEOF", VK_EREOF);
	mKeyCodeMap.put(L"Play", VK_PLAY);
	mKeyCodeMap.put(L"Zoom", VK_ZOOM);
		
}

} 

#endif
