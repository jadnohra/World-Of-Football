#include "WESpatialManager_KDTreeBase.h"

#include "WESpatialObjectGroup_KDTree.h"
#include "../../math/WEEnglobe.h"

namespace WE {

const float SpatialManager_KDTreeBase::VolumeScale = 1000.0f;

const float VolumeScale = SpatialManager_KDTreeBase::VolumeScale;

SpatialManager_KDTreeBase::PlaneClassifyCount::PlaneClassifyCount() {

	count[0] = 0;
	count[1] = 0;
	count[2] = 0;
}

bool SpatialManager_KDTreeBase::PlaneClassifyCount::equals(const PlaneClassifyCount& comp) const {

	return count[0] == comp.count[0] && count[1] == comp.count[1] && count[2] == comp.count[2];
}

SpatialManager_KDTreeBase::SubdivisionSettings::SubdivisionSettings(const Vector3* pMinCellSize, 
								const UINT32* pMaxDepth, const Index* pMaxCellElementCount,
								const Index* pSplitPreferenceFactor, const float* pFreeSpaceLoad,
								const float* pLargeVolumeCoeff) {

	if (pMinCellSize) {

		minCellSize = *pMinCellSize;

	} else {

		minCellSize.zero();
	}

	if (pMaxDepth) {

		maxDepth = *pMaxDepth;

	} else {

		maxDepth = UINT32_MAX;
	}

	if (pMaxCellElementCount) {

		maxCellElementCount = *pMaxCellElementCount;

	} else {

		maxCellElementCount = (Index) -1;
	}

	if (pSplitPreferenceFactor) {

		splitPreferenceFactor = *pSplitPreferenceFactor;

	} else {

		splitPreferenceFactor = 1;
	}

	if (pFreeSpaceLoad) {

		freeSpaceLoad = *pFreeSpaceLoad;

	} else {

		freeSpaceLoad = 0.0f;
	}

	if (pLargeVolumeCoeff) {

		largeVolumeCoeff = *pLargeVolumeCoeff;

	} else {

		largeVolumeCoeff = 0.2f;
	}
}

SpatialManager_KDTreeBase::Cell::Cell() : mStaticTriCount(0) {
}

SpatialManager_KDTreeBase::Index SpatialManager_KDTreeBase::Cell::getStaticTriCount() {

	return mStaticTriCount;
}

SpatialManager_KDTreeBase::Index SpatialManager_KDTreeBase::Cell::updateStaticTriCount() {

	mStaticTriCount = 0;

	for (StaticTriConts::Index i = 0; i < mStaticTriConts.count; ++i) {

		mStaticTriCount += mStaticTriConts.el[i]->mTriIndices.size;
	}

	return mStaticTriCount;
}

void SpatialManager_KDTreeBase::Cell::staticTriSplit(const AAPlane& plane, Cell& back, Cell& front, PlaneClassifyCount& splitCount) {

	for (StaticTriConts::Index i = 0; i < mStaticTriConts.count; ++i) {

		StaticTriCont& triCont = dref(mStaticTriConts.el[i]);

		TriBuffer& buffer = triCont.buffer;
		SpatialIndices& indices = triCont.mTriIndices;

		Index backTriCount = 0;
		Index frontTriCount = 0;

		for (Index ii = 0; ii < indices.size; ++ii) {
			
			switch(plane.classify(buffer.get(indices.el[ii]))) {

				case PP_Back: {

					++backTriCount;
					++splitCount.count[PP_Back];

				} break;

				case PP_Front: {

					++frontTriCount;
					++splitCount.count[PP_Front];

				} break;

				case PP_BackAndFront: {

					++backTriCount;
					++frontTriCount;
					++splitCount.count[PP_BackAndFront];

				} break;
			}
		}

		StaticTriCont* pBackTriCont = NULL;
		StaticTriCont* pFrontTriCont = NULL;

		if (backTriCount) {
			
			pBackTriCont = back.mStaticTriConts.addNewOne();

			pBackTriCont->buffer = &buffer;
			pBackTriCont->mTriIndices.resize(backTriCount);
		}

		if (frontTriCount) {
			
			pFrontTriCont = front.mStaticTriConts.addNewOne();

			pFrontTriCont->buffer = &buffer;
			pFrontTriCont->mTriIndices.resize(frontTriCount);
		}

		Index backTriIndex = 0;
		Index frontTriIndex = 0;

		for (Index ii = 0; ii < indices.size; ++ii) {
			
			switch(plane.classify(buffer.get(indices.el[ii]))) {

				case PP_Back: {

					pBackTriCont->mTriIndices.el[backTriIndex++] = indices.el[ii];
					
				} break;

				case PP_Front: {

					pFrontTriCont->mTriIndices.el[frontTriIndex++] = indices.el[ii];

				} break;

				case PP_BackAndFront: {

					pBackTriCont->mTriIndices.el[backTriIndex++] = indices.el[ii];
					pFrontTriCont->mTriIndices.el[frontTriIndex++] = indices.el[ii];

				} break;
			}
		}

		back.updateStaticTriCount();
		front.updateStaticTriCount();
	}
}

void SpatialManager_KDTreeBase::Cell::staticTriClassify(const AAPlane* planes, PlaneClassifyCount* splitCounts, int planeCount) {

	for (int pi = 0; pi < planeCount; ++pi) {

		const AAPlane& plane = planes[pi];
		PlaneClassifyCount& splitCount = splitCounts[pi];

		for (StaticTriConts::Index i = 0; i < mStaticTriConts.count; ++i) {

			StaticTriCont& triCont = dref(mStaticTriConts.el[i]);

			TriBuffer& buffer = triCont.buffer;
			SpatialIndices& indices = triCont.mTriIndices;

			for (Index ii = 0; ii < indices.size; ++ii) {
				
				++splitCount.count[plane.classify(buffer.get(indices.el[ii]))];
			}
		}
	}
}

void SpatialManager_KDTreeBase::Cell::staticTriAddToVol(AAB& vol) {

	for (StaticTriConts::Index i = 0; i < mStaticTriConts.count; ++i) {

		StaticTriCont& triCont = dref(mStaticTriConts.el[i]);

		TriBuffer& buffer = triCont.buffer;
		SpatialIndices& indices = triCont.mTriIndices;

		for (Index ii = 0; ii < indices.size; ++ii) {
			
			buffer.get(indices.el[ii]).addTo(vol);
		}
	}
}

void SpatialManager_KDTreeBase::TreeNodeLeaf::updateBoundingVolume() {

	staticTriAddToVol(staticTriBoundingVolume);
}

void SpatialManager_KDTreeBase::TreeNodeLeaf::add(StaticVolume* pVolume) {

	mStaticVols.addOne(pVolume);
}

void SpatialManager_KDTreeBase::TreeNode::recurseAdd(StaticVolume* pVolume) {

	if (branch.isValid()) {

		switch (pVolume->volume().classify(branch->plane)) {

			case PP_Back: {

				branch->child[PP_Back].recurseAdd(pVolume);
				
			} break;

			case PP_Front: {

				branch->child[PP_Front].recurseAdd(pVolume);
				
			} break;

			case PP_BackAndFront: {

				branch->child[PP_Back].recurseAdd(pVolume);
				branch->child[PP_Front].recurseAdd(pVolume);
				
			} break;
		}

	} else if (leaf.isValid()) {

		leaf->add(pVolume);
	}
}

void SpatialManager_KDTreeBase::TreeNode::split(const SubdivisionSettings& settings, const Index& totalCount, Index depth, bool recurse, Index inSharedElementCount, float volumeScale) {

	branch.destroy();

	TreeNodeLeaf& nodeContents = leaf;
	
	bool doSplit = false;

	if ((nodeContents.getStaticTriCount() - inSharedElementCount) <= settings.maxCellElementCount 
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

			/*
			SpatialProcessor::PlaneTriSplit split(&branch->plane, true);

			SpatialProcessor::PartialPtrArray::split_Tri1(split, nodeContents.mTriObjects, nodeContents.mTriObjects.size, 
															branch->child[PP_Back].leaf->mTriObjects, 
															branch->child[PP_Front].leaf->mTriObjects);

			*/

			PlaneClassifyCount splitCounts;

			nodeContents.staticTriSplit(branch->plane, branch->child[PP_Back].leaf, branch->child[PP_Front].leaf, splitCounts);
			leaf.destroy();

			sharedElementCount = splitCounts.count[PP_BackAndFront];

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

/*
void SpatialManager_KDTreeBase::TreeNodeLeaf::updateBoundingVolume() {

	SpatialProcessor::PartialPtrArray::addToVol_Tri1(boundingVolume, mTriObjects, mTriObjects.size);
}
*/

typedef SpatialManager_KDTreeBase::SubdivisionSettings SubdivisionSettings;
typedef SpatialManager_KDTreeBase::PlaneClassifyCount PlaneClassifyCount;

//if almost equal index1 will be preferred
int selectSplittingPlane(const SubdivisionSettings& settings, const SpatialIndex& totalCount, 
							const int& index1, const AAPlane& plane1, const PlaneClassifyCount& count1,
							const int& index2, const AAPlane& plane2, const PlaneClassifyCount& count2,
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

int selectSplittingPlane(const SubdivisionSettings& settings, const SpatialIndex& totalCount, 
							const AAPlane* planes, 
							const PlaneClassifyCount* counts,
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
bool SpatialManager_KDTreeBase::TreeNode::optimizeSplittingPlane(const SubdivisionSettings& settings, const Index& totalCount, 
																		const int& dim, const Vector3& normal, const Span& searchSpan, 
																		AAPlane& plane, PlaneClassifyCount& count, float& outSplitCoeffBack, 
																		bool allowSmallResidualCells, 
																		const float& volumeScale, const float& volumeSplitCoeffBack, float* pGranulatity) {
	
	float granularity = pGranulatity ? *pGranulatity : (searchSpan.max - searchSpan.min) / 4.0f;
	
	if (8.0f * (granularity) < settings.minCellSize.el[dim]) {

		if (allowSmallResidualCells) {

			leaf->staticTriClassify(&plane, &count);
			//SpatialProcessor::PlaneTriClassifyCount classif(1, &plane, &count);
			//SpatialProcessor::PartialPtrArray::classify_Tri1(classif, leaf->mTriObjects, leaf->mTriObjects.size);

			outSplitCoeffBack = volumeSplitCoeffBack;

			return true;

		} 
		
		return false;
	} 

	const int testCount = 3;

	AAPlane planes[testCount];
	PlaneClassifyCount counts[testCount];
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


	//SpatialProcessor::PlaneTriClassifyCount classif(testCount, planes, counts);
	//SpatialProcessor::PartialPtrArray::classify_Tri1(classif, leaf->mTriObjects, leaf->mTriObjects.size);
	leaf->staticTriClassify(planes, counts, testCount);

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

bool SpatialManager_KDTreeBase::TreeNode::findSplittingPlane(const SubdivisionSettings& settings, const Index& totalCount, AAPlane& plane, float volumeScale) {

	const int dimCount = 3;

	AAPlane planes[dimCount];
	PlaneClassifyCount counts[dimCount];
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

/*
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
*/

void SpatialManager_KDTreeBase::DynamicVolume::appendToQuery(SpatialObjectGroup_KDTree& query) {

	for (NodePtrs::Index i = 0; i < nodes.count; ++i) {

		nodes.el[i]->leaf->appendToQuery(query, this);
	}
}

void SpatialManager_KDTreeBase::DynamicVolume::clear(SpatialDynamicListener* pListener, void* pListenerContext) {

	for (NodePtrs::Index i = 0; i < nodes.count; ++i) {

		nodes.el[i]->leaf->onClear(this);
	}

	nodes.count = 0;

	/*
	if (pListener)
		pListener->spatialClearCells(object, pListenerContext);
		*/
	//object.spatialClearCells(pListenerContext);
}

bool SpatialManager_KDTreeBase::TreeNode::englobes(const VolumeStruct& testVolume) {

	return WE::englobes(volume, testVolume);
}

void SpatialManager_KDTreeBase::TreeNode::recurseAdd(SpatialDynamicListener* pListener, DynamicVolume* pObject, const VolumeStruct& volume, void* pListenerContext) {

	if (branch.isValid()) {

		switch (volume.classify(branch->plane)) {

			case PP_Back: {

				branch->child[PP_Back].recurseAdd(pListener, pObject, volume, pListenerContext);

			} break;

			case PP_Front: {

				branch->child[PP_Front].recurseAdd(pListener, pObject, volume, pListenerContext);

			} break;

			default: {

				branch->child[PP_Back].recurseAdd(pListener, pObject, volume, pListenerContext);
				branch->child[PP_Front].recurseAdd(pListener, pObject, volume, pListenerContext);
			}
		}

	} else {

		leaf->add(this, pListener, pObject, pListenerContext);
	}
}

void SpatialManager_KDTreeBase::TreeNodeLeaf::add(TreeNode* pOwner, SpatialDynamicListener* pListener, DynamicVolume* pObject, void* pListenerContext) {

	mDynVols.addOne(pObject);
	pObject->nodes.addOne(pOwner);
}

void SpatialManager_KDTreeBase::TreeNodeLeaf::remove(TreeNode* pOwner, SpatialDynamicListener* pListener, DynamicVolume* pObject, void* pListenerContext) {

	mDynVols.searchRemoveSwapWithLast(pObject);
	pObject->nodes.searchRemoveSwapWithLast(pOwner);
}

void SpatialManager_KDTreeBase::TreeNodeLeaf::onClear(DynamicVolume* pObject) {

	mDynVols.searchRemoveSwapWithLast(pObject);
}

void SpatialManager_KDTreeBase::TreeNodeLeaf::appendToQuery(SpatialObjectGroup_KDTree& query, DynamicVolume* pExcluded) {

	for (DynVols::Index i = 0; i < mDynVols.count; ++i) {

		if (mDynVols[i] != pExcluded) {

			query.uniqueAddDynamicVolume(mDynVols[i]->binding);
		}
	}

	for (StaticVols::Index i = 0; i < mStaticVols.count; ++i) {

		query.uniqueAddVolume(mStaticVols[i]->binding);
	}


	for (StaticTriConts::Index i = 0; i < mStaticTriConts.count; ++i) {

		//we donno if boundingVolume already added for this leaf/triCont combination
		//checking is (too much?) trouble
		query.uniqueAddTriCont(mStaticTriConts[i], staticTriBoundingVolume, false);
	}
}

void SpatialManager_KDTreeBase::TreeNodeLeaf::render(Renderer& renderer) {

	for (StaticTriConts::Index i = 0; i < mStaticTriConts.count; ++i) {

		StaticTriCont& triCont = dref(mStaticTriConts.el[i]);

		TriBuffer& buffer = triCont.buffer;
		SpatialIndices& indices = triCont.mTriIndices;

		for (Index ii = 0; ii < indices.size; ++ii) {
			
			const TriangleEx1& tri = buffer.get(indices.el[ii]);
			
			renderer.draw(tri, &Matrix4x3Base::kIdentity, &RenderColor::kYellow, false, true);
		}
	}
}

void SpatialManager_KDTreeBase::TreeNode::render(Renderer& renderer) {

	bool draw = false;

	if (leaf.isValid() && leaf->mDynVols.count) {

		renderer.draw(volume, &Matrix4x3Base::kIdentity, &RenderColor::kMagenta, true);
		draw = true;
	} 

	renderer.draw(volume, &Matrix4x3Base::kIdentity, &RenderColor::kRed, true);
	

	if (/*recurse && */branch.isValid()) {

		branch->child[0].render(renderer);
		branch->child[1].render(renderer);
	}

	if (/*draw && */leaf.isValid()) {
	
		leaf->render(renderer);
	}
}

}