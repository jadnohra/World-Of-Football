#ifndef _WESceneTransformArray_h
#define _WESceneTransformArray_h

#include "../WETL/WETLArray.h"
#include "WeSceneDataTypes.h"
#include "WESceneTransform.h"

namespace WE {

	typedef WETL::ArrayBase<SceneTransform, SceneTransformIndex::Type> SceneTransforms;
	typedef WETL::StaticArray<SceneTransform, false, SceneTransformIndex::Type> StaticSceneTransformArray;
}

#endif