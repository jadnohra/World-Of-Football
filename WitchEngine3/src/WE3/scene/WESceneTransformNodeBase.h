#ifndef _WESceneTransformNodeBase_h
#define _WESceneTransformNodeBase_h

#include "WESceneTransform.h"

namespace WE {

	
	class SceneTransformNodeBase  {
	public:

		inline void baseTransformUpdateWorld(const SceneTransform& parentTransformWorld) {
			mul(mTransformLocal, parentTransformWorld, mTransformWorld);
		}

	public:

		SceneTransform mTransformLocal; 
		SceneTransform mTransformWorld;
	};

}

#endif