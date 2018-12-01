#ifndef _WESceneNode_h
#define _WESceneNode_h

#include "WE3/scene/WESceneTransformNodeBase.h"
#include "WE3/WETL/WETLTree.h"
#include "WE3/WEPtr.h"
#include "WE3/object/WEObject.h"
using namespace WE;

#include "EESceneEvent.h"
#include "EESceneNodeMoveTester.h"

namespace EE {

	class Scene;

	class SceneNode : public Object, public WETL::TreeNodeT<SceneNode>, public SceneTransformNodeBase {
	public:

		virtual void nodeProcessSceneEvt(SceneBaseEvent& sceneEvt, const bool& nodeWasDirty);

	public:

		
		bool nodeTreeClean();
		bool nodeTreeRender();
		bool nodeTreeMessage(const SceneEventType& evt);
		bool nodeTreeEvent(SceneBaseEvent& evt);

		//simply propagates to all children (doesnt clean on the way)
		void nodePropagate(const SceneEventType& evt, bool includeThis = true); 
		void nodePropagate(SceneBaseEvent& sceneEvt, bool includeThis = true); 

	public:

		SceneNode();
		virtual ~SceneNode();

		bool nodeInit(Scene& scene, const ObjectType& type, const ObjectIndex& id, const TCHAR* name);
		bool nodeIsInited();
		void nodeDestroy(bool resetTreeNode); //use with care!!!

		const TCHAR* nodeName();

		bool nodeIsDirty();

		/*
		const Vector3& nodeLocalPosition();
		const Vector3& nodeWorldPosition();

		//nodeMarkDirty should be set b4 calling these functions
		void nodeLookAt(const Vector3& lookAt); //uses scene up
		void nodeLookAt(const Vector3& lookAt, const Vector3& unitUp);
		void nodeOrient(const Vector3& direction); //uses scene up
		*/
	
		//only works when parent is clean
		//reusing the tester object is recommended for performance
		//IF useCollDetectorTest is enabled: 
		//  ICollDetecttor.startTest method is used
		//  tester.getCollTestData can then be used to see what happene dwith the collisions
		//  and then the accept flag can be used to accept or reject the test
		bool nodeMoveTestStart(SceneNodeMoveTester& tester, const Vector3& testPos, bool useCollDetectorTest); 
		void nodeMoveTestEnd(SceneNodeMoveTester& tester, bool accept);
		
	public:

		//all children will also become dirty, propagates to whole subtree
		void nodeMarkDirty();
				
		//can only be called on a root node
		bool nodeTreeUpdate(SceneBaseEvent& sceneEvt);
		
	public:

		//must be dirty to return valid transform
		//SceneTransform& nodeGetLocalTransform();

	protected:

		inline void nodeDispatchEvt(const bool& nodeWasDirty, SceneBaseEvent& sceneEvt) {

			nodeProcessSceneEvt(sceneEvt, nodeWasDirty);
		}

		bool nodeUpdateSubtree(SceneBaseEvent& sceneEvt, bool includeThis);
		void nodeRecurseUpdate(SceneBaseEvent& sceneEvt);
		void nodeDirtyNodeRecurseUpdate(const SceneTransform& parentWorldTransform, SceneBaseEvent& sceneEvt);
		void nodeRecursePropagate(SceneBaseEvent& sceneEvt);

	public:

		SoftPtr<Scene> mNodeScene;
		String mNodeName;

		unsigned short mNodeIsDirty;
	};

}

#endif