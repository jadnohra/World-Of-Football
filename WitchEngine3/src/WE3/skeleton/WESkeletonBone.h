#ifndef _WESkeletonBone_h
#define _WESkeletonBone_h

#include "../scene/WESceneTransformNodeBase.h"
#include "../scene/WESceneTransformation.h"
#include "../scene/WESceneTransformArray.h"
#include "../math/WEAAB.h"
#include "../math/WEMatrix.h"
#include "../WETL/WETLTree.h"
#include "../coordSys/WECoordSysConvPool.h"

#include "WESkeletonDataTypes.h"

namespace WE {


	struct BoneSkinTransform {

		SceneTransform skinTransform; //in local mesh space
		//SceneTransform skinWorldTransform; //in world space
	};

	struct BoneInstance {

		int paletteIndex;
		BoneSkinTransform skin;
		Box localBox;
		OBB worldBox;
		SceneTransform worldTransform;

		bool markedColl;
	};
	

	class SkeletonInstanceBoneBinding {
	public:

		//BoneInstance index must correspond to bone paletteIndex
		typedef WETL::StaticArray<BoneInstance, false, BoneIndex::Type> BoneInstances;
		typedef StaticSceneTransformArray BoneWorldTransforms; //in world space
		
		float boneBoxScale;

		BoneInstances boneInstances;
		BoneWorldTransforms boneWorldTransforms;
		AAB dynamicAAB;
	};
	

	enum BoneRecurseMode {
		BR_None = 0, BR_SetStartPose, BR_Update	
	};

	struct BoneNodeRecurseContext {

		BoneRecurseMode mode;
		SkeletonInstanceBoneBinding* pInstanceBinding;

		BoneNodeRecurseContext(BoneRecurseMode _mode = BR_None) {

			mode = _mode;

			pInstanceBinding = NULL;
		}
	};

	struct BoneTransformation : SceneTransformation {
	};

	class Bone : public SceneTransformNodeBase, public WETL::TreeNodeT<Bone> {
	public:

		int paletteIndex;

		BoneTransformation init;
		SceneTransform init_inverseBoneWorldMatrix;	

		Quaternion blendQuaternion; //used as temp Quaternion for blending

		AAB boundingBox;
		String name;
		
		bool contributeToVolume;
		bool enableColl;

		String gameName;
		unsigned int gameID;

		void boneUpdate(BoneNodeRecurseContext& context);
		void boneTreeRecurseUpdate(const SceneTransform& parentTransformWorld, BoneNodeRecurseContext& context);

		void applyConverterToInit(CoordSysConv& conv);
		void transformInit(const Matrix4x3Base& transf);

		void emptyToBoundingBox();
		void addToBoundingBox(const Point& meshPoint);

		void setAsStartPose();
		inline void setLocalMatrix(const SceneTransformation& trans);
		inline void setLocalMatrixFromInit();
		
		Bone();
	};

}

#endif