#ifndef _EESceneNodeLocation_h
#define _EESceneNodeLocation_h

#include "../EESceneNode.h"

namespace EE {

	
	class SceneNodeLocation : public SceneNode {
	public:

		SceneNodeLocation();

		virtual void nodeProcessSceneEvt(SceneBaseEvent& sceneEvt, const bool& nodeWasDirty);

	public:
	};

}

#endif