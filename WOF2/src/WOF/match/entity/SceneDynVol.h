#ifndef h_WOF_match_SceneDynVol
#define h_WOF_match_SceneDynVol

#include "WE3/WEPtr.h"
#include "WE3/mesh/WEMeshInstance.h"
using namespace WE;

#include "../DataTypes.h"
#include "../scene/SceneNode.h"
#include "../collision/Colliders.h"


namespace WOF { namespace match {

	/*
		although we inherit from SceneNode, we are not really a SceneNode, 
		but we do this becuase of the virtual function addSweptBallContacts

		to fix this a revamp is needed
	*/
	class SceneDynVol : public SceneNode {
	public:

		static inline SceneDynVol* dynVolFromObject(Object* pObject) {
			return (SceneDynVol*) pObject;
		}

		static inline SceneDynVol* dynVolFromNode(SceneNode* pNode) {
			return (SceneDynVol*) pNode;
		}

	public:

		SceneDynVol();
		~SceneDynVol();

		bool create(Match& match, Object* pRefObject, const TCHAR* name, const WorldMaterialID& collMaterial, const Vector3& pos, VolumeStruct& refVolume, ObjectType objType = Node_DynVol);
		void update(const Vector3& pos);

		virtual void render(Renderer& renderer, const bool& cleanIfNeeded, const bool& flagExtraRenders, RenderPassEnum pass);

		virtual void addSweptBallContacts(WorldMaterialManager& matManager, 
											const WorldMaterialID& sphereMat, const float& radius, 
											const Vector3& pos, const Vector3& sweepVector, 
											SweptIntersectionArray& intersections, bool executeMaterials);
	


		inline DynamicVolumeCollider& getCollider() { return mVolColl; }

	public:

		DynamicVolumeCollider mVolColl;
		Volume mRefVolume;

		SoftPtr<Object> mRefObject;
	};


	class SceneStaticVol : public SceneNode {
	public:

		static inline SceneStaticVol* staticVolFromObject(Object* pObject) {
			return (SceneStaticVol*) pObject;
		}

		static inline SceneStaticVol* staticVolFromNode(SceneNode* pNode) {
			return (SceneStaticVol*) pNode;
		}

	public:

		SceneStaticVol();
		~SceneStaticVol();

		bool create(Match& match, Object* pRefObject, const TCHAR* name, const WorldMaterialID& collMaterial, VolumeStruct& volume, const bool& registerWithCollEngine, ObjectType objType = Node_DynVol);

		virtual void render(Renderer& renderer, const bool& cleanIfNeeded, const bool& flagExtraRenders, RenderPassEnum pass);

		virtual void addSweptBallContacts(WorldMaterialManager& matManager, 
											const WorldMaterialID& sphereMat, const float& radius, 
											const Vector3& pos, const Vector3& sweepVector, 
											SweptIntersectionArray& intersections, bool executeMaterials);
	


		inline VolumeCollider& getCollider() { return mVolColl; }
		inline VolumeStruct& getColliderVolume() { return mVolColl.volume; }

	public:

		VolumeCollider mVolColl;

		SoftPtr<Object> mRefObject;
	};

} }


#endif