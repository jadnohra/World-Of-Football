#ifndef _EESceneEvent_h
#define _EESceneEvent_h

#include "WE3/render/WERenderer.h"
#include "WE3/WETime.h"
using namespace WE;


namespace EE {

	struct SceneBaseEvent {

		bool isGameEvt;
	};

	struct SceneGameEvent : SceneBaseEvent {

		SceneGameEvent() {

			isGameEvt = true;
		}
	};


	enum SceneEventEnum {

		SE_Init = 0, SE_MoveTest, SE_Render, SE_AddTime
	};

	typedef int SceneEventType;
	
	class SceneNode;
	class Scene;

	struct SceneEvent : SceneBaseEvent {

		SceneEventType type;
		
		union {

			struct {

				Renderer* pRenderer;
			};
			struct {
				
				Time dt;
			};
		};

		SceneEvent() {

			isGameEvt = false;
		}
	};
	
}

#endif