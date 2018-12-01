#ifndef _WEVersion_h
#define _WEVersion_h

#include "WEString.h"

namespace WE {

	struct Version {

		int major;
		int minor;
		unsigned long build;
		const TCHAR* name;
		const TCHAR* comment;

		Version();
	};

	void getWitchEngineVersion(Version& version);

}

#endif