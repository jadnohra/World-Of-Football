#include "WESpatialManager_KDTree.h"

#include "../../WEDebugBypass.h"
#include "../../math/WEMerge.h"

namespace WE {

const float VolumeScale = 1000.0f;

SpatialManagerBuilder_KDTree::SpatialManagerBuilder_KDTree(SpatialManager* pCreator)
	: SpatialManagerBuilder(pCreator)	{

	WE_MMemNew(mTriObjects.ptrRef(), SpatialTriObjects());
	WE_MMemNew(mVolumes.ptrRef(), SpatialVolumes());
}

void SpatialManagerBuilder_KDTree::clear() {

	mTriObjects->destroy();
	mVolumes->destroy();

	mTreeRoot.destroy();
}

bool SpatialManagerBuilder_KDTree::process(const SpatialManagerBuilder::SubdivisionSettings& settings) {

	mTreeRoot.destroy();
	MMemNew(mTreeRoot.ptrRef(), TreeNode());
	
	mTreeRoot->volume.empty();
	
	{
		WE_MMemNew(mTreeRoot->leaf.ptrRef(), TreeNodeLeaf());

		TreeNodeLeaf& completeScene = mTreeRoot->leaf;
		
		for (SpatialVolumes::Index i = 0; i < mVolumes->size; ++i) {
		
			mergeInto(mTreeRoot->volume, mVolumes->el[i]->spatialVolume());
		}

		SpatialProcessor::PtrArray::addToVol_Tri1(mTreeRoot->volume, mTriObjects.dref(), mTriObjects->size);

		completeScene.mTriObjects.reserve(mTriObjects->size);

		for (SpatialTriObjects::Index i = 0; i < mTriObjects->size; ++i) {
			
			WE_MMemNew(completeScene.mTriObjects.el[i], SpatialTriContainer::Partial());

			SoftPtr<AbstractContainer::ElementAddressBatch> batch = mTriObjects->el[i]->createElementAddressBatch();
			batch->initWith(dref(mTriObjects->el[i]));

			completeScene.mTriObjects.el[i]->container = mTriObjects->el[i];
			completeScene.mTriObjects.el[i]->batch = batch;
		}

		completeScene.mTriObjects.updateElementCount();

		mTreeRoot->split(settings, completeScene.mTriObjects.elementCount, 0, true);

		for (SpatialVolumes::Index i = 0; i < mVolumes->size; ++i) {
		
			mTreeRoot->recurseAdd(dref(mVolumes->el[i]));
		}
	}
	

	return true;
}

void SpatialManager_KDTreeBase::TreeNodeLeaf::add(SpatialVolume& volume) {

	mVolumes.addOne(&volume);
}

void SpatialManager_KDTreeBase::TreeNode::recurseAdd(SpatialVolume& volume) {

	if (branch.isValid()) {

		switch (volume.spatialVolume().classify(branch->plane)) {

			case PP_Back: {

				branch->child[PP_Back].recurseAdd(volume);
				
			} break;

			case PP_Front: {

				branch->child[PP_Front].recurseAdd(volume);
				
			} break;

			case PP_BackAndFront: {

				branch->child[PP_Back].recurseAdd(volume);
				branch->child[PP_Front].recurseAdd(volume);
				
			} break;
		}

	} else if (leaf.isValid()) {

		leaf->add(volume);
	}
}

void SpatialManager_KDTreeBase::TreeNode::split(const SpatialManagerBuilder::SubdivisionSettings& settings, const Index& totalCount, Index depth, bool recurse, Index inSharedElementCount, float volumeScale) {

	branch.destroy();

	TreeNodeLeaf& nodeContents = leaf;
	
	bool doSplit = false;

	if ((nodeContents.mTriObjects.elementCount - inSharedElementCount) <= settings.maxCellElementCount 
		|| depth >= settings.maxDepth) {

		doSplit = false;

	} else {

		AAPlane splitPlane;
		Index sharedElementCount = 0;

		doSplit = findSplittingPlane(settings, totalCount, splitPlane, volumeScale);

		if (doSplit) {

			WE_MMemNew(branch.ptrRef(), TreeNodeBranch());

			branch->plane = splitPlane;

			WE_MMemNew(branch->child[PP_Back].leaf.ptrRef(), TreeNodeLeaf());
			WE_MMemNew(branch->child[PP_Front].leaf.ptrRef(), TreeNodeLeaf());

			assrt (!splitPlane.negative);
			
			branch->child[PP_Back].volume = volume;
			branch->child[PP_Front].volume = volume;
			branch->child[PP_Back].volume.max.el[splitPlane.normalDim] = splitPlane.position.el[splitPlane.normalDim];
			branch->child[PP_Front].volume.min.el[splitPlane.normalDim] = splitPlane.position.el[splitPlane.normalDim];

			float backVolCoeff = branch->child[PP_Back].volume.size(splitPlane.normalDim) / volume.size(splitPlane.normalDim);

			SpatialProcessor::PlaneTriSplit split(&branch->plane, true);

			SpatialProcessor::PartialPtrArray::split_Tri1(split, nodeContents.mTriObjects, nodeContents.mTriObjects.size, 
															branch->child[PP_Back].leaf->mTriObjects, 
															branch->child[PP_Front].leaf->mTriObjects);


			leaf.destroy();

			sharedElementCount = split.resultSummary.count[PP_BackAndFront];

			if (recurse) {

				branch->child[PP_Back].split(settings, totalCount, depth + 1, recurse, sharedElementCount, volumeScale * backVolCoeff);
				branch->child[PP_Front].split(settings, totalCount, depth + 1, recurse, sharedElementCount, volumeScale * (1.0f - backVolCoeff));
			}

			return;
		}

	}

	if (!doSplit) {

		leaf->updateBoundingVolume();
				
	} else {

		assrt(false);
	}
}

void SpatialManager_KDTreeBase::TreeNodeLeaf::updateBoundingVolume() {

	SpatialProcessor::PartialPtrArray::addToVol_Tri1(boundingVolume, mTriObjects, mTriObjects.size);
}

//if almost equal index1 will be preferred
int selectSplittingPlane(const SpatialManagerBuilder::SubdivisionSettings& settings, const SpatialManagerBuilder::Index& totalCount, 
							const int& index1, const AAPlane& plane1, const SpatialProcessor::PlaneTriClassifyCount::Count& count1,
							const int& index2, const AAPlane& plane2, const SpatialProcessor::PlaneTriClassifyCount::Count& count2,
							const float& volumeScale, float splitCoeffsBack1, float splitCoeffsBack2) {

	
	float shareLoadDiff;

	Index allCount = count1.count[PP_Back] + count1.count[PP_Front] + count1.count[PP_BackAndFront];
	shareLoadDiff = (float) ((float) count1.count[PP_BackAndFront] - (float) count2.count[PP_BackAndFront]) / (float) allCount;

	//if (count1.count[PP_BackAndFront] == count2.count[PP_BackAndFront]) {
	if (fabs(shareLoadDiff) <= 0.3f) {

		{
			float splitCoeffs[3];
			
			splitCoeffs[PP_Back] = splitCoeffsBack1;
			splitCoeffs[PP_Front] = 1.0f - splitCoeffsBack1;

			{
				int splitIndex = PP_Back;

				float load = (float) count1.count[splitIndex] / (float) totalCount;

				if (load <= settings.freeSpaceLoad) {

					bool isLargeVolume = splitCoeffs[splitIndex] * volumeScale >= settings.largeVolumeCoeff * VolumeScale;

					if (isLargeVolume) {

						return index1;					
					}
				}
			}
		}

		{
			float splitCoeffs[3];
			
			splitCoeffs[PP_Back] = splitCoeffsBack2;
			splitCoeffs[PP_Front] = 1.0f - splitCoeffsBack2;

			{
				int splitIndex = PP_Back;

				float load = (float) count2.count[splitIndex] / (float) allCount;

				if (load <= settings.freeSpaceLoad) {

					bool isLargeVolume = splitCoeffs[splitIndex] * volumeScale >= settings.largeVolumeCoeff * VolumeScale;

					if (isLargeVolume) {

						return index2;					
					}
				}
			}
		}

		/*
		if ((2 * allCount) > (settings.maxCellElementCount * settings.splitPreferenceFactor)) {
		
			//prefer empty space
			
			Index min1 = MMin(count1.count[PP_Back], count1.count[PP_Front]);
			Index min2 = MMin(count2.count[PP_Back], count2.count[PP_Front]);

			return min1 <= min2 ? index1 : index2;
		} 
		*/


		//prefer even split

		float spl[2];

		spl[0] = (float) count1.count[PP_Back] / (float) count1.count[PP_Front];
		spl[1] = (float) count2.count[PP_Back] / (float) count2.count[PP_Front];

		return fabs(1.0f - spl[0]) <= fabs(1.0f - spl[1]) ? index1 : index2;
	}

	return count1.count[PP_BackAndFront] < count2.count[PP_BackAndFront] ? index1 : index2;
}

int selectSplittingPlane(const SpatialManagerBuilder::SubdivisionSettings& settings, const SpatialManagerBuilder::Index& totalCount, 
							const AAPlane* planes, 
							const SpatialProcessor::PlaneTriClassifyCount::Count* counts,
							int count, float volumeScale, float* splitCoeffsBack) {

	int selectIndex = 0;

	for (int i = 1; i < count; ++i) {

		selectIndex = selectSplittingPlane(settings, totalCount,
											selectIndex, planes[selectIndex], counts[selectIndex],
											i, planes[i], counts[i], volumeScale, splitCoeffsBack[selectIndex], splitCoeffsBack[i]);
	}

	return selectIndex;
}

//plane must be already initialized, count will be calculated
bool SpatialManager_KDTreeBase::TreeNode::optimizeSplittingPlane(const SpatialManagerBuilder::SubdivisionSettings& settings, const Index& totalCount, 
																		const int& dim, const Vector3& normal, const Span& searchSpan, 
																		AAPlane& plane, SpatialProcessor::PlaneTriClassifyCount::Count& count, float& outSplitCoeffBack, 
																		bool allowSmallResidualCells, 
																		const float& volumeScale, const float& volumeSplitCoeffBack, float* pGranulatity) {
	
	float granularity = pGranulatity ? *pGranulatity : (searchSpan.max - searchSpan.min) / 4.0f;
	
	if (8.0f * (granularity) < settings.minCellSize.el[dim]) {

		if (allowSmallResidualCells) {

			SpatialProcessor::PlaneTriClassifyCount classif(1, &plane, &count);
			SpatialProcessor::PartialPtrArray::classify_Tri1(classif, leaf->mTriObjects, leaf->mTriObjects.size);

			outSplitCoeffBack = volumeSplitCoeffBack;

			return true;

		} 
		
		return false;
	} 

	const int testCount = 3;

	AAPlane planes[testCount];
	SpatialProcessor::PlaneTriClassifyCount::Count counts[testCount];
	float splitCoeffsBack[3];

	planes[0] = plane;
	splitCoeffsBack[0] = volumeSplitCoeffBack;

	float offset = granularity;
	float voumeSplitCoeffOffset = (offset / (searchSpan.max - searchSpan.min));
	Point testPoint;
	
	testPoint = plane.position;
	testPoint.el[dim] -= offset;
	splitCoeffsBack[1] = volumeSplitCoeffBack - voumeSplitCoeffOffset;

	planes[1].init(testPoint, normal, true);

	testPoint = plane.position;
	testPoint.el[dim] += offset;
	splitCoeffsBack[2] = volumeSplitCoeffBack + voumeSplitCoeffOffset;

	planes[2].init(testPoint, normal, true);


	SpatialProcessor::PlaneTriClassifyCount classif(testCount, planes, counts);
	SpatialProcessor::PartialPtrArray::classify_Tri1(classif, leaf->mTriObjects, leaf->mTriObjects.size);

	int selectIndex = selectSplittingPlane(settings, totalCount, planes, counts, testCount, volumeScale, splitCoeffsBack);

	if (selectIndex == 0) {

		bool allEqual = true;

		for (int i = 1; i < testCount; ++i) {

			if (!counts[i].equals(counts[0])) {

				allEqual = false;
				break;
			}
		}

		if (allEqual) {

			//choose one closest to the most elementCount leaf
			selectIndex = counts[0].count[PP_Back] > counts[0].count[PP_Front] ? 1 : 2;
		}
	}

	if (selectIndex != 0) {

		Span newSpan;

		/*
		if (selectIndex == 1) {

			newSpan.min = searchSpan.min;
			newSpan.max = plane.position.el[dim];

		} else {

			newSpan.min = plane.position.el[dim];
			newSpan.max = searchSpan.max;
		}
		*/

		newSpan = searchSpan;
		float newGranularity = granularity * 0.5f;

		optimizeSplittingPlane(settings, totalCount, dim, normal, newSpan, planes[selectIndex], counts[selectIndex], outSplitCoeffBack, false, volumeScale, splitCoeffsBack[selectIndex], &newGranularity);

		plane = planes[selectIndex];
		count = counts[selectIndex];

		//outSplitCoeffBack = splitCoeffsBack[selectIndex];

		return true;
	}

	count = counts[0];
	return true;
}

bool SpatialManager_KDTreeBase::TreeNode::findSplittingPlane(const SpatialManagerBuilder_KDTree::SubdivisionSettings& settings, const Index& totalCount, AAPlane& plane, float volumeScale) {

	const int dimCount = 3;

	AAPlane planes[dimCount];
	SpatialProcessor::PlaneTriClassifyCount::Count counts[dimCount];
	bool acceptDim[dimCount];
	bool acceptedDimCount = 0;
	float splitCoeffBack[dimCount];

	for (int dim = 0; dim < dimCount; ++dim) {

		Vector3 normal(0.0f);
		normal.el[dim] = 1.0f;
		Point testPoint;

		assrt(settings.minCellSize.el[dim] != 0.0f);

		float dimSize = volume.size(dim);
		float splitCount = dimSize / settings.minCellSize.el[dim];

		if (splitCount > 1.0f) {

			Span searchSpan;

			searchSpan.min = volume.min.el[dim] + settings.minCellSize.el[dim];
			searchSpan.max = volume.max.el[dim] - settings.minCellSize.el[dim];

			volume.center(testPoint);
			planes[dim].init(testPoint, normal, true);
			
			acceptDim[dim] = optimizeSplittingPlane(settings, totalCount, dim, normal, searchSpan, planes[dim], counts[dim], splitCoeffBack[dim], false, volumeScale, 0.5f);

		} else {

			acceptDim[dim] = false;
		}

		if (acceptDim[dim])
			++acceptedDimCount;
	}

	if (acceptedDimCount == 0)
		return false;

	int selectIndex = -1;

	for (int dim = 0; dim < dimCount; ++dim) {

		if (acceptDim[dim]) {

			if (selectIndex == -1) {

				selectIndex = dim;

			} else {

				selectIndex = selectSplittingPlane(settings, totalCount, selectIndex, planes[selectIndex], counts[selectIndex],
													dim, planes[dim], counts[dim], volumeScale, splitCoeffBack[selectIndex], splitCoeffBack[dim]);
			}
		}
	}

	plane = planes[selectIndex];

	return true;
}

void SpatialManager_KDTreeBase::TreeNodeLeaf::cloneTo(TreeNodeLeaf& target, const bool& includeVolumes) {

	mTriObjects.cloneTo(target.mTriObjects);
	
	if (includeVolumes) {

		if (mVolumes.size) {

			target.mVolumes.reserve(mVolumes.size);

			for (SpatialVolumes::Index i = 0; i < mVolumes.size; ++i) {

				target.mVolumes.el[i] = mVolumes.el[i];
			}

		} else {

			target.mVolumes.destroy();
		}
	}
}

bool SpatialManagerBuilder_KDTree::add(SpatialTriContainer& object) {

	mTriObjects->addOne(&object);

	return true;
}

bool SpatialManagerBuilder_KDTree::add(SpatialVolume& object) {

	mVolumes->addOne(&object);

	return true;
}

bool SpatialManagerBuilder_KDTree::build(SpatialManager& _targetManager) {

	SpatialManager_KDTree& targetManager = (SpatialManager_KDTree&) _targetManager;

	targetManager.mTriObjects = mTriObjects.ptr();
	mTriObjects.ptrRef() = NULL;

	targetManager.mVolumes = mVolumes.ptr();
	mVolumes.ptrRef() = NULL;

	targetManager.mTreeRoot = mTreeRoot.ptr();
	mTreeRoot.ptrRef() = NULL;

	return true;
}

bool SpatialManagerBuilder_KDTree::isCacheValid() {

	return false;
}

bool SpatialManagerBuilder_KDTree::loadFromCache() {

	return false;
}

bool SpatialManagerBuilder_KDTree::saveToCache() {

	return false;
}


void SpatialManagerBuilder_KDTree::render(Renderer& renderer, 
								bool renderMain, const RenderColor* pMainColor, 
								bool renderContainers, const RenderColor* pContainersColor, 
								bool renderDetails, const RenderColor* pDetailColor) {

	if (mTreeRoot.isValid()) {

		mTreeRoot->render(renderer, renderMain, pMainColor, 
							renderContainers, pContainersColor, 
							renderDetails, pDetailColor, true);
	}
}

void SpatialManager_KDTreeBase::TreeNodeLeaf::render(Renderer& renderer, 
														bool renderContainers, const RenderColor* pContainersColor, 
														bool renderDetails, const RenderColor* pDetailColor) {

	if (renderDetails) {

		SpatialProcessor::Render render(&renderer, pDetailColor, true);

		SpatialProcessor::PartialPtrArray::render_Tri1(render, mTriObjects, mTriObjects.size);

		/*
		{
			DebugBypass* pBypass = getDebugBypass();

			if (pBypass->currTri == pBypass->renderTri) {

				SpatialProcessor::Render render(&renderer, &RenderColor::kRed, true);

				SpatialProcessor::PartialPtrArray::render_Tri1(render, mTriObjects, mTriObjects.size);
			}

			++pBypass->currTri;
		}
		*/
	}
}

void SpatialManager_KDTree::TreeNode::render(Renderer& renderer, bool renderMain, const RenderColor* pMainColor, 
													bool renderContainers, const RenderColor* pContainersColor, 
													bool renderDetails, const RenderColor* pDetailColor, bool recurse) {

	bool draw = false;

	if (leaf.isValid() && leaf->mDynamicVolumes.count) {

		renderer.draw(volume, &Matrix4x3Base::kIdentity, &RenderColor::kMagenta, true);
		draw = true;
	} 

	draw = true;
	if (draw && renderMain && leaf.isValid()) {

		renderer.draw(volume, &Matrix4x3Base::kIdentity, pMainColor, true);
	}

	/*
	{
		DebugBypass* pBypass = getDebugBypass();

		if (draw && renderMain && leaf.isValid()) {

			if (pBypass->currTri == pBypass->renderTri) {

				renderer.draw(volume, &Matrix4x3Base::kIdentity, &RenderColor::kRed, true);
				renderDetails = true;
			}

			++pBypass->currTri;
		}
	}
	*/

	if (recurse && branch.isValid()) {

		branch->child[0].render(renderer, renderMain, pMainColor, renderContainers, pContainersColor, renderDetails, pDetailColor, recurse);
		branch->child[1].render(renderer, renderMain, pMainColor, renderContainers, pContainersColor, renderDetails, pDetailColor, recurse);
	}

	if (draw && leaf.isValid()) {
	
		leaf->render(renderer, renderContainers, pContainersColor, renderDetails, pDetailColor);
	}
}

}