#include "WESweepPrune.h"
#include "WEBubbleSort.h"

#include "../../../WEMem.h"
#include "float.h"

namespace WE {


void SweepPruneSpans::sortValuesCopyTo(SweepPruneSpan* target) {

	target[0].endPoints[0].sortValue = spans[0].endPoints[0].sortValue;
	target[0].endPoints[1].sortValue = spans[0].endPoints[1].sortValue;
	target[1].endPoints[0].sortValue = spans[0].endPoints[0].sortValue;
	target[1].endPoints[1].sortValue = spans[0].endPoints[1].sortValue;
	target[2].endPoints[0].sortValue = spans[0].endPoints[0].sortValue;
	target[2].endPoints[1].sortValue = spans[0].endPoints[1].sortValue;
}

/*

void SweepPruneSpans::spansCopyTo0(SweepPruneSpan* target) {

	memcpy(spans, target, 3 * sizeof(SweepPruneSpan));
}

void SweepPruneSpans::spansCopyTo1(SweepPruneSpan* target, const short& dimCount) {

	memcpy(spans, target, dimCount * sizeof(SweepPruneSpan));
}

void SweepPruneSpans::spansCopyTo2(SweepPruneSpan* target, const short& dimCount) {

	for (int d = 0; d < dimCount; d++) {

		target[d].endPoints[0].pBind = spans[d].endPoints[0].pBind;
		target[d].endPoints[0].sortIndex = spans[d].endPoints[0].sortIndex;
		target[d].endPoints[0].sortValue = spans[d].endPoints[0].sortValue;

		target[d].endPoints[1].pBind = spans[d].endPoints[1].pBind;
		target[d].endPoints[1].sortIndex = spans[d].endPoints[1].sortIndex;
		target[d].endPoints[1].sortValue = spans[d].endPoints[1].sortValue;
	}
}

void SweepPruneSpans::spansCopyTo3(SweepPruneSpan* target, const short& dimCount) {

	for (int d = 0; d < dimCount; d++) {

		target[d].endPoints[0].sortIndex = spans[d].endPoints[0].sortIndex;
		target[d].endPoints[0].sortValue = spans[d].endPoints[0].sortValue;

		target[d].endPoints[1].sortIndex = spans[d].endPoints[1].sortIndex;
		target[d].endPoints[1].sortValue = spans[d].endPoints[1].sortValue;
	}
}

void SweepPruneSpans::spansCopyTo4(SweepPruneSpan* target, const short& dimCount) {

	for (int d = 0; d < dimCount; d++) {

		target[d] = spans[d];
	}
}
*/

SweepPruneBinding::SweepPruneBinding(bool isRay) {

	if (isRay) {

		for (int d = 0; d < 3; d++) {

			spans[d].endPoints[0].pBind = this;
			spans[d].endPoints[0].type = SPST_RayOrigin;
			spans[d].endPoints[1].pBind = this;
			spans[d].endPoints[1].type = SPST_RayPoint;
		}

	} else {

		for (int d = 0; d < 3; d++) {

			spans[d].endPoints[SPST_Min].pBind = this;
			spans[d].endPoints[SPST_Min].type = SPST_Min;
			spans[d].endPoints[SPST_Max].pBind = this;
			spans[d].endPoints[SPST_Max].type = SPST_Max;
		}
	}
}


SweepPruneBinding& SweepPrune::newBinding(Object& object, ObjectData& data, bool isRay) {

	SweepPruneBinding*& pBind = bindingPtr(data);

	assrt(pBind == NULL);

	MMemNew(pBind, SweepPruneBinding(isRay));

	pBind->pObject = &object;

	return (*pBind);
}

void SweepPrune::deleteBinding(ObjectData& data) {

	SweepPruneBinding*& pBind = bindingPtr(data);

	MMemDelete(pBind);
}

inline SweepPruneBinding*& SweepPrune::bindingPtr(ObjectData& data) {

	return (SweepPruneBinding*&) (data.ptr);
}

inline SweepPruneBinding& SweepPrune::binding(ObjectData& data) {

	return dref<SweepPruneBinding>((SweepPruneBinding*) (data.ptr));
}

SweepPrune::SweepPrune() {

	mDimCount = 0;
}

SweepPrune::~SweepPrune() {

	destroy();
}

void SweepPrune::destroy() {

	for (int d = 0; d < mDimCount; d++) {

		mEndPoints[d].destroy();
	}

	mDimCount = 0;
}

void SweepPrune::initAxis(short dim, const Vector3& axis) {

	mAxis[dim] = axis;
}

void SweepPrune::initAxis(short dim, Axis axis) {

	mPrincAxis[dim] = axis;
}

void SweepPrune::init(bool usePrincipalAxis, short dimCount, SweepPruneIndex::Type objectCount) {

	destroy();

	mDimCount = dimCount;
	assrt(mDimCount != 0 && mDimCount <= 3);

	mUsePrincipalAxis = usePrincipalAxis;

	if (objectCount) {

		objectCount = MMax(4, objectCount);

		for (int d = 0; d < mDimCount; d++) {

			mEndPoints[d].reserve(objectCount * 2);
		}
	}
}

void SweepPrune::onStartColl(SweepPruneBinding* pBind1, SweepPruneBinding* pBind2) {

	if (mIsEnabledListener) {

		mListener->collStarted(pBind1->pObject, pBind2->pObject);
	}
}


void SweepPrune::onEndColl(SweepPruneBinding* pBind1, SweepPruneBinding* pBind2) {
	
	if (mIsEnabledListener) {

		mListener->collEnded(pBind1->pObject, pBind2->pObject);
	}
}


//PS dont change to SweepPruneIndex& this wont work because of the swapping
void SweepPrune::swapEndPoints(EndPointArray& arr, SweepPruneIndex::Type index1, SweepPruneIndex::Type index2) {

	SweepPruneEndPoint* pTemp = arr.el[index1];

	arr.el[index1]->sortIndex = index2;
	arr.el[index2]->sortIndex = index1;

	arr.el[index1] = arr.el[index2];
	arr.el[index2] = pTemp;
}

inline bool bindSpansOverlap(SweepPruneBinding& bind1, SweepPruneBinding& bind2, int dim) {
	
	return spanOverlap(bind1.spans[dim].endPoints[SPST_Min].sortValue, bind1.spans[dim].endPoints[SPST_Max].sortValue, 
						bind2.spans[dim].endPoints[SPST_Min].sortValue, bind2.spans[dim].endPoints[SPST_Max].sortValue);
}

inline bool bindSpansOverlapRay(SweepPruneBinding& bindRay, SweepPruneBinding& bind2, int dim, short rayEndPoint = 0) {
	
	return spanOverlap(bind2.spans[dim].endPoints[SPST_Min].sortValue, bind2.spans[dim].endPoints[SPST_Max].sortValue,
						bindRay.spans[dim].endPoints[rayEndPoint].sortValue);
}

							
bool __fastcall testSpansOverlapNextDims(SweepPruneBinding& bind, SweepPruneBinding& compBind, int dim, short checkDimCount) {

	short i = 0;

	if (++dim == 3) dim = 0; 
	if (bindSpansOverlap(bind, compBind, dim) == false) return false;
	if (++i == checkDimCount) return true;
	if (++dim == 3) dim = 0; 
	if (bindSpansOverlap(bind, compBind, dim) == false) return false;
	if (++i == checkDimCount) return true;
	if (++dim == 3) dim = 0; 
	if (bindSpansOverlap(bind, compBind, dim) == false) return false;


	return true;
}

bool __fastcall testSpansOverlapNextDimsRay(SweepPruneBinding& bindRay, SweepPruneBinding& compBind, int dim, short checkDimCount, short rayEndPoint = 0) {

	short i = 0;

	if (++dim == 3) dim = 0; 
	if (bindSpansOverlapRay(bindRay, compBind, dim, rayEndPoint) == false) return false;
	if (++i == checkDimCount) return true;
	if (++dim == 3) dim = 0; 
	if (bindSpansOverlapRay(bindRay, compBind, dim, rayEndPoint) == false) return false;
	if (++i == checkDimCount) return true;
	if (++dim == 3) dim = 0; 
	if (bindSpansOverlapRay(bindRay, compBind, dim, rayEndPoint) == false) return false;

	
	return true;
}



bool SweepPrune::hasBinding(Object& object, ObjectData& data) {

	return (bindingPtr(data) != NULL);
}

bool SweepPrune::createBinding(Object& object, ObjectData& data, const CollMask& mask) {

	SweepPruneBinding* pBind = bindingPtr(data);

	if (pBind != NULL) {

		return false;
	}

	SweepPruneBinding& bind = newBinding(object, data);
	bind.mask = mask;

	return true;
}

void SweepPrune::destroyBinding(Object& object, ObjectData& data) {

	deleteBinding(data);
}

bool SweepPrune::extractCollMask(Object& object, ObjectData& data, CollMask& mask) {

	SweepPruneBinding* pBind = bindingPtr(data);

	if (pBind) {

		mask = pBind->mask;

		return true;
	}

	return false;
}

void SweepPrune::add(Object& object, ObjectData& data, VolumeStruct& volume) {

	SweepPruneBinding& bind = binding(data);

	SweepPruneEndPoint* pEndPoint;

	for (int d = 0; d < mDimCount; d++) {

		for (short type = 0; type < 2; type++) {

			pEndPoint = &(bind.spans[d].endPoints[type]);
			pEndPoint->sortIndex = mEndPoints[d].addOneIndex(pEndPoint);
		}
	}

	update(object, data, volume);
}

bool __fastcall _isPotentialColl(const SweepPruneBinding* pBind1, const SweepPruneBinding* pBind2, const CollMaskMatrix& matrix) {

	return (pBind1->mask.mask(pBind2->mask, matrix) && (pBind1 != pBind2));
}

bool __fastcall isPotentialColl(const SweepPruneBinding* pBind1, const SweepPruneBinding* pBind2, const CollMaskMatrix& matrix) {

	return (pBind1->mask.mask(pBind2->mask, matrix) && (pBind1 != pBind2));
	//return (pBind1->mask.mask<ObjectType>(pBind2->mask, pBind1->pObject->objectType, pBind2->pObject->objectType, matrix) && (pBind1 != pBind2));
}

bool __fastcall isPotentialCollRay(const SweepPruneBinding* pBindRay, const SweepPruneBinding* pBind2, const CollMaskMatrix& matrix) {

	return pBindRay->mask.mask(pBind2->mask, matrix);
}

void SweepPrune::_toForwardSwapDir(SwapDirectionType& dir, EndPointArray& sortArray, SweepPruneIndex::Type& lastIndex) {

	assrt(sortArray.count != 0);

	dir = SDT_Forward;
	lastIndex = sortArray.count - 1;
}

bool SweepPrune::_shoulContinueSwapping(SwapDirectionType& dir, EndPointArray& sortArray, SweepPruneIndex::Type& sortIndex, SweepPruneIndex::Type& compIndex, SweepPruneIndex::Type& lastIndex, bool& didSwap) {

	if (dir == SDT_Back) {

		compIndex = sortIndex - 1;
		
		if (sortIndex != 0) {

			return true;
		}

		if (didSwap) {

			return false;
		}

		_toForwardSwapDir(dir, sortArray, lastIndex);
	}	

	compIndex = sortIndex + 1;

	return (sortIndex != lastIndex);
}

bool SweepPrune::_needSwap(SwapDirectionType& dir, SweepPruneEndPoint& endPoint, SweepPruneIndex::Type& compIndex, EndPointArray& sortArray) {

	if (dir == SDT_Back) {
		
		return (endPoint.sortValue < sortArray.el[compIndex]->sortValue);
	} 

	return (endPoint.sortValue > sortArray.el[compIndex]->sortValue);
}

void SweepPrune::_preSwapOp(int& dim, SwapDirectionType& dir, SweepPruneEndPoint& endPoint,
							SweepPruneEndPoint& crossedEndPoint) {

	PreSwapOp op = PSOP_None;
	bool rayFirst;
	
	
	//this code should be equivalent to switch statements and hopefully a bit faster
	if (dir == SDT_Back) {

		op = (PreSwapOp) (crossedEndPoint.type - endPoint.type);
		
	} else {

		op = (PreSwapOp) (endPoint.type - crossedEndPoint.type);
	}
	
	if (op >= 3) {
		
		if ((rayFirst = (endPoint.type == SPST_RayOrigin)) == false) {
		
			op = (PreSwapOp) -op;
		}

	} else if (op <= -3) {

		if ((rayFirst = (endPoint.type == SPST_RayOrigin)) == false) {
		
			op = (PreSwapOp) -op;
		}
	} 

	//another options is using mDimCount instead of (mDimCount - 1) in isCollNextDims
	//and isCollNextDimsRay

	switch (op) {
		
		case PSOP_StartColl:
			if (isPotentialColl(endPoint.pBind, crossedEndPoint.pBind, *mpCollMaskMatrix)
					&& testSpansOverlapNextDims(*endPoint.pBind, *crossedEndPoint.pBind, dim, mDimCount - 1)) {

				onStartColl(endPoint.pBind, crossedEndPoint.pBind);
			}
			break;
		case PSOP_EndColl:
			if (isPotentialColl(endPoint.pBind, crossedEndPoint.pBind, *mpCollMaskMatrix)) {

				onEndColl(endPoint.pBind, crossedEndPoint.pBind);
			}
			break;
		case PSOP_StartCollRay:
		case PSOP_StartCollRay2:
			if (rayFirst) {

				if (isPotentialCollRay(endPoint.pBind, crossedEndPoint.pBind, *mpCollMaskMatrix)
						&& testSpansOverlapNextDimsRay(*endPoint.pBind, *crossedEndPoint.pBind, dim, mDimCount - 1)) {

					onStartColl(endPoint.pBind, crossedEndPoint.pBind);
				}

			} else {

				if (isPotentialCollRay(crossedEndPoint.pBind, endPoint.pBind, *mpCollMaskMatrix)
						&& testSpansOverlapNextDimsRay(*crossedEndPoint.pBind, *endPoint.pBind, dim, mDimCount - 1)) {

					onStartColl(crossedEndPoint.pBind, endPoint.pBind);
				}
			}
			break;
		case PSOP_EndCollRay:
		case PSOP_EndCollRay2:
			if (rayFirst) {

				if (isPotentialCollRay(endPoint.pBind, crossedEndPoint.pBind, *mpCollMaskMatrix)) {

					onEndColl(endPoint.pBind, crossedEndPoint.pBind);
				}

			} else {

				if (isPotentialCollRay(crossedEndPoint.pBind, endPoint.pBind, *mpCollMaskMatrix)) {

					onEndColl(crossedEndPoint.pBind, endPoint.pBind);
				}
			}
			break;
	}

}


void SweepPrune::_swap(int& dim, SweepPruneEndPoint& endPoint, EndPointArray& sortArray, 
							 SweepPruneIndex::Type& compIndex, bool& didSwap, SweepPruneIndex::Type& lastIndex) {

	SwapDirectionType swapDir = SDT_Back;
	didSwap = false;
	
	while (_shoulContinueSwapping(swapDir, sortArray, endPoint.sortIndex, compIndex, lastIndex, didSwap)) {

		if (_needSwap(swapDir, endPoint, compIndex, sortArray)) {

			didSwap = true;

			_preSwapOp(dim, swapDir, endPoint, *sortArray.el[compIndex]); 
		
			/*
			if (floatOver) {

				if (swapDir == SDT_Back) {

					--endPoint.sortIndex;

				} else {

					++endPoint.sortIndex;
				}

			} else {

				swapEndPoints(sortArray, endPoint.sortIndex, compIndex);
			}
			*/
			swapEndPoints(sortArray, endPoint.sortIndex, compIndex);

		} else {

			if (swapDir == SDT_Back) {
			
				_toForwardSwapDir(swapDir, sortArray, lastIndex);

			} else {

				break;
			}
		}
	}
}

void SweepPrune::_blindSwap(SweepPruneEndPoint& endPoint, EndPointArray& sortArray, SweepPruneIndex::Type& targetIndex) {

	while (endPoint.sortIndex < targetIndex) {

		swapEndPoints(sortArray, endPoint.sortIndex, endPoint.sortIndex + 1);
	} 

	while (endPoint.sortIndex > targetIndex) {

		swapEndPoints(sortArray, endPoint.sortIndex, endPoint.sortIndex - 1);
	}
}

void SweepPrune::_blindMove(SweepPruneSpan* spans, SweepPruneSpan* targetSpans) {

	int d = 0;

	d = 0;

	_blindSwap((spans[d].endPoints[0]), mEndPoints[d], (targetSpans[d].endPoints[0]).sortIndex);
	_blindSwap((spans[d].endPoints[1]), mEndPoints[d], (targetSpans[d].endPoints[1]).sortIndex);
	if (++d == mDimCount) return;
	_blindSwap((spans[d].endPoints[0]), mEndPoints[d], (targetSpans[d].endPoints[0]).sortIndex);
	_blindSwap((spans[d].endPoints[1]), mEndPoints[d], (targetSpans[d].endPoints[1]).sortIndex);
	if (++d == mDimCount) return;
	_blindSwap((spans[d].endPoints[0]), mEndPoints[d], (targetSpans[d].endPoints[0]).sortIndex);
	_blindSwap((spans[d].endPoints[1]), mEndPoints[d], (targetSpans[d].endPoints[1]).sortIndex);

	/*

	for (d = 0; d < mDimCount; d++) {

		for (short type = 0; type < 2; type++) {

			_blindSwap((spans[d].endPoints[type]), mEndPoints[d], (targetSpans[d].endPoints[type]).sortIndex);
		}
	}
	*/
}

void SweepPrune::_swap(SweepPruneSpan* spans) {

	int d;

	SweepPruneIndex::Type  compIndex;
	SweepPruneIndex::Type  lastIndex;
	bool didSwap;

	d = 0;

	_swap(d, (spans[d].endPoints[0]), mEndPoints[d], compIndex, didSwap, lastIndex);
	_swap(d, (spans[d].endPoints[1]), mEndPoints[d], compIndex, didSwap, lastIndex);
	if (++d == mDimCount) return;
	_swap(d, (spans[d].endPoints[0]), mEndPoints[d], compIndex, didSwap, lastIndex);
	_swap(d, (spans[d].endPoints[1]), mEndPoints[d], compIndex, didSwap, lastIndex);
	if (++d == mDimCount) return;
	_swap(d, (spans[d].endPoints[0]), mEndPoints[d], compIndex, didSwap, lastIndex);
	_swap(d, (spans[d].endPoints[1]), mEndPoints[d], compIndex, didSwap, lastIndex);

	/*
	for (d = 0; d < mDimCount; d++) {

		for (short type = 0; type < 2; type++) {

			_swap(d, (spans[d].endPoints[type]), mEndPoints[d], compIndex, didSwap, lastIndex);
		}
	}
	*/
}


void SweepPrune::_computeSpans(SweepPruneSpan* spans, VolumeStruct& volume) {

	int d;

	d = 0;
	if (mUsePrincipalAxis) {

		volume.computeSpan(mPrincAxis[d], spans[d].endPoints[SPST_Min].sortValue, spans[d].endPoints[SPST_Max].sortValue);
		if (++d == mDimCount) return;
		volume.computeSpan(mPrincAxis[d], spans[d].endPoints[SPST_Min].sortValue, spans[d].endPoints[SPST_Max].sortValue);
		if (++d == mDimCount) return;
		volume.computeSpan(mPrincAxis[d], spans[d].endPoints[SPST_Min].sortValue, spans[d].endPoints[SPST_Max].sortValue);

		/*
		for (d = 0; d < mDimCount; d++) 
			volume.computeSpan(mPrincAxis[d], spans[d].endPoints[SPST_Min].sortValue, 
								spans[d].endPoints[SPST_Max].sortValue);
		*/

	} else {

		volume.computeSpan(mAxis[d], spans[d].endPoints[SPST_Min].sortValue, spans[d].endPoints[SPST_Max].sortValue);
		if (++d == mDimCount) return;
		volume.computeSpan(mAxis[d], spans[d].endPoints[SPST_Min].sortValue, spans[d].endPoints[SPST_Max].sortValue);
		if (++d == mDimCount) return;
		volume.computeSpan(mAxis[d], spans[d].endPoints[SPST_Min].sortValue, spans[d].endPoints[SPST_Max].sortValue);

		/*
		for (d = 0; d < mDimCount; d++) 
			volume.computeSpan(mAxis[d], spans[d].endPoints[SPST_Min].sortValue, 
								spans[d].endPoints[SPST_Max].sortValue);
		*/
		

	}
}


void SweepPrune::update(Object& object, ObjectData& data, VolumeStruct& volume) {

	SweepPruneBinding& bind = binding(data);

	_computeSpans(bind.spans, volume);
	_swap(bind.spans);
}

void SweepPrune::update(Object& object, ObjectData& data, VolumeStruct& volume, ObjectData& volumeCache) {

	SweepPruneVolCache& cache = toCache(volumeCache);
	SweepPruneBinding& bind = binding(data);

	cache.sortValuesCopyTo(bind.spans);
	_swap(bind.spans);
}

void SweepPrune::removeSpansFromEndPoints(SweepPruneSpan* spans) {

	EndPointArray::Index i;

	for (int d = 0; d < 3; d++) {
		
		i = spans[d].endPoints[SPST_Min].sortIndex;

		mEndPoints[d].removeIndex(spans[d].endPoints[SPST_Max].sortIndex);
		mEndPoints[d].removeIndex(i);

		while (i < mEndPoints[d].count) {

			mEndPoints[d].el[i]->sortIndex = i;
			++i;
		}
	}
}

void SweepPrune::remove(Object& object, ObjectData& data, bool destroyBinding, bool isRemovingAllObjects) {

	if (isRemovingAllObjects == false) {

		SweepPruneBinding& bind = binding(data);
	
		removeSpansFromEndPoints(bind.spans);
	}

	if (destroyBinding)
		deleteBinding(data);
}

void SweepPrune::render(Renderer& renderer, Object& object, ObjectData& data, const RenderColor* pRenderColor) {

	if (mDimCount != 3) {

		return;
	}

	SweepPruneBinding& bind = binding(data);

	OBB obb;
	
	for (int d = 0; d < mDimCount; ++d) {

		float& min = bind.spans[d].endPoints[0].sortValue;
		float& max = bind.spans[d].endPoints[1].sortValue;

		MOBBCenter(obb).el[d] = (min + max) * 0.5f;
		MOBBExtents(obb).el[d] = (max - min) * 0.5f;
		

		if (mUsePrincipalAxis) {

			MOBBAxis(obb, d).set(kAxis[mPrincAxis[d]]);

		} else {

			MOBBAxis(obb, d).set(mAxis[d]);
		}
	}
		
	renderer.draw(obb, &Matrix4x3Base::kIdentity, pRenderColor);
}

SweepPruneVolCache& SweepPrune::toCache(ObjectData& cacheData) {

	return dref<SweepPruneVolCache>((SweepPruneVolCache*) cacheData.ptr);
}

SweepPruneVolCache*& SweepPrune::toCachePtr(ObjectData& cacheData) {

	return (SweepPruneVolCache*&)(cacheData.ptr);
}

void SweepPrune::createVolumeCache(ObjectData& cacheData) {

	if (toCachePtr(cacheData) == NULL) {

		MMemNew(cacheData.ptr, SweepPruneVolCache());
		
	} else {
	
		assrt(false);
	}
}

void SweepPrune::destroyVolumeCache(ObjectData& cacheData) {

	MMemDelete(toCachePtr(cacheData));
}

void SweepPrune::initVolumeCache(VolumeStruct& volume, ObjectData& cacheData) {

	SweepPruneVolCache& cache = toCache(cacheData);
	
	_computeSpans(cache.spans, volume);
}

void SweepPrune::initVolumeCache(Object& object, ObjectData& data, ObjectData& cacheData) {

	SweepPruneVolCache& cache = toCache(cacheData);
	SweepPruneBinding& bind = binding(data);

	bind.sortValuesCopyTo(cache.spans);
}

/*
SweepPruneState& SweepPrune::toState(ObjectData& stateData) {

	return dref<SweepPruneState>((SweepPruneState*) stateData.ptr);
}

SweepPruneState*& SweepPrune::toStatePtr(ObjectData& stateData) {

	return (SweepPruneState*&)(stateData.ptr);
}

void SweepPrune::createObjectState(ObjectData& stateData) {

	if (toStatePtr(stateData) == NULL) {

		MMemNew(stateData.ptr, SweepPruneState());
		
		SweepPruneState& state = toState(stateData);
		//state.reserveMarks(2);

	} else {
	
		assrt(false);
	}
}

void SweepPrune::destroyObjectState(ObjectData& stateData) {

	MMemDelete(toStatePtr(stateData));
}

void SweepPrune::syncStateToObject(Object& object, ObjectData& data, ObjectData& stateData) {

	SweepPruneTest& state = toTest(stateData);
	SweepPruneBinding& bind = binding(data);

	bind.spansCopyTo(state.spans);
}

void SweepPrune::syncObjectToState(Object& object, ObjectData& data, ObjectData& stateData) {

	SweepPruneTest& state = toTest(stateData);
	SweepPruneBinding& bind = binding(data);

	state.spansCopyTo(bind.spans);
}
*/

void SweepPrune::addRay(Object& object, ObjectData& data, const CollMask& mask, Point& origin) {

	SweepPruneBinding& bind = newBinding(object, data, true);

	bind.mask = mask;

	SweepPruneEndPoint* pEndPoint;

	for (int d = 0; d < mDimCount; d++) {

		pEndPoint = &(bind.spans[d].endPoints[0]);
		pEndPoint->sortIndex = mEndPoints[d].addOneIndex(pEndPoint);
	}

	updateRay(object, data, origin);
}

void SweepPrune::_computeRaySpans(SweepPruneSpan* spans, Point& origin) {

	int d;

	d = 0;
	if (mUsePrincipalAxis) {

		computeSpan(mPrincAxis[d], origin, spans[d].endPoints[0].sortValue);
		if (++d == mDimCount) return;
		computeSpan(mPrincAxis[d], origin, spans[d].endPoints[0].sortValue);
		if (++d == mDimCount) return;
		computeSpan(mPrincAxis[d], origin, spans[d].endPoints[0].sortValue);

		/*
		for (d = 0; d < mDimCount; d++) 
			computeSpan(mPrincAxis[d], origin, bind.spans[d].endPoints[0].sortValue);
		*/
		
		
	} else {

		computeSpan(mAxis[d], origin, spans[d].endPoints[0].sortValue);
		if (++d == mDimCount) return;
		computeSpan(mAxis[d], origin, spans[d].endPoints[0].sortValue);
		if (++d == mDimCount) return;
		computeSpan(mAxis[d], origin, spans[d].endPoints[0].sortValue);

		/*
		for (d = 0; d < mDimCount; d++) 
			computeSpan(mAxis[d], origin, bind.spans[d].endPoints[0].sortValue);
		*/
		
	}
}

void SweepPrune::updateRay(Object& object, ObjectData& data, Point& origin) {

	SweepPruneBinding& bind = binding(data);
	int d;

	_computeRaySpans(bind.spans, origin);
	
	SweepPruneIndex::Type compIndex;
	SweepPruneIndex::Type lastIndex;
	bool didSwap;

	for (d = 0; d < mDimCount; d++) {

		_swap(d, (bind.spans[d].endPoints[0]), (mEndPoints[d]), compIndex, didSwap, lastIndex);
	}
}

void SweepPrune::fireRay(Object& object, ObjectData& data, Vector& _direction, Object*& pHitObject) {

	SweepPruneBinding& bind = binding(data);

	/*
	if (bind.mColls.count) {

		SweepPrunePair* pPair = bind.mColls.el[0];
		pHitObject = pPair->pBind[getSwitchedIndexInPair(pPair, &bind)]->pObject;

		return;
	}
	*/
	
	SweepPruneIndex::Type  compIndex;
	float axisInterval[3];
	float interval[4];
	short minInterval;
	float angleCos[3];
	int dir[3];
	Vector direction;
	int d;
	bool validInterval;

	_direction.normalize(direction);

	for (d = 0; d < mDimCount; d++) {
		
		bind.spans[d].endPoints[1].sortIndex = bind.spans[d].endPoints[0].sortIndex;
		bind.spans[d].endPoints[1].sortValue = bind.spans[d].endPoints[0].sortValue;

		if (mUsePrincipalAxis) {

			angleCos[d] = direction.el[d];
		} else {

			angleCos[d] = mAxis[d].dot(direction);
		}

		if (angleCos[d] > 0.0f) {
			dir[d] = 1;

		} else if (angleCos[d] < 0.0f) {
			dir[d] = -1;

		} else {
			dir[d] = 0;
		}
	}


	interval[3] = FLT_MAX;
	pHitObject = NULL;

	while (pHitObject == NULL) {

		minInterval = 3;

		for (d = 0; d < mDimCount; d++) {

			EndPointArray& sortArray = mEndPoints[d];
			SweepPruneEndPoint& endPoint = bind.spans[d].endPoints[1];

			validInterval = false;

			switch (dir[d]) {
				case 1:
					{
						if (endPoint.sortIndex != sortArray.count - 1) {

							compIndex = endPoint.sortIndex + 1;
							axisInterval[d] = sortArray.el[compIndex]->sortValue - endPoint.sortValue;

							validInterval = true;
						} 
					}
					break;
				case -1:
					{
						if (endPoint.sortIndex > 0) {

							compIndex = endPoint.sortIndex - 1;
							axisInterval[d] = sortArray.el[compIndex]->sortValue - endPoint.sortValue;

							validInterval = true;

						} 
					}
					break;
			}

			if (validInterval) {

				interval[d] = axisInterval[d] / angleCos[d];
				
				if (interval[d] < interval[minInterval]) {
					minInterval = d;
				}
			}
		}

		if (minInterval == 3) {

			return;
		}

		d = minInterval;
		SweepPruneEndPoint& endPoint = bind.spans[d].endPoints[1];
		endPoint.sortIndex += dir[d];
		SweepPruneEndPoint& crossedEndPoint = *(mEndPoints[d].el[endPoint.sortIndex]);
		endPoint.sortValue = crossedEndPoint.sortValue;

		++d; if (d == 3) d = 0;
		bind.spans[d].endPoints[1].sortValue += interval[minInterval] * angleCos[d];
		++d; if (d == 3) d = 0;
		bind.spans[d].endPoints[1].sortValue += interval[minInterval] * angleCos[d];

		if (dir[minInterval] == 1 ? crossedEndPoint.type == SPST_Min :crossedEndPoint.type == SPST_Max) {
		
			if (testSpansOverlapNextDimsRay(bind, *(crossedEndPoint.pBind), minInterval, 2, 1)) {

				pHitObject = crossedEndPoint.pBind->pObject;
				return;
			}
		}
	}
}

/*
void SweepPrune::fireRayDebug(Object& object, ObjectData& data, Vector& _direction, Object*& pHitObject, int& iterCount, int limitIterCount, Renderer& renderer, const RenderColor* pColor) {

	SweepPruneBinding& bind = binding(data);
	
	if (bind.mColls.count) {

		SweepPrunePair* pPair = bind.mColls.el[0];
		pHitObject = pPair->pBind[getSwitchedIndexInPair(pPair, &bind)]->pObject;

		return;
	}
	
	SweepPruneIndex::Type  compIndex;
	float axisInterval[3];
	float interval[4];
	short minInterval;
	float angleCos[3];
	int dir[3];
	Vector direction;
	int d;
	bool validInterval;

	_direction.normalize(direction);

	int doRender = 0;
	Vector renderPoint[2];

	for (d = 0; d < mDimCount; d++) {
		
		bind.spans[d].endPoints[1].sortIndex = bind.spans[d].endPoints[0].sortIndex;
		bind.spans[d].endPoints[1].sortValue = bind.spans[d].endPoints[0].sortValue;

		renderPoint[doRender].el[d] = bind.spans[d].endPoints[1].sortValue;

		if (mUsePrincipalAxis) {

			angleCos[d] = direction.el[d];
		} else {

			angleCos[d] = mAxis[d].dot(direction);
		}

		if (angleCos[d] > 0.0f) {
			dir[d] = 1;

		} else if (angleCos[d] < 0.0f) {
			dir[d] = -1;

		} else {
			dir[d] = 0;
		}
	}
	
	interval[3] = FLT_MAX;
	pHitObject = NULL;

	iterCount = 0;

	while (pHitObject == NULL) {

		++iterCount;

		if (iterCount == limitIterCount) {

			return;
		}

		minInterval = 3;

		for (d = 0; d < mDimCount; d++) {

			EndPointArray& sortArray = mEndPoints[d];
			SweepPruneEndPoint& endPoint = bind.spans[d].endPoints[1];

			validInterval = false;

			switch (dir[d]) {
				case 1:
					{
						if (endPoint.sortIndex != sortArray.count - 1) {

							compIndex = endPoint.sortIndex + 1;
							axisInterval[d] = sortArray.el[compIndex]->sortValue - endPoint.sortValue;

							validInterval = true;
						} 
					}
					break;
				case -1:
					{
						if (endPoint.sortIndex > 0) {

							compIndex = endPoint.sortIndex - 1;
							axisInterval[d] = sortArray.el[compIndex]->sortValue - endPoint.sortValue;

							validInterval = true;

						} 
					}
					break;
			}

			if (validInterval) {

				interval[d] = axisInterval[d] / angleCos[d];
				
				if (interval[d] < interval[minInterval]) {
					minInterval = d;
				}
			}
		}

		if (minInterval == 3) {

			return;
		}

		d = minInterval;
		SweepPruneEndPoint& endPoint = bind.spans[d].endPoints[1];
		endPoint.sortIndex += dir[d];
		SweepPruneEndPoint& crossedEndPoint = *(mEndPoints[d].el[endPoint.sortIndex]);
		endPoint.sortValue = crossedEndPoint.sortValue;

		++doRender;
		renderPoint[doRender].el[d] = bind.spans[d].endPoints[1].sortValue;

		++d; if (d == 3) d = 0;
		bind.spans[d].endPoints[1].sortValue += interval[minInterval] * angleCos[d];
		renderPoint[doRender].el[d] = bind.spans[d].endPoints[1].sortValue;
		++d; if (d == 3) d = 0;
		bind.spans[d].endPoints[1].sortValue += interval[minInterval] * angleCos[d];
		renderPoint[doRender].el[d] = bind.spans[d].endPoints[1].sortValue;

		renderer.draw(renderPoint[(doRender + 1)%2], renderPoint[doRender], &Matrix4x3Base::kIdentity, pColor);
		doRender = (doRender + 1)%2;

		if (dir[minInterval] == 1 ? crossedEndPoint.type == SPST_Min :crossedEndPoint.type == SPST_Max) {

			if (isCollNextDimsRay(bind, *(crossedEndPoint.pBind), minInterval, 2, 1)) {

				pHitObject = crossedEndPoint.pBind->pObject;
				return;
			}
		}
	}
}
*/

void SweepPrune::removeRay(Object& object, ObjectData& data, bool isRemovingAllObjects) {
	
	if (isRemovingAllObjects == false) {

		SweepPruneBinding& bind = binding(data);

		removeSpansFromEndPoints(bind.spans);
	}

	deleteBinding(data);
}

}