#ifndef _EESceneEntityMesh_h
#define _EESceneEntityMesh_h

#include "WE3/mesh/WEMeshInstance.h"
using namespace WE;

#include "../EESceneEntity.h"

namespace EE {


	class SceneEntityMesh : public SceneEntity {
	public:

		static SceneEntityMesh* fromObject(Object* pObject);
		static SceneEntityMesh* fromNode(SceneNode* pNode);

		SceneEntityMesh();
		virtual ~SceneEntityMesh();

		bool entInit(Scene& scene, Mesh& mesh, bool enableCollider, CollMask* pCollMask, bool enableNPCollider, const String* pCollMaterial, bool overrideMeshMaterials);
		MeshInstance* entGetMeshInstance(bool markDirty);
		NPGeometryCollider* entGetNPCollider();
		
		virtual void nodeProcessSceneEvt(SceneBaseEvent& sceneEvt, const bool& nodeWasDirty);

	protected:

		void entInitLocalVolume(Scene& scene);
		void entCreateNPCollider(Scene& scene, bool overrideMeshMaterials);
		
		//processes all base events except render
		void entProcessBaseEvent(SceneEvent& sceneEvt, const bool& nodeWasDirty);
		void entRender(SceneEvent& sceneEvt, const bool& nodeWasDirty);
		void entInit(const bool& nodeWasDirty, SceneEvent& sceneEvt);
		bool entClean(const bool& nodeWasDirty, SceneEvent& sceneEvt);
		//void entAddTime(const bool& nodeWasDirty, SceneEvent& sceneEvt);
		bool entHandleMoveTest(SceneNodeMoveTester& test);

		void entReTransformNPCollider();
		void _entBlindReTransformNPCollider();

	protected:

		static SceneEntityBitFlagType kInitFlag_2_ReTransfNPCollider;

		bool entNeedsReTransfNPCollider();
		void _entSetNeedsReTransfNPCollider(bool set);

	protected:

		HardPtr<MeshInstance> mEntMeshInstance;
		HardPtr<NPGeometryCollider> mEntNPCollider;
	};
}

#endif