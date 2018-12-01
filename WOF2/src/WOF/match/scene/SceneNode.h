#ifndef h_WOF_match_SceneNode
#define h_WOF_match_SceneNode

#include "WE3/scene/WESceneTransformNodeBase.h"
#include "WE3/WETL/WETLTree.h"
#include "WE3/WEPtr.h"
#include "WE3/object/WEObject.h"
using namespace WE;

#include "../inc/CollisionDef.h"
#include "WE3/render/WERenderer.h"

namespace WOF { namespace match {

	class Match;

	
	/*
	Node Reform:

		* also from code usage we noticed: remove skelInst from being 
			baked into meshInst
		* also move most entity creation and init into entities themselves
			from entityManager

		* virtual fucntions have no cost size (only the table)

		* do we need SceneTransformNodeBase ?
		* do we need mNodeLocalTransformIsDirty ?
		* do we need mNodeWorldTransformChangedFlag ?
		
		* following is used in many places, maybe make a controller
			which controls the follower nodes externally if the nodes allow it
			and tracks the followed nodes (if they allow it)

		* or is a scene node just a tree node with virtual fcts ?
		* decide how and if parent movement is handled by attached nodes
		  and design accordingly
		* some nodes per example dont support having movable parents ?
		* ...
	*/

	class SceneNode : public Object, public WETL::TreeNodeT<SceneNode>, public SceneTransformNodeBase {
	public:

		static inline SceneNode* nodeFromObject(Object* pObject) {
			return (SceneNode*) pObject;
		}

	public:

		virtual int getSceneNodeClassID();

		virtual void render(Renderer& renderer, const bool& cleanIfNeeded, const bool& flagExtraRenders, RenderPassEnum pass) {}

		virtual void addSweptBallContacts(WorldMaterialManager& matManager, 
											const WorldMaterialID& sphereMat, const float& radius, 
											const Vector3& pos, const Vector3& sweepVector, 
											SweptIntersectionArray& intersections, bool executeMaterials) {}
		virtual WorldMaterialID getTriMaterial(const ObjectBinding* pBinding);
		/*
		virtual bool getOBBContact(const OBB& obb, Vector& normal, float& penetration) {return false;}
		*/

		virtual bool isCameraIdle() { return true; }

		SceneNode();
		virtual ~SceneNode();

		bool nodeInit(Match& match, const ObjectType& type, const ObjectIndex& id, const TCHAR* name);
		bool nodeIsInited();
		void nodeDestroy(bool resetTreeNode); //use with care!!!

		virtual bool loadTransform(BufferString& tempStr, Match& match, DataChunk* pChunk, CoordSysConv* pConv) { return true; }

		const TCHAR* nodeName();

		bool nodeIsDirty();

	public:

		//all children will also become dirty, propagates to whole subtree
		void nodeMarkDirty();
				
		//can only be called on a root node
		bool rootNodeTreeClean();
		
	protected:

		inline void transformUpdateWorld(const SceneTransform& parentTransformWorld) {
			mNodeWorldTransformChangedFlag = true;
			baseTransformUpdateWorld(parentTransformWorld);
		}

		
		inline void nodeDispatchEvt(const bool& nodeWasDirty) {
		}

		bool nodeCleanSubtree(bool includeThis);
		void nodeRecurseClean();
		void nodeDirtyNodeRecurseClean(const SceneTransform& parentWorldTransform);

	public:

		SoftPtr<Match> mNodeMatch;
		String mNodeName;

		unsigned short mNodeLocalTransformIsDirty;
		unsigned short mNodeWorldTransformChangedFlag;
	};

} }

#endif