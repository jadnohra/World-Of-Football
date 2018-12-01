#ifndef _EESceneEntity_h
#define _EESceneEntity_h

#include "WE3/object/WEObjectData.h"
#include "WE3/string/WEString.h"
#include "WE3/math/WEVolume.h"
#include "WE3/math/WEMatrix.h"
#include "WE3/coll/narrowPhase/collider/WENPGeometryCollider.h"
using namespace WE;

#include "EESceneDataTypes.h"
#include "EESceneNode.h"


namespace EE {

	typedef unsigned short SceneEntityBitFlagType;

	class SceneEntity : public SceneNode {
	public:

		static SceneEntity* fromObject(Object* pObject);
		static SceneEntity* fromNode(SceneNode* pNode);
		
		void entHighlight(bool highlight);
		
		virtual void nodeProcessSceneEvt(SceneBaseEvent& sceneEvt, const bool& nodeWasDirty);
		Volume& entGetWorldVolume();

	public:

		SceneEntity();	
		virtual ~SceneEntity();
	
		void entEnableCollider(bool enableCollider, const CollMask* pMask);
		bool entColliderIsActive();
		bool entColliderIsAttaching();

	public:

		bool mEntIsVisible;
		SceneEntityBitFlagType mEntInitFlags;
		unsigned short mEntIsHighlighted;

		Volume mEntVolumeLocal;
		Volume mEntVolumeWorld;
		CollMaterial mEntVolumeMaterial;

		unsigned short mEntCollIsActive;
		unsigned short mEntCollEnable;
		ObjectData mEntCollData;

	protected:

		static SceneEntityBitFlagType kInitFlag_1_AttachCollider;

	public:

		inline void entUpdateVolumeWorld(Matrix4x3Base& worldTransf) {
			transform(mEntVolumeLocal, worldTransf, mEntVolumeWorld);
		}

		inline void entSignalColliderVolumeWorldChanged() {
			if (mEntCollEnable) {
				_entSignalColliderVolumeWorldChanged();
			}
		}

		inline void entSignalColliderVolumeWorldChanged(SceneNodeMoveTester& test) {
			if (test.isCollTestAssisted) {
				_entSignalColliderVolumeWorldChanged_CollAssisted(test);
			} else {
				entSignalColliderVolumeWorldChanged();
			}
		}

		bool entIsValidWorldVolume();


		bool entIsAttachedCollider();
		bool entNeedsAttachCollider();

		bool entNeedsInitCollider();
		void entInitCollider(const bool& nodeWasDirty, SceneEvent& sceneEvt);

		bool entInitCollMaterial(Scene& scene, const String& collMaterial);

		bool entNeedsCreateNPCollider();
		void entSignalNPColliderCreated();

		void entCreateColliderData(const CollMask& mask);
		void entAttachCollider();
		void entDetachCollider();
		bool entColliderHasCollisions();


	protected:

		void _entSetNeedsAttachCollider(bool set);
		void _entSignalColliderVolumeWorldChanged();
		void _entSignalColliderVolumeWorldChanged_CollAssisted(SceneNodeMoveTester& test);
	};

}

#endif