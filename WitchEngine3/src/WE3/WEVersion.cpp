#include "WEVersion.h"

#include "autoVersionBuildNumber.h"
#include "string.h"

#define _VERSION_MAJOR_WitchEngine3 0
#define _VERSION_MINOR_WitchEngine3 3
#define _VERSION_NAME_WitchEngine3 L""
#define _VERSION_COMMENT_WitchEngine3 L""


namespace WE {


Version::Version() {

	memset(this, 0, sizeof(Version));
}


void getWitchEngineVersion(Version& version) {

	version.major = _VERSION_MAJOR_WitchEngine3;
	version.minor = _VERSION_MINOR_WitchEngine3;
	version.build = _VERSION_BUILD_WitchEngine3;

	version.name = _VERSION_NAME_WitchEngine3;
	version.comment = _VERSION_COMMENT_WitchEngine3;
}



}

