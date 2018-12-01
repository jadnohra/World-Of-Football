#include "WESkeletonBone.h"
#include "WESkeletonUpdateMode.h"

#include "../math/WEVolume.h"

namespace WE {

Bone::Bone() {

	mTransformLocal.identity();
	paletteIndex = -1;

	contributeToVolume = true;
	enableColl = true;
}

void Bone::emptyToBoundingBox() {

	boundingBox.empty();
}

void Bone::addToBoundingBox(const Point& meshPoint) {

	Point boneSpacePoint;

	init_inverseBoneWorldMatrix.transformPoint(meshPoint, boneSpacePoint);
	boundingBox.add(boneSpacePoint);
}

void Bone::applyConverterToInit(CoordSysConv& conv) {

	init.applyConverter(conv);
	setLocalMatrixFromInit();

	conv.toTargetVolume(boundingBox);
}

void Bone::transformInit(const Matrix4x3Base& transf) {

	init.transform(transf);
	setLocalMatrixFromInit();

	transform(transf, boundingBox);
}

void Bone::setLocalMatrix(const SceneTransformation& trans) {

	mTransformLocal.setTransformation(trans);
}

void Bone::setLocalMatrixFromInit() {

	setLocalMatrix(init);
}

void Bone::boneUpdate(BoneNodeRecurseContext& context) {

	switch(context.mode) {

		case BR_SetStartPose: 
			{
				inverse(mTransformWorld, init_inverseBoneWorldMatrix);
			}
			break;

		case BR_Update: 
			{

				if (context.pInstanceBinding) {

					BoneInstance& inst = context.pInstanceBinding->boneInstances.el[paletteIndex];

					mul(init_inverseBoneWorldMatrix, mTransformWorld, inst.skin.skinTransform);
					
					if (contributeToVolume) {

						inst.localBox.init(boundingBox, context.pInstanceBinding->boneBoxScale);
						mTransformWorld.syncBox(inst.localBox);

						if (boundingBox.isEmpty() == false) {

							inst.localBox.mergeTo(context.pInstanceBinding->dynamicAAB);
						} 

					} else {

						enableColl = false;
					}
				}
			}
			break;
	}
}

void Bone::boneTreeRecurseUpdate(const SceneTransform& parentTransformWorld, BoneNodeRecurseContext& context) {

	baseTransformUpdateWorld(parentTransformWorld);
	boneUpdate(context);
	

	Bone* pChild = pFirstChild;

	while (pChild != NULL) {

		pChild->boneTreeRecurseUpdate(mTransformWorld, context);
		pChild = pChild->pNextSibling;
	}
}

}