#include "WESkeleton.h"
#include "WESkeletonUpdateMode.h"

#include "../WEMacros.h"
#include "../binding/directx/9/d3dx9/includeD3DX9.h"


namespace WE {


AnimationTrack::AnimationTrack() {
}

AnimationTrack::~AnimationTrack() {

	destroy();
}
	
void AnimationTrack::destroy() {

	mKeys.destroy();
}

void AnimationTrack::init(UINT size) {

	mKeys.destroy();

	if (size == 0) {
		return;
	}

	mKeys.reserve(size);
}

AnimationKey* AnimationTrack::addKey() {

	return &(mKeys.addOneReserved());
}



Animation::Animation() {

	mLength = 0.0f;
	//mSpeed = 1.0f;
	mLoop = false;
}

Animation::~Animation() {
	destroy();
}
	
void Animation::destroy() {

	mTracks.destroy();
	mName.destroy();

	mLength = 0.0f;
	//mSpeed = 1.0f;
	mLoop = false;
}

void Animation::init(UINT size) {

	mTracks.destroy();

	if (size == 0) {

		return;
	}

	mTracks.reserve(size);
}

AnimationTrack* Animation::addTrack() {

	AnimationTrack* pTrack;
	MMemNew(pTrack, AnimationTrack());

	mTracks.addOneReserved(pTrack);

	return (pTrack);
}



AnimationGroup::AnimationGroup() {
}

AnimationGroup::~AnimationGroup() {

	destroy();
}
	

void AnimationGroup::destroy() {

	mAnimations.destroy();
}

void AnimationGroup::init(UINT size) {

	destroy();
	
	if (size == 0) {
		return;
	}

	mAnimations.reserve(size);
}

Animation* AnimationGroup::addAnimation() {

	Animation* pAnim;
	MMemNew(pAnim, Animation());

	mAnimations.addOneReserved(pAnim);

	return (pAnim);
}



Skeleton::Skeleton() {

	mBoneBoxScale = 1.0f;
}

Skeleton::~Skeleton() {
}

void Skeleton::destroy() {
	
	mBoneMapByGameID.destroy();
	mBoneArray.destroy();
	mAnimationGroup.destroy();
	mAnimMap.destroy();
}

BoneIndex::Type Skeleton::getBoneCount() {

	return mBoneArray.size;
}

Bone* Skeleton::findBoneByGameName(const TCHAR* name) {

	for (BoneArray::Index i = 0; i < mBoneArray.size; ++i) {

		if (mBoneArray[i].gameName.equals(name)) {

			return &mBoneArray[i];
		}
	}

	return NULL;
}

AnimationIndex::Type Skeleton::getAnimationCount() {

	return mAnimationGroup.mAnimations.count;

}

const TCHAR* Skeleton::getAnimationName(AnimationIndex::Type anim) {

	return mAnimationGroup.mAnimations.el[anim]->mName.c_tstr();
}


float Skeleton::getAnimationLength(AnimationIndex::Type animIndex) {

	SoftPtr<Animation> anim = mAnimationGroup.mAnimations.el[animIndex];

	return anim->mLength;
}

bool Skeleton::createAnimMap() {

	mAnimMap.destroy();
	mAnimMap.reserve(mAnimationGroup.mAnimations.count);

	for (AnimationIndex::Type i = 0; i < mAnimationGroup.mAnimations.count; ++i) {

		if (!mAnimMap.insert(mAnimationGroup.mAnimations[i]->mName.hash(), i)) {

			return false;
		}
	}

	return true;
}


Bone* Skeleton::getBoneByBlendIndex(BoneIndex::Type blendIndex) {

	Bone* ret = &(mBoneArray.el[blendIndex]);

	if (ret->paletteIndex != blendIndex) {

		return NULL;
	}

	return ret;
}

void Skeleton::updateRecurse(BoneNodeRecurseContext& context) {

	if (mBoneArray.size == 0) {

		return;
	}

	mBoneArray.el[0].boneTreeRecurseUpdate(SceneTransform::kIdentity, context);
/*
#ifdef DSkelBoneUpdateAlgo_Alt 	
	mBoneArray.el[0].sceneTreeUpdate(&context);
#else
	mBoneArray.el[0].sceneTreeUpdateDirtySpots(&context);
#endif
	*/
}	


void Skeleton::setAsStartPose() {

	static BoneNodeRecurseContext startPoseContext(BR_SetStartPose);
	updateRecurse(startPoseContext);
/*
	//mBoneArray.el[0].sceneMarkDirty();
	updateRecurse(startPoseContext);
*/
}


void Skeleton::updateRecurse(SkeletonInstanceBoneBinding* pInstanceBinding) {

	static BoneNodeRecurseContext updateContext(BR_Update);

	if (pInstanceBinding) {

		updateContext.pInstanceBinding = pInstanceBinding;
		pInstanceBinding->dynamicAAB.empty();
	}

	updateRecurse(updateContext);
}




void Skeleton::getInterpolatedKey(Animation* pAnim, float time, 
								  AnimationIndex::Type track, AnimationIndex::Type key, AnimationKey* pKey, Bone** ppBone) {

	*ppBone = &(mBoneArray.el[(pAnim->mTracks.el[track]->boundId)]);

	AnimationIndex::Type key2 = key + 1;
	if (key2 >= pAnim->mTracks.el[track]->mKeys.count) {
		*pKey = (pAnim->mTracks.el[track]->mKeys.el[key]);
		return;
	}

	AnimationKey* pKey1 = &(pAnim->mTracks.el[track]->mKeys.el[key]);
	AnimationKey* pKey2 = &(pAnim->mTracks.el[track]->mKeys.el[key2]);


	float scale;

	scale = (time - pKey1->time) / (pKey2->time - pKey1->time);

	D3DXVECTOR3 t1;
	D3DXVECTOR3 t2;
	D3DXVECTOR3 t;

	//t1 = D3DXVECTOR3(pKey1->trans);
	//t2 = D3DXVECTOR3(pKey2->trans);
	MDXVectFromPtr(t1, pKey1->trans);
	MDXVectFromPtr(t2, pKey2->trans);

	D3DXVec3Lerp(&t, &t1, &t2, scale);

	pKey->time = time;
	pKey->trans[0] = t.x;
	pKey->trans[1] = t.y;
	pKey->trans[2] = t.z;

	D3DXQUATERNION q1;
	D3DXQUATERNION q2;
	D3DXQUATERNION q;
	D3DXQUATERNION qNorm;

	D3DXVECTOR3 v;

	//v = D3DXVECTOR3(pKey1->rotaxis);
	MDXVectFromPtr(v, pKey1->rotaxis);
	D3DXQuaternionRotationAxis(&q1, &v, pKey1->rot);

	//v = D3DXVECTOR3(pKey2->rotaxis);
	MDXVectFromPtr(v, pKey2->rotaxis);
	D3DXQuaternionRotationAxis(&q2, &v, pKey2->rot);

	D3DXQuaternionSlerp(&q, &q1, &q2, scale);

	D3DXQuaternionNormalize(&qNorm, &q);
	D3DXQuaternionToAxisAngle(&qNorm, &v, &pKey->rot);

	pKey->rotaxis[0] = v.x;
	pKey->rotaxis[1] = v.y;
	pKey->rotaxis[2] = v.z;

	pKey->rotaxis.normalize();
}


void Skeleton::stopAnimating() {

	for (BoneIndex::Type i = 0; i < mBoneArray.size; i++) {

		mBoneArray.el[i].setLocalMatrixFromInit();
	}
}

TrackAnimState::TrackAnimState() {

	reset();
}

TrackAnimCache::TrackAnimCache() : animIndex(AnimationIndex::Null) {
}

void TrackAnimState::reset() {

	keyIndex = AnimationIndex::Null;
}

void TrackAnimCache::init(Skeleton& skeleton) {

	mStates.destroy();

	AnimationIndex::Type maxTrackCount = 0;

	for (AnimationIndex::Type i = 0; i < skeleton.mAnimationGroup.mAnimations.count; ++i) {

		maxTrackCount = tmax(maxTrackCount, skeleton.mAnimationGroup.mAnimations.el[i]->mTracks.count);
	}
	
	mStates.reserve(maxTrackCount);
}

bool Skeleton::gatherBlendState(AnimationIndex::Type anim, AnimationIndex::Type trackIndex, float time, TrackAnimCache* pTrackAnimCache, Bone** ppBone, AnimationKey& key) {

	//D3DXVECTOR3 v;
	//Bone* pBone;
	
	AnimationTrack& track = dref(mAnimationGroup.mAnimations[anim]->mTracks[trackIndex]);

	if (track.mKeys.count) {

		AnimationIndex::Type j = AnimationIndex::Null;
		AnimationIndex::Type startSearchIndex = track.mKeys.count - 1;

		if (pTrackAnimCache && pTrackAnimCache->animIndex == anim) {

			AnimationIndex::Type& keyIndex = pTrackAnimCache->mStates[trackIndex].keyIndex;

			if (keyIndex != AnimationIndex::Null) {

				if (track.mKeys[keyIndex].time <= time) {

					if (keyIndex + 1 < track.mKeys.count) {

						if (track.mKeys[keyIndex + 1].time >= time) {

							j = keyIndex;
						}

					} else {

						j = keyIndex;
					}

				} else {

					if (keyIndex + 1 < track.mKeys.count)
						startSearchIndex = keyIndex + 1;
				}
			}
		} 

		if (j == AnimationIndex::Null) {

			bool found = false;
			int phase = 0;

			for (int phase = 0; !found && phase < 2; ++phase) {

				for (j = startSearchIndex; !found && j >= 0 && j != (AnimationIndex::Type) -1; --j) {

					if (track.mKeys[j].time <= time) {

						if (j + 1 < track.mKeys.count) {

							if (track.mKeys[j + 1].time >= time) {

								found = true;
								break;

							} else {

								break;
							}

						} else {

							found = true;
							break;
						}
					}
				}

				if (startSearchIndex != track.mKeys.count - 1) {
					
					startSearchIndex = track.mKeys.count - 1;
				}
			}

			if (found) {

				if (pTrackAnimCache)
					pTrackAnimCache->mStates[trackIndex].keyIndex = j;

			} else {

				return false;
				j = AnimationIndex::Null;
			}
		}

		if (j != AnimationIndex::Null) {

			getInterpolatedKey((mAnimationGroup.mAnimations.el[anim]), time, trackIndex, j, &key, ppBone);
		}
	}
	
	
	return true;
}


void Skeleton::setState(AnimationIndex::Type mainAnim, float time, TrackAnimCache* pMainTrackAnimCache, float mainWeight, unsigned int blendAnimCount, SkeletonBlendAnim* blendAnims, int stride) {

	if (mAnimationGroup.mAnimations.count == 0) {

		return;
	}

	if (mainAnim == AnimationIndex::Null) {

		stopAnimating();

	} else {

		setState(mainAnim, time, pMainTrackAnimCache, mainWeight, true, (blendAnimCount == 0));
		
		if (blendAnimCount > 0) {

			for (unsigned int bi = 0; bi < blendAnimCount; ++bi) {

				SkeletonBlendAnim* blendAnim = (SkeletonBlendAnim*) (((BYTE*) blendAnims) + bi * stride);

				setState(blendAnim->anim, time, NULL, blendAnim->weight, false, false);
			}

			for (BoneIndex::Type i = 0; i < mBoneArray.size; i++) {

				Bone& bone = mBoneArray.el[i];

				bone.mTransformLocal.setOrientation(bone.blendQuaternion);
			}
		}

	}
}

void Skeleton::setState(AnimationIndex::Type anim, float time, TrackAnimCache* pTrackAnimCache, float weight, bool initBones, bool finalizeBones) {

	Bone* pBone;
	
	Quaternion quatAnim;
	Quaternion weightedQuat;
	Quaternion blendQuat;

	AnimationKey key;

	for (AnimationIndex::Type i = 0; i < mAnimationGroup.mAnimations.el[anim]->mTracks.count; i++) {

		if (gatherBlendState(anim, i, time, pTrackAnimCache, &pBone, key)) {

			if (initBones) {

				pBone->mTransformLocal.setPosition(pBone->init.trans);
				pBone->blendQuaternion.fromAxisAngle(pBone->init.rotaxis, pBone->init.rot, true);
			}

			key.trans.mulAndAdd(weight, pBone->mTransformLocal.position());
			
			blendQuat.fromAxisAngle(key.rotaxis, key.rot, true);

			if (weight == 1.0f) {
				
				mul(Quaternion(pBone->blendQuaternion), blendQuat, pBone->blendQuaternion);
		
			} else {

				nlerp(Quaternion::kIdentity, blendQuat, weight, weightedQuat);
				mul(Quaternion(pBone->blendQuaternion), weightedQuat, pBone->blendQuaternion);
			}

			if (finalizeBones) {

				pBone->mTransformLocal.setOrientation(pBone->blendQuaternion);
			}
		}
	}

	if (pTrackAnimCache && pTrackAnimCache->animIndex != anim) {

		pTrackAnimCache->animIndex = anim;
	}
}


void Skeleton::prepareForRenderer(Renderer& renderer, CoordSysConvPool& convPool, bool _setAsStartPose) {

	applyCoordSys(convPool, renderer.getCoordSys(), _setAsStartPose);
}

void Skeleton::applyCoordSys(CoordSysConvPool& convPool, const CoordSys& coordSys, bool _setAsStartPose) {

	SoftRef<CoordSysConv> conv = convPool.getConverterFor(mCoordSys, coordSys, false);

	if (conv.isNull()) {

		return;
	}

	for (BoneIndex::Type i = 0; i < mBoneArray.size; i++) {

		mBoneArray.el[i].applyConverterToInit(conv.dref());
	}

	for (AnimationIndex::Type i = 0; i < mAnimationGroup.mAnimations.count; i++) {
		for (AnimationIndex::Type j = 0; j < mAnimationGroup.mAnimations.el[i]->mTracks.count; j++) {
			for (AnimationIndex::Type k = 0; k < mAnimationGroup.mAnimations.el[i]->mTracks.el[j]->mKeys.count; k++) {

				mAnimationGroup.mAnimations.el[i]->mTracks.el[j]->mKeys.el[k].applyConverter(conv.dref());
			}
		}
	}

	if (_setAsStartPose) {

		setAsStartPose();
	}

	mCoordSys.set(coordSys);
}

void Skeleton::transform(const Matrix4x3Base *pTrans, bool _setAsStartPose) {

	if (pTrans == NULL) {

		return;
	}

	const Matrix4x3Base& transf = *pTrans;

	for (BoneIndex::Type i = 0; i < mBoneArray.size; i++) {

		mBoneArray.el[i].transformInit(transf);
	}

	for (AnimationIndex::Type i = 0; i < mAnimationGroup.mAnimations.count; i++) {
		for (AnimationIndex::Type j = 0; j < mAnimationGroup.mAnimations.el[i]->mTracks.count; j++) {
			for (AnimationIndex::Type k = 0; k < mAnimationGroup.mAnimations.el[i]->mTracks.el[j]->mKeys.count; k++) {

				mAnimationGroup.mAnimations.el[i]->mTracks.el[j]->mKeys.el[k].transform(transf);
			}
		}
	}

	if (_setAsStartPose) {

		setAsStartPose();
	}
}

}