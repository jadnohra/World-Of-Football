#include "WESkeletonInstance.h"


namespace WE {


SkeletonInstance::SkeletonInstance(bool useTrackAnimCache) : mpSkeleton(NULL) {

	mCurrAnim.setNull();

	mLoop = false;
	mTime = 0.0f;
	mLoopClippedTime = 0.0f;
	mSpeedFactor = 1.0f;

	mNeedsUpdateAnim = false;
	mNeedsUpdateWorldMatrix = false;

	mCurrAnimWeight = 1.0f;

	if (useTrackAnimCache)
		WE_MMemNew(mTrackAnimCache.ptrRef(), TrackAnimCache());
}	


SkeletonInstance::~SkeletonInstance() {

	destroy();
}

void SkeletonInstance::destroy() {

	if (mpSkeleton) {

		mpSkeleton = NULL;

		mCurrAnim.setNull();
		mLoop = false;
		mTime = 0.0f;
		mLoopClippedTime = 0.0f;
		mSpeedFactor = 1.0f;

		mNeedsUpdateAnim = false;
		mNeedsUpdateWorldMatrix = false;
	}

	mWorldMatrix.destroy();
}

//if NULL allocates and uses own matrix
void SkeletonInstance::useWorldMatrix(Matrix4x3Base* pExternalWorldMatrix) {

	if (pExternalWorldMatrix != NULL) {

		mWorldMatrix.set(pExternalWorldMatrix, Ptr_SoftRef);

	} else {

		Matrix4x3Base* pOwnMatrix;
		MMemNew(pOwnMatrix, Matrix4x3Base);

		mWorldMatrix.set(pOwnMatrix, Ptr_HardRef);
	}
}

Skeleton* SkeletonInstance::getSkeleton() {

	return mpSkeleton;
}

AAB& SkeletonInstance::getLocalDynamicAAB() {

	updateIfNeeded();

	return binding.dynamicAAB;
}


void SkeletonInstance::bind(Skeleton* pSkeleton, float boneBoxScale) {

	mpSkeleton = pSkeleton;

	//mWorldMatrix->identity();

	mNeedsUpdateAnim = true;
	mNeedsUpdateWorldMatrix = true;

	binding.dynamicAAB.empty();
	binding.boneInstances.destroy();
	binding.boneWorldTransforms.destroy();

	if (mpSkeleton) {

		binding.boneInstances.setSize(mpSkeleton->mBoneArray.size);
		binding.boneWorldTransforms.setSize(mpSkeleton->mBoneArray.size);

		binding.boneBoxScale = boneBoxScale > 0.0f ? boneBoxScale : mpSkeleton->mBoneBoxScale;

		Bone* pBone;
		BoneInstance* pInst;

		for (BoneIndex::Type i = 0; i < binding.boneInstances.size; i++) {

			pBone = &(mpSkeleton->mBoneArray.el[i]);
			pInst = &(binding.boneInstances.el[i]);

			pInst->paletteIndex = pBone->paletteIndex;
			pInst->skin.skinTransform.identity();

			pInst->localBox.basis().identity();
			pInst->localBox.position().zero();

			/*
			if (initCallback) {

				//(initCallback)(pInst->paletteIndex, pInst->contributeToVolume, pInst->enableColl, callbackContext);
				(initCallback)(pBone->name, pInst->contributeToVolume, pInst->enableColl, callbackContext);
			}
			*/

			binding.boneWorldTransforms.el[i].identity();
		}

		if (mTrackAnimCache.isValid())
			mTrackAnimCache->init(dref(mpSkeleton));
	}
}

SceneTransforms& SkeletonInstance::getWorldTransformArray() {

	return binding.boneWorldTransforms;
}

bool SkeletonInstance::hasValidSkeleton() {

	return ((mpSkeleton != NULL) && (mpSkeleton->getBoneCount() > 0));
}

void SkeletonInstance::setLoop(bool loop) {

	mLoop = loop;
}

void SkeletonInstance::getAnimation(AnimationIndex::Type& anim, float& speedFactor) {

	anim = mCurrAnim;
	speedFactor = mSpeedFactor;
}

float SkeletonInstance::getAnimationLength(const AnimationIndex::Type& anim, float speedFactor, bool includeSpeed) {

	assrt(AnimationIndex::isValid(anim));

	return mpSkeleton->getAnimationLength(anim) / speedFactor;
}



void SkeletonInstance::addBlendAnimation(const AnimationIndex::Type& anim, const float& weight) {

	BlendAnim& blendAnim = mBlendAnims.addOne();

	blendAnim.anim = anim;
	blendAnim.weight = weight;
	
	
	blendAnim.length = mpSkeleton->mAnimationGroup.mAnimations.el[anim]->mLength;
	blendAnim.speedFactor = (mCurrentLength * mSpeedFactor) / blendAnim.length;
	blendAnim.time = 0.0f;
	blendAnim.loopClippedTime = 0.0f;
}

void SkeletonInstance::setAnimationWeight(const float& weight) {

	mCurrAnimWeight = weight;
}

void SkeletonInstance::resetBlendAnimations() {

	mBlendAnims.count = 0;
	mCurrAnimWeight = 1.0f;
}

void SkeletonInstance::setAnimation(const AnimationIndex::Type& anim, const float& speedFactor) {
	
	resetBlendAnimations();

	if (mCurrAnim != anim && AnimationIndex::isValid(anim)) {

		mCurrentLength = mpSkeleton->mAnimationGroup.mAnimations.el[anim]->mLength;
		mLoop = mpSkeleton->mAnimationGroup.mAnimations.el[anim]->mLoop;
	}

	mCurrAnim = anim;
	setSpeedFactor(speedFactor);


	if (AnimationIndex::isValid(anim)) {

		setTime(0.0f);
		//setWorldMatrix(Matrix4x3Base::kIdentity);
	}

	mNeedsUpdateAnim = true;
}

void SkeletonInstance::setSpeedFactor(const float& speedFactor) {

	mSpeedFactor = speedFactor;
}

void SkeletonInstance::addTime(const Time& time) {

	if (mCurrAnim.isValid()) {

		setTime(mTime + (time * (/*mpSkeleton->mAnimationGroup.mAnimations.el[mCurrAnim]->mSpeed * */mSpeedFactor)));
	}
}


void SkeletonInstance::setTime(const Time& time) {

	if (mCurrAnim.isValid()) {

		mTime = time;
		mLoopClippedTime = mTime;

		if (mLoop) {

			int multi = (int) (mLoopClippedTime / mCurrentLength);
			mLoopClippedTime -= (int) (multi * mCurrentLength);
		} 

		for (BlendAnims::Index i = 0; i < mBlendAnims.count; ++i) {

			BlendAnim& blendAnim = mBlendAnims.el[i];

			blendAnim.time = blendAnim.speedFactor * (mTime / mSpeedFactor);
			blendAnim.loopClippedTime = blendAnim.time;

			if (mLoop) {

				int multi = (int) (blendAnim.loopClippedTime / blendAnim.length);
				blendAnim.loopClippedTime -= (int) (multi * blendAnim.length);
			}
		}

		mNeedsUpdateAnim = true;
	}
}

bool SkeletonInstance::isAnimating() {

	return (mCurrAnim.isValid());
}

bool SkeletonInstance::reachedAnimationEnd() {

	if (mCurrAnim.isNull()) {

		return true;
	}

	if (mLoop) {

		return false;
	}

	return (mTime >= mCurrentLength);
}

float SkeletonInstance::getTime(bool loopClipped) {

	if (loopClipped) {

		return mLoopClippedTime;
	}

	return mTime;
}


void SkeletonInstance::setWorldMatrix(const Matrix4x3Base* pWorldMatrix) {

	mNeedsUpdateWorldMatrix = true;

	if (pWorldMatrix != NULL) {

		mWorldMatrix.dref() = *pWorldMatrix;
	}
}

void SkeletonInstance::signalExternalWorldMatrixChanged() {

	mNeedsUpdateWorldMatrix = true;
}

//void SkeletonInstance::prepareForRendering() {
//	updateIfNeeded();
//}


bool SkeletonInstance::update() {

	return updateIfNeeded();
}

bool SkeletonInstance::updateIfNeeded() {

	bool updated = false;

	if (mNeedsUpdateAnim) {

		if (mBlendAnims.count)
			mpSkeleton->setState(mCurrAnim, mLoopClippedTime, mTrackAnimCache, mCurrAnimWeight, mBlendAnims.count, mBlendAnims.el, sizeof(BlendAnim));
		else
			mpSkeleton->setState(mCurrAnim, mLoopClippedTime, mTrackAnimCache, mCurrAnimWeight);
		
		mpSkeleton->updateRecurse(&binding);
		mNeedsUpdateAnim = false;
		updated = true;
	}

	if (updated || mNeedsUpdateWorldMatrix) {

		setState(mWorldMatrix);
		mNeedsUpdateWorldMatrix = false;
		updated = true;
	}

	return updated;
}

BoneInstance* SkeletonInstance::findBoneInstanceByGameName(const TCHAR* name) {

	for (BoneIndex::Type i = 0; i < binding.boneInstances.size; i++) {

		Bone& bone = mpSkeleton->getBone(binding.boneInstances.el[i].paletteIndex);

		if (bone.gameName.equals(name))
			return &binding.boneInstances.el[i];
	}

	return NULL;
}

BoneInstance* SkeletonInstance::findBoneInstanceByName(const TCHAR* name) {

	for (BoneIndex::Type i = 0; i < binding.boneInstances.size; i++) {

		Bone& bone = mpSkeleton->getBone(binding.boneInstances.el[i].paletteIndex);

		if (bone.name.equals(name))
			return &binding.boneInstances.el[i];
	}

	return NULL;
}

void SkeletonInstance::renderBoundingBoxes(Renderer& renderer, const RenderColor* pColor, const RenderColor* pMarkedColor) {

	updateIfNeeded();
	
	for (BoneIndex::Type i = 0; i < binding.boneInstances.size; i++) {

		
		//GeometryRenderer::global()->render(pDevice, binding.boneInstances.el[i].localBox, false, 1.0f, 0.0f, 
		//									binding.boneInstances.el[i].markedColl ? 0.0f : 1.0f);
		

		//if (binding.boneInstances.el[i].markedColl) {

		//PUT THIS BACK
			if (mpSkeleton->getBone(binding.boneInstances.el[i].paletteIndex).enableColl) {
				renderer.draw(binding.boneInstances.el[i].worldBox, &Matrix4x3Base::kIdentity, 
							binding.boneInstances.el[i].markedColl ? pMarkedColor : pColor);

				/*
				Bone& bone = mpSkeleton->mBoneArray[binding.boneInstances[i].paletteIndex];

				renderer.draw((RigidMatrix4x3&) bone.mTransformWorld, 20.0f);
				*/
				//renderer.draw((RigidMatrix4x3&) binding.boneWorldTransforms[i], 20.0f);
			}
		//GeometryRenderer::global()->render(pDevice, binding.boneInstances.el[i].worldBox, true, 1.0f, 0.0f, 
		//									binding.boneInstances.el[i].markedColl ? 0.0f : 1.0f);

		//}
		
		
		//GeometryRenderer::global()->render(pDevice, binding.boneInstances.el[i].localBox.basis(true), 
		//										binding.boneInstances.el[i].localBox.position(true), false);
		

		binding.boneInstances.el[i].markedColl = false;
	}

	

}


void SkeletonInstance::test(const SkeletonInstance* pCollInstance) {

	for (BoneIndex::Type i = 0; i < binding.boneInstances.size; i++) {

		if (binding.boneInstances.el[i].localBox.isEmpty() == false) {

			for (BoneIndex::Type j = 0; j < pCollInstance->binding.boneInstances.size; j++) {

				if (mpSkeleton->getBone(pCollInstance->binding.boneInstances.el[i].paletteIndex).enableColl) {

					if (pCollInstance->binding.boneInstances.el[j].localBox.isEmpty() == false) {

						if (intersect(binding.boneInstances.el[i].worldBox, pCollInstance->binding.boneInstances.el[j].worldBox)) {
							binding.boneInstances.el[i].markedColl = true;
							//break;
						}

					}
				}

			}
		}
	}

}

void SkeletonInstance::test(const OBB* pBox) {

	for (BoneIndex::Type i = 0; i < binding.boneInstances.size; i++) {

		
		if (mpSkeleton->getBone(binding.boneInstances.el[i].paletteIndex).enableColl) {

			if (binding.boneInstances.el[i].localBox.isEmpty() == false) {

				if (intersect(binding.boneInstances.el[i].worldBox, *pBox)) {
						binding.boneInstances.el[i].markedColl = true;
						//break;
				}
			}
		}
	}

}

void SkeletonInstance::setState(const Matrix4x3Base& worldMatrix) {

	for (BoneIndex::Type i = 0; i < binding.boneInstances.size; i++) {

		mul(binding.boneInstances.el[i].skin.skinTransform, worldMatrix, binding.boneWorldTransforms.el[i]);

		if (mpSkeleton->getBone(binding.boneInstances.el[i].paletteIndex).enableColl) {

			binding.boneInstances.el[i].localBox.transform(worldMatrix, binding.boneInstances.el[i].worldBox);
			mul(mpSkeleton->getBone(binding.boneInstances.el[i].paletteIndex).mTransformWorld, worldMatrix, binding.boneInstances.el[i].worldTransform);
		}
		//mul(binding.boneInstances.el[i].skin.skinTransform, worldMatrix, binding.boneInstances.el[i].skin.skinWorldTransform);
	}
}

void SkeletonInstance::setDeviceMatrices(Renderer& renderer, BYTE* boneIndices, USHORT boneCount) {
	
	if (binding.boneInstances.size <= 0) {
		return;
	}
	
	updateIfNeeded();
	
	
	for (USHORT b = 0; b < boneCount; b++) {

		renderer.setTransform(binding.boneWorldTransforms.el[boneIndices[b]], RT_World, b);
	}

}

void SkeletonInstance::setDeviceMatrixPalette(Renderer& renderer) {

	if (binding.boneInstances.size <= 0) {

		return;
	}

	updateIfNeeded();

	for (BoneIndex::Type i = 0; i < binding.boneInstances.size; i++) {

		if (binding.boneInstances.el[i].paletteIndex >= 0) {

			renderer.setTransform(binding.boneWorldTransforms.el[i], RT_World, binding.boneInstances.el[i].paletteIndex);
		}
	}
}


}