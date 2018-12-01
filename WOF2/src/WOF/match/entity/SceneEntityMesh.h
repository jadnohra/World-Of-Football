#ifndef h_WOF_match_SceneEntityMesh
#define h_WOF_match_SceneEntityMesh

#include "WE3/WEPtr.h"
#include "WE3/mesh/WEMeshInstance.h"
using namespace WE;

#include "../DataTypes.h"
#include "../scene/SceneNode.h"
#include "../inc/CollisionDef.h"
#include "SceneDynVol.h"


namespace WOF { namespace match {


	class SceneEntityMesh : public SceneNode {
	public:

		static inline SceneEntityMesh* entMeshFromObject(Object* pObject) {
			return (SceneEntityMesh*) pObject;
		}

		static inline SceneEntityMesh* entMeshFromNode(SceneNode* pNode) {
			return (SceneEntityMesh*) pNode;
		}

	public:

		virtual int getSceneNodeClassID();

		virtual void addSweptBallContacts(WorldMaterialManager& matManager, 
											const WorldMaterialID& sphereMat, const float& radius, 
											const Vector3& pos, const Vector3& sweepVector, 
											SweptIntersectionArray& intersections, bool executeMaterials);

		virtual WorldMaterialID getTriMaterial(const ObjectBinding* pBinding);
		/*
		virtual bool getOBBContact(const OBB& obb, Vector& normal, float& penetration);
		*/

		SceneEntityMesh();
		~SceneEntityMesh();

		bool _init_mesh(Mesh& mesh);
		bool _init_collider(const WorldMaterialID& collMaterial);
		bool _init_NPGeomCollider();
		void _init_shadowing(bool enable);

		bool init_create(Match& match, BufferString& tempStr, DataChunk& chunk, CoordSysConv* pConv, ObjectType objType = Node_EntMesh);
		bool init_nodeAttached(BufferString& tempStr, DataChunk& chunk, CoordSysConv* pConv);
		void init_prepareScene();

		bool clean();

		virtual void render(Renderer& renderer, const bool& cleanIfNeeded, const bool& flagExtraRenders, RenderPassEnum pass);

		inline const Vector3& getLocalPos() { return mTransformLocal.row[3]; }
		inline Vector3& localPos() { return mTransformLocal.row[3]; }
		inline const Vector3& getWorldPos() { return mTransformWorld.row[3]; }
		inline Vector3& worldPos() { return mTransformWorld.row[3]; }

	public:

		inline void setVisible(bool visible) { mIsVisible = visible; }

		bool hasProxyVol();
		SceneDynVol* getProxyVol();
		SceneDynVol* createProxyVol(VolumeStruct& refVolume, const ObjectType& objType = Node_DynVol);
		void destroyProxyVol();

		SceneStaticVol* createStaticVol(const TCHAR* name, const bool& registerWithCollEngine, const ObjectType& objType = Node_DynVol);

		bool extractBoneMeshLocalTransformAt(const TCHAR* boneName, const TCHAR* animName, Time time, SceneTransform& result);
		bool extractBoneMeshLocalTransformAt(BoneIndex::Type boneIndex, AnimationIndex::Type animIndex, Time time, SceneTransform& result);

	public:

		MeshInstance mMeshInst;
		HardPtr<SkeletonInstance> mSkelInst;

		struct ColliderContainer {

			union {

				struct {

					HardPtr<VolumeCollider> staticColl;
				};

				struct {

					HardPtr<DynamicVolumeCollider> dynamicColl;
				};
			};

			//Volume volume;
		};

		HardPtr<MeshCollider> mMeshCollider;
		HardPtr<ColliderContainer> mVolumeColliders;
		
		
		Volume mNonSkeletalVolumeLocal;
		bool mEnableShadowing;
		bool mIsVisible;

		HardPtr<SceneDynVol> mProxyVol;

	protected:

		struct BoneAttachment {

			SoftPtr<BoneInstance> boneInst;
		};

		HardPtr<BoneAttachment> mBoneAttachment;

	protected:

		bool _init_nonSkeletalVolumeLocal();
	};

} }


#endif