#include "WESpatialManager_KDTreeBuilder.h"

#include "WESpatialManager_KDTree.h"

#include "../../math/WEMerge.h"
#include "../../helper/load/WEMathDataLoadHelper.h"

namespace WE {

SpatialManager_KDTreeBuilder::SpatialManager_KDTreeBuilder(SpatialManager* pCreator)
	: SpatialManagerBuilder(pCreator)	{

	WE_MMemNew(mContainer.ptrRef(), SpatialObjectContainer());
}

void SpatialManager_KDTreeBuilder::clear() {

	mContainer.destroy();

	mTreeRoot.destroy();
	mGlobalTriBuffer.destroy();
	mGlobalStaticVolumes.destroy();
}

bool SpatialManager_KDTreeBuilder::build(SpatialManager& _targetManager) {

	SpatialManager_KDTree& targetManager = (SpatialManager_KDTree&) _targetManager;

	targetManager.mGlobalTriBuffer = mGlobalTriBuffer.ptr();
	mGlobalTriBuffer.ptrRef() = NULL;

	targetManager.mGlobalStaticVolumes = mGlobalStaticVolumes.ptr();
	mGlobalStaticVolumes.ptrRef() = NULL;

	targetManager.mTreeRoot = mTreeRoot.ptr();
	mTreeRoot.ptrRef() = NULL;

	for (Index i = 0; i < mContainer->mDynVols.count; ++i) {
		
		Vol& vol = dref(mContainer->mDynVols.el[i]);
		targetManager.addDynamicVolume(vol.volume(), vol.binding);
	}

	return true;
}

bool SpatialManager_KDTreeBuilder::process(void* pConfig, bool destroyConfig) {

	SoftPtr<SubdivisionSettings> settings = (SubdivisionSettings*) pConfig;

	mTreeRoot.destroy();
	MMemNew(mTreeRoot.ptrRef(), TreeNode());
	
	mTreeRoot->volume.empty();
	
	if (mContainer.isValid()) {

		WE_MMemNew(mTreeRoot->leaf.ptrRef(), TreeNodeLeaf());
		WE_MMemNew(mGlobalTriBuffer.ptrRef(), TriBufferManager());

		TreeNodeLeaf& completeScene = mTreeRoot->leaf;
		
		for (Index i = 0; i < mContainer->mVols.count; ++i) {
		
			mergeInto(mTreeRoot->volume, mContainer->mVols.el[i]->volume());
		}

		for (Index i = 0; i < mContainer->mTriConts.count; ++i) {
		
			mContainer->mTriConts.el[i]->spatial->addToVol(mTreeRoot->volume);
		}

		completeScene.mStaticTriConts.reserve(mContainer->mTriConts.count);
		mGlobalTriBuffer->reserve(mContainer->mTriConts.count);

		for (Index i = 0; i < mContainer->mTriConts.count; ++i) {
			
			TriCont& triCont = dref(mContainer->mTriConts.el[i]);

			mGlobalTriBuffer->add(triCont.spatial, triCont.binding);
			StaticTriCont* pNewCont = completeScene.mStaticTriConts.addNewOne();

			pNewCont->buffer = &mGlobalTriBuffer->getBuffer(mGlobalTriBuffer->getBufferCount() - 1);

			Index triCount = pNewCont->buffer->getCount();

			pNewCont->mTriIndices.resize(triCount);

			for (Index ti = 0; ti < triCount; ++ti) {
				
				pNewCont->mTriIndices.el[ti] = ti;
			}
		}

		completeScene.updateStaticTriCount();

		mTreeRoot->split(settings.dref(), completeScene.getStaticTriCount(), 0, true);

		WE_MMemNew(mGlobalStaticVolumes.ptrRef(), StaticVolManager());

		mGlobalStaticVolumes->reserve(mContainer->mVols.count);

		for (Index i = 0; i < mContainer->mVols.count; ++i) {
		
			Vol& vol = dref(mContainer->mVols.el[i]);

			SoftPtr<StaticVolume> newVol = mGlobalStaticVolumes->addNewOne();

			newVol->binding = vol.binding;
			newVol->volume().init(vol.volume());

			mTreeRoot->recurseAdd(newVol);
		}
	}

	if (destroyConfig)
		destroyConfiguration(pConfig);

	return true;
}

void SpatialManager_KDTreeBuilder::render(Renderer& renderer) {

	if (mTreeRoot.isValid()) {

		mTreeRoot->render(renderer);
	}
}

void* SpatialManager_KDTreeBuilder::loadConfiguration(BufferString& tempStr, DataChunk* pChunk, CoordSysConv* pConv) {

	SoftPtr<SubdivisionSettings> settings;

	if (pChunk) {

		WE_MMemNew(settings.ptrRef(), SubdivisionSettings());

		if (MathDataLoadHelper::extract(tempStr, *pChunk, settings->minCellSize, true, true, true, L"minCellSize")) {

			if (pConv) {
				pConv->toTargetPoint(settings->minCellSize.el);
			}
		}
			
		pChunk->scanAttribute(tempStr, L"maxDepth", L"%u", &settings->maxDepth);
		pChunk->scanAttribute(tempStr, L"maxCellElementCount", L"%u", &settings->maxCellElementCount);
		pChunk->scanAttribute(tempStr, L"freeSpaceLoad", L"%f", &settings->freeSpaceLoad);
		pChunk->scanAttribute(tempStr, L"largeVolumeCoeff", L"%f", &settings->largeVolumeCoeff);
	}

	return settings.ptr();
}

void SpatialManager_KDTreeBuilder::destroyConfiguration(void*& pConfig) {

	HardPtr<SubdivisionSettings> settings = (SubdivisionSettings*) pConfig;

	settings.destroy();
	pConfig = NULL;
}

bool SpatialManager_KDTreeBuilder::add(SpatialTriContainer* pSpatial, ObjectBinding* pBinding, bool destroyWhenDone) {

	if (mContainer.isValid()) {

		mContainer->add(pSpatial, pBinding, destroyWhenDone);

		return true;
	}

	return false;
}

bool SpatialManager_KDTreeBuilder::add(VolumeStruct& spatial, SpatialVolumeBinding* pBinding) {

	if (mContainer.isValid()) {

		mContainer->add(spatial, pBinding);

		return true;
	}

	return false;
}

bool SpatialManager_KDTreeBuilder::addDynamicVolume(VolumeStruct& spatial, SpatialVolumeBinding* pBinding) {

	if (mContainer.isValid()) {

		mContainer->addDynamicVolume(spatial, pBinding);

		return true;
	}

	return false;
}


}