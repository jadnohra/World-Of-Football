#ifndef _WEPathResolver_h
#define _WEPathResolver_h

#include "../string/WEString.h"
#include "../string/WEBufferString.h"

namespace WE {

	class PathResolver {
	public:
		
		virtual void resolvePath(const TCHAR* path, String& absPath) const = 0;

		static void appendPath(BufferString& currPath, const TCHAR* appendPath, bool process, bool currPathHasFileName);
		static void appendPath(BufferString& currPath, const TCHAR* appendPath, bool process);
		static void simpleAppendPath(BufferString& currPath, const TCHAR* appendPath);
	};

	void resolvePath(PathResolver* pResolver, const TCHAR* path, String& absPath);
}

#endif