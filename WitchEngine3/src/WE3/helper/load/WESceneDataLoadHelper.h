#ifndef _WESceneDataLoadHelper_h
#define _WESceneDataLoadHelper_h

#include "../../string/WEBufferString.h"
#include "../../data/WEDataSource.h"

#include "../../scene/WESceneTransformation.h"

namespace WE {

	class SceneDataLoadHelper {
	public:

		//pos="x,y,z", axis="a", angle="t"
		//returns false if transformation is invalid
		static bool extract(BufferString& tempString, DataChunk& chunk, SceneTransformation& trans);
	};
}

#endif