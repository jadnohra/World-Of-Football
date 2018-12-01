#ifndef _EESceneNodeMoveTester_h
#define _EESceneNodeMoveTester_h

#include "WE3/object/WEObjectData.h"
#include "WE3/coll/broadPhase/WEICollDetector.h"
using namespace WE;

#include "EESceneEvent.h"

namespace EE {

	class SceneNodeMoveTester : public SceneEvent {
	public:

		SceneNodeMoveTester();
		~SceneNodeMoveTester();

		void create(ICollDetector* pCollDetect); 
		void destroy(ICollDetector* pCollDetect); 

	public:

		bool isTestStart;
		bool isCollTestAssisted;
		bool isCollTestAccepted;
		ObjectData collTestData;

		Vector3 origLocalPos;

	protected:

		bool usingCollTestData;
	};

}

#endif