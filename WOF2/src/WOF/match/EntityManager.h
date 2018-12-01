#ifndef h_WOF_match_EntityManager
#define h_WOF_match_EntityManager


#include "WE3/scene/WESceneMeshContainer.h"
#include "WE3/scene/WESceneNodeContainer.h"
#include "WE3/WEPtr.h"
using namespace WE;

#include "DataTypes.h"
#include "scene/SceneNodeFactory.h"
#include "scene/SceneNode.h"
#include "MeshPropertyManager.h"
#include "Clock.h"
#include "scene/SceneLoader_v1.h"

#include "inc/EntitiesDef.h"
#include "Team.h"

#include "WE3/spatialManager/kdtree/WESpatialManager_KDTree.h"
#include "WE3/defaultImpl/world/WEWorldPolygonalCollider.h"

namespace WOF { namespace match {

	
	class Match;

	class SceneNodeContainer : public WE::SceneNodeContainer<SceneNode, SceneNodeIndex::Type, WETL::ResizeDouble, 16> {
	};

	class SceneNodeContainerSoft : public WE::SceneNodeContainerSoft<SceneNode, SceneNodeIndex::Type, WETL::ResizeDouble, 16> {
	};

	class SceneEntityMeshContainerSoft : public WE::SceneNodeContainerSoft<SceneEntityMesh, SceneNodeIndex::Type, WETL::ResizeDouble, 16> {
	};

	class SceneScriptedCharacterContainerSoft : public WE::SceneNodeContainerSoft<SceneScriptedCharacter, SceneNodeIndex::Type, WETL::ResizeDouble, 8> {
	};

	class SceneCameraContainerSoft : public WE::SceneNodeContainerSoft<SceneCamera, SceneNodeIndex::Type, WETL::ResizeDouble, 16> {
	};

	class EntityManager : public SceneNodeFactory, public SceneLoaderClient {
	public:

		inline SceneMeshContainer& getMeshes() { return mMeshes; }

		inline SkeletalMeshPropertyManager& getSkelMeshPropManager() { return mSkelMeshProps; }

		inline Team& getTeam(const TeamEnum& team) { return mTeams[team]; }

		Ball& getBall();
		Ball& getSimulBall();
		SceneEntityMesh& getPitch();
		SceneStaticVol& getSimulPitch();

	public:

		EntityManager();
		virtual ~EntityManager();

		void destroyNodes();

		void loadPluginPath(Match& match, String& pluginPath);
		bool loadScene(Match& match);

		virtual void loadUnknownChunk(BufferString& tempStr, Match& match, DataChunk& chunk, const CoordSys& destCoordSys, CoordSysConv* pConv);

		virtual SceneNode* createNode(BufferString& tempStr, Match& match, DataChunk& chunk, CoordSysConv* pConv, bool& success);
		virtual void signalNodeAttached(BufferString& tempStr, Match& match, DataChunk& chunk, SceneNode* pNode, CoordSysConv* pConv);
		virtual SceneNode* getNode(const TCHAR* nodeName);

		SceneNodeIndex::Type genNodeId();

		void setActiveCamera(const TCHAR* camName);
		void setActiveCamera(const SceneNodeIndex::Type& index);
		SceneCamera& getActiveCamera();
		void nextCamera();
		void prevCamera();
		
		void prepareScene();
		void cleanScene();

		//bool handleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
		void frameMove(Match& match, const Clock& time);
		void render(Match& match, Renderer& renderer, bool flagRenderExtras, RenderPassEnum pass);

	protected:

		void updateBallPosession(Match& match, const Clock& time);

	protected:

		bool loadSceneFile(Match& match);
		void prepareScene(SceneNode* pNode);
		void loadManualSceneNodes(Match& match);
		bool loadManualPostPrepareSceneNodes(Match& match);

		void putNode(SceneNode* pNode);

		SceneNode* createEntNode(BufferString& tempStr, Match& match, DataChunk& chunk);
		SceneCamera* createCamera(BufferString& tempStr, Match& match, DataChunk& chunk, CoordSysConv* pConv);
		SceneEntityMesh* createEntMesh(BufferString& tempStr, Match& match, DataChunk& chunk, CoordSysConv* pConv, ObjectType objType = Node_EntMesh);
		SceneScriptedCharacter* createEntMeshScripted(BufferString& tempStr, Match& match, DataChunk& chunk, CoordSysConv* pConv, ObjectType objType = Node_ScriptedCharacter);
		Ball* createBall(BufferString& tempStr, Match& match, DataChunk& chunk, CoordSysConv* pConv);
		Footballer* createFootballer(BufferString& tempStr, Match& match, DataChunk& chunk, CoordSysConv* pConv);
		SceneNode* createLight(BufferString& tempStr, Match& match, DataChunk& chunk, CoordSysConv* pConv);
	public:
	
		bool mSceneIsLoaded;
		bool mSceneIsLoading;
		bool mSceneIsPrepared;

		int mSceneLoadErrCount;

		SceneNodeIndex mNodeIdCounter;

		SceneMeshContainer mMeshes;
		SkeletalMeshPropertyManager mSkelMeshProps;
		
		SceneNodeContainer mAllNodes;
		SceneNodeContainerSoft mNodes;
		SceneCameraContainerSoft mCameras;
		SceneEntityMeshContainerSoft mEntMeshes[2];
		SceneScriptedCharacterContainerSoft mScriptedCharacters;

		SoftPtr<SceneEntityMesh> mPitch;
		SoftPtr<SceneStaticVol> mSimulPitch;

		SoftPtr<Ball> mBall;
		SoftPtr<Ball> mSimulBall;
		Team mTeams[2];

		SceneNode mSceneRoot;

		SoftPtr<SceneCamera> mActiveCam;
		SceneNodeIndex mActiveCamIndex;

		float mControlRadius;

		typedef WETL::CountedArray<Footballer*, false, unsigned int> BallFootballers;
		BallFootballers mBallFootballers;
		BallFootballers mLostBallFootballers;

		/*
		typedef WETL::CountedPtrArray<WorldPolygonalCollider*, false, unsigned int> KDTreeColliders;
		SpatialManagerBuilder_KDTree mKDTreeBuilder;
		SpatialManager_KDTree mKDTree;
		KDTreeColliders mKDTreeColliders;
		SoftPtr<Footballer> mKDTreeFootballer;
		SpatialDynamicVolume mKDTreeFootballerVolume;

		bool mFlagTest;
		int mTestLimit;
		*/

	};

} }

#endif