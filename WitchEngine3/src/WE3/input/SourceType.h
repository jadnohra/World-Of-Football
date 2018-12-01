#ifndef _SourceTypes_h_WE_Input
#define _SourceTypes_h_WE_Input

#include "../string/WEString.h"

namespace WE { namespace Input {

	struct SourceType {

		static const TCHAR* Unknown;
		static const TCHAR* Virtual;
		static const TCHAR* Dir2D;
		static const TCHAR* Button;
		static const TCHAR* Axis;
		static const TCHAR* XAxis;
		static const TCHAR* YAxis;
		static const TCHAR* ZAxis;
		static const TCHAR* XRotAxis;
		static const TCHAR* YRotAxis;
		static const TCHAR* ZRotAxis;
		static const TCHAR* POV0;
		static const TCHAR* POV1;
		static const TCHAR* POV2;
		static const TCHAR* Slider;
	};

} }

#endif