#include "WESkeletonLoader_v1.h"

#include "../helper/load/WECoordSysDataLoadHelper.h"
#include "../helper/load/WEMathDataLoadHelper.h"
#include "../binding/win/includeWindows.h"

namespace WE {

typedef WETL::HashMap<StringHash, WE::Bone*, WETL::IndexShort::Type> Skeleton_BoneNodeByNameMap;

struct Skeleton_LoadContext {

	BufferString tempStr;
	Skeleton* pSkel;
	Skeleton_BoneNodeByNameMap boneNodeByName;
	SoftRef<CoordSysConv> conv;
};



Bone* Skeleton_loadBone(DataChunk& boneChunk, BoneIndex::Type index, Skeleton_LoadContext& ct) {

	StringHash nameHash = boneChunk.scanAttributeHash(ct.tempStr, L"name");
	
	SoftRef<DataChunk> part;
	
	BoneIndex::Type boneIndex = index;
	Bone* pBone = &(ct.pSkel->mBoneArray.el[boneIndex]);
	
	part = boneChunk.getFirstChild();
	MathDataLoadHelper::extract(ct.tempStr, part.dref(), pBone->init.trans, false); 
	
	toNextSibling(part);
	part->scanAttribute(ct.tempStr, L"angle", L"%f", &pBone->init.rot);

	toFirstChild(part);
	MathDataLoadHelper::extract(ct.tempStr, part.dref(), pBone->init.rotaxis, false); 
	pBone->init.rotaxis.normalize();
	
	pBone->paletteIndex = index;
	pBone->setLocalMatrixFromInit();
	pBone->emptyToBoundingBox();

	boneChunk.getAttribute(L"name", pBone->name);

	if (ct.conv.isValid()) {

		pBone->applyConverterToInit(ct.conv.dref());
	}

	ct.boneNodeByName.insert(nameHash, pBone);

	return pBone;
}


void Skeleton_loadBones(DataSourcePool& dataSrcPool, DataChunk& bonesChunk, Skeleton_LoadContext& ct) {

	int intBoneCount = -1;

	intBoneCount = bonesChunk.countChildren(ct.tempStr);

	if (BoneIndex::isValidIndex(intBoneCount) == false) {
	
		assrt(false);
		return;
	}
		
	ct.pSkel->mBoneArray.setSize(safeSizeCast<int, BoneIndex::Type>(intBoneCount));
	ct.pSkel->mBoneMapByGameID.reserve(ct.pSkel->mBoneArray.size);

	SoftRef<DataSource> dataSource = bonesChunk.getSource(); 
	SoftRef<DataChunk> skelPropsChunk = dataSrcPool.getRelativeRemoteChunk(bonesChunk, true, dataSource, L"skelprops.xml/skelprops");
	

	//TODO add dummy root node to enable multiple real root nodes
	//pSkeleton->mBones.init(boneCount + 1);
	//KETreeNode* pRootNode = pSkeleton->mBones.addNode(NULL);
	
	BoneIndex idx = 0;
	SoftRef<DataChunk> boneChunk = bonesChunk.getFirstChild();

	while(boneChunk.isValid()) {

		Bone* pBone = Skeleton_loadBone(boneChunk.dref(), idx, ct);

			if (skelPropsChunk.isValid()) {

			pBone->contributeToVolume = false;
			pBone->enableColl = false;
		}
	
		idx++;
		toNextSibling(boneChunk);
	}

	BufferString tempStr;

	if (skelPropsChunk.isValid()) {

		skelPropsChunk->scanAttribute(tempStr, L"boneBoxScale", L"%f", &ct.pSkel->mBoneBoxScale);

		SoftRef<DataChunk> child = skelPropsChunk->getFirstChild();

		while (child.isValid()) {

			if (child->getAttribute(L"name", tempStr)) {

				SoftPtr<Bone> bone;

				ct.boneNodeByName.find(tempStr.hash(), bone.ptrRef());

				if (bone.isValid()) {

					child->scanAttribute(tempStr, L"contributeToVolume", bone->contributeToVolume);
					child->scanAttribute(tempStr, L"enableColl", bone->enableColl);

					child->getAttribute(L"gameName", bone->gameName);
					bone->gameID = -1;
					child->scanAttribute(tempStr, L"gameID", L"%u", &bone->gameID);

					if (bone->gameID != -1) {

						ct.pSkel->mBoneMapByGameID.insert(bone->gameID, bone);
					}

				} else {

					assrt(false);
					log(LOG_WARN, L"Bone [%s] Not found for skelprop mapping", tempStr.c_tstr());
				}
			}

			toNextSibling(child);
		}
	}
		
	return;
}


void Skeleton_loadBoneHierarchy(DataChunk& hierChunk, Skeleton_LoadContext& ct) {

	StringHash boneHash;
	StringHash parentHash;

	Bone* pBone;
	Bone* pParent;

	SoftRef<DataChunk> link = hierChunk.getFirstChild();
	while(link.isValid()) {

		boneHash = link->scanAttributeHash(ct.tempStr, L"bone");
		parentHash = link->scanAttributeHash(ct.tempStr, L"parent");
		
		if (boneHash != 0 && parentHash != 0) {

			pBone = NULL;
			pParent = NULL;

			ct.boneNodeByName.find(boneHash, pBone);
			ct.boneNodeByName.find(parentHash, pParent);
			
			assrt(pBone != NULL && pParent != NULL);

			pParent->addChild(pBone);
		}

		toNextSibling(link);
	}
}


void Skeleton_loadBoneAnimationTrackKey(DataChunk& keyChunk, AnimationKey* pKey, Skeleton_LoadContext& ct) {

	keyChunk.scanAttribute(ct.tempStr, L"time", L"%f", &pKey->time);
	
	SoftRef<DataChunk> part;
	
	part = keyChunk.getFirstChild();
	MathDataLoadHelper::extract(ct.tempStr, part.dref(), pKey->trans, false); 
	
	toNextSibling(part);
	part->scanAttribute(ct.tempStr, L"angle", L"%f", &pKey->rot);

	toFirstChild(part);
	MathDataLoadHelper::extract(ct.tempStr, part.dref(), pKey->rotaxis, false); 

	if (ct.conv.isValid()) {
	
		pKey->applyConverter(ct.conv.dref());
	}
}

void Skeleton_loadBoneAnimationTrack(DataChunk& trackChunk, AnimationTrack* pTrack, Skeleton_LoadContext& ct) {

	StringHash boneNameHash = trackChunk.scanAttributeHash(ct.tempStr, L"bone");
	
	if (boneNameHash == 0) {

		assrt(false);
		return;
	}

	WE::Bone* pBone = NULL;
	ct.boneNodeByName.find(boneNameHash, pBone);
	
	if (pBone == NULL) {

		log(L"bone [%s] not found", ct.tempStr.c_tstr());
		assrt(false);
		return;
	}

	pTrack->boundId = pBone->paletteIndex;

	SoftRef<DataChunk> keys = trackChunk.getFirstChild();

	int keyCount = keys->countChildren(ct.tempStr);

	if (keyCount <= 0) {

		return;
	}

	pTrack->init(keyCount);
	
	SoftRef<DataChunk> key = keys->getFirstChild();

	while(key.isValid()) {

		Skeleton_loadBoneAnimationTrackKey(key, pTrack->addKey(), ct);

		toNextSibling(key);
	}
}


void Skeleton_loadBoneAnimation(DataChunk& animChunk, Animation* pAnim, Skeleton_LoadContext& ct) {
	
	animChunk.scanAttribute(ct.tempStr, L"length", L"%f", &pAnim->mLength);
	//animChunk.scanAttribute(ct.tempStr, L"speed", L"%f", &pAnim->mSpeed);
	animChunk.scanAttribute(ct.tempStr, L"loop", pAnim->mLoop);
	animChunk.getAttribute(L"name", pAnim->mName);

	SoftRef<DataChunk> tracks = animChunk.getFirstChild();
	
	int trackCount = tracks->countChildren(ct.tempStr);
	
	if (trackCount <= 0) {

		return;
	}

	pAnim->init(trackCount);

	SoftRef<DataChunk> track = tracks->getFirstChild();

	while (track.isValid()) {

		Skeleton_loadBoneAnimationTrack(track, pAnim->addTrack(), ct);

		toNextSibling(track);
	}
	
}

int Skeleton_processBoneExtraAnimations(bool load, DataChunk& animsChunk, Skeleton_LoadContext& ct, DataSourcePool& dataSrcPool) {

	int extraAnimationCount = 0;
	
	BufferString tempStr;
	{
		SoftRef<DataSource> source = animsChunk.getSource();

		if (source.isValid()) {

			BufferString dirPath(source->getSourcePath());

			PathResolver::appendPath(dirPath, L"../anims/", true, false);

			bool isFolder;
			
			if (Win::isValidFile(dirPath.c_tstr(), isFolder) && isFolder) {

				Win::FileIterator iter;
				BufferString fileName;
				bool isFolder;
				String ext(L".xml");

				if (Win::startFileIterate(dirPath.c_tstr(), iter)) {

					do {

						iter.getCurrFileName(fileName, isFolder);

						if (!isFolder && fileName.length() >= 4) {

							if (fileName.findNext(ext, fileName.length() - 4) > 0) {

								fileName.insert(dirPath);

								SoftRef<DataChunk> skelChunk = dataSrcPool.getChunk(fileName.c_tstr(), true, NULL, true);

								if (skelChunk.isValid()) {

									SoftRef<DataChunk> animations = skelChunk->getChild(L"animations");

									
									if (animations.isValid()) {

										extraAnimationCount += animations->countChildren(tempStr, L"animation");

										if (load) {

											SoftRef<DataChunk> anim = animations->getFirstChild();

											while(anim.isValid()) {

												Skeleton_loadBoneAnimation(anim, ct.pSkel->mAnimationGroup.addAnimation(), ct);

												toNextSibling(anim);
											}
										}
									}
								}
							}
						}

					} while(Win::nextFileIterate(iter));
				}
			}
		}
	}

	return extraAnimationCount;
}

void Skeleton_loadBoneAnimations(DataChunk& animsChunk, Skeleton_LoadContext& ct, DataSourcePool& dataSrcPool) {

	int animationCount = animsChunk.countChildren(ct.tempStr);
	int extraAnimationCount = Skeleton_processBoneExtraAnimations(false, animsChunk, ct, dataSrcPool);
	
	BufferString tempStr;
	

	ct.pSkel->mAnimationGroup.init(animationCount + extraAnimationCount);
	
	if (animationCount > 0) {

		SoftRef<DataChunk> anim = animsChunk.getFirstChild();
		while(anim.isValid()) {

			Skeleton_loadBoneAnimation(anim, ct.pSkel->mAnimationGroup.addAnimation(), ct);

			toNextSibling(anim);
		}
	}

	if (extraAnimationCount > 0) {

		Skeleton_processBoneExtraAnimations(true, animsChunk, ct, dataSrcPool);
	}
}


bool SkeletonLoader_v1::load(Skeleton& skeleton, DataSourcePool& dataPool, DataSourceChunk skelChunk, const CoordSys* pTargetSemantics, CoordSysConvPool* pConvPool, bool loadCoordSys) {

	dataPool.resolvePossiblyRemoteChunk(skelChunk, true, NULL);

	if (skelChunk.isNull()) {

		return false;
	}

	skeleton.destroy();

	Skeleton_LoadContext context;
	
	if (loadCoordSys) {

		if (CoordSysDataLoadHelper::extract(context.tempStr, skelChunk.pChunk(), skeleton.mCoordSys, true) == false) {

			//no coordSys found
			assrt(false);
			return false;
		}
	}

	context.pSkel = &skeleton;

	if (pTargetSemantics) {

		context.conv = pConvPool->getConverterFor(skeleton.mCoordSys, *pTargetSemantics, false);
		skeleton.mCoordSys.set(*pTargetSemantics);

	} else {

		context.conv = NULL;
	}


	SoftRef<DataChunk> child = skelChunk->getChild(L"bones");

	if (child.isValid()) {

		Skeleton_loadBones(dataPool, child.dref(), context);
	} 

	if (skeleton.mBoneArray.size == 0) {

		return true;
	}

	toNextSibling(child);

	if (child.isValid()) {
	
		Skeleton_loadBoneHierarchy(child.dref(), context);
	}	

	toNextSibling(child);

	if (child.isValid()) {
	
		Skeleton_loadBoneAnimations(child.dref(), context, dataPool);
	}	

	skeleton.setAsStartPose();
	
	if (!skeleton.createAnimMap()) {

		assrt(false);
		log(LOG_ERROR, L"Skeleton Failed to create Animation map");

		return false;
	}

	return true;
}

}