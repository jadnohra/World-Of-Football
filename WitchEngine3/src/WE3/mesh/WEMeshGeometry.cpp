#include "WEMeshGeometry.h"

#include "../WEMacros.h"
#include "../geometry/WEGeometryAnalyzer.h"

namespace WE {

MeshGeometry::MeshGeometry() {
}

MeshGeometry::~MeshGeometry() {
}

void MeshGeometry::applyCoordSys(CoordSysConvPool& convPool, const CoordSys& coordSys) {

	RefWrap<CoordSysConv> conv;

	for (MeshGeometryPartIndex i = 0; i < mParts.size; i++) {

		mParts.el[i]->applyCoordSys(convPool, coordSys);
	}
}

void MeshGeometry::applyTransform(GeometryVertexTransform* pTrans) {

	for (MeshGeometryPartIndex i = 0; i < mParts.size; i++) {

		mParts.el[i]->applyTransform(pTrans);
	}
}

void MeshGeometry::reanchor(float* anchoringOffset) {

	for (MeshGeometryPartIndex i = 0; i < mParts.size; i++) {

		mParts.el[i]->reanchor(anchoringOffset);
	}
}

bool MeshGeometry::setPartCount(MeshGeometryPartIndex::Type count) {

	if (mParts.size != 0) {

		return false;
	}

	mParts.setSize(count);
	mPartBindings.setSize(count);

	return true;
}

MeshGeometryPartIndex::Type MeshGeometry::getPartCount() {

	return mParts.size;
}

bool MeshGeometry::createPart(MeshGeometryPartIndex::Type index, MeshGeometryPart** ppPart) {

	if (mParts.el[index] != NULL) {

		return false;
	}

	MMemNew(mParts.el[index], MeshGeometryPart());

	if (ppPart) {

		*ppPart = mParts.el[index];
	}

	return true;
}

MeshGeometryPart& MeshGeometry::getPart(MeshGeometryPartIndex::Type index) {

	return dref<MeshGeometryPart>(mParts.el[index]);
}

MeshGeometryPartBindings& MeshGeometry::getPartBindings(MeshGeometryPartIndex::Type index) {

	return (mPartBindings.el[index]);
}

bool MeshGeometry::hasUnifiedCoordSys() {

	return mUnifiedCoordSys.isValid();
}

const CoordSys& MeshGeometry::getUnifiedCoordSys() {

	return mUnifiedCoordSys;
}

void MeshGeometry::unifyCoordSys(CoordSysConvPool& convPool, const CoordSys* pCoordSys) {

	if (mUnifiedCoordSys.isValid()) {
		
		if (pCoordSys) {	

			if (mUnifiedCoordSys.equals(*pCoordSys)) {

				return;

			} else {

				mUnifiedCoordSys.set(*pCoordSys);
			}
	
		} else {

			return;
		}

	} else {

		mUnifiedCoordSys.set(*pCoordSys);
	}
	
	applyCoordSys(convPool, mUnifiedCoordSys);
}

void MeshGeometry::anchorGetOffset(const GeometryAnchorType* anchor, Vector3& anchorOffset) {
	
	AAB volume;

	extractVolume(volume, false);
	reanchorGetOffset(volume, anchor, anchorOffset.el);
}

/*
void MeshGeometry::transform(MeshGeometryTransform& transform) {

	if ((transform.hasAnchor) && reanchorIsAnchoring(transform.anchor)) {

		Vector3 anchorOffset;
		AAB volume;

		extractVolume(volume, false);
		reanchorGetOffset(volume, transform.anchor, anchorOffset.el);

		reanchor(anchorOffset.el);
	}

	if (transform.hasTransform) {

		applyTransform(&transform.transform);
	}
}
*/


void MeshGeometry::extractVolume(AAB& volume, bool cache) {

	if (mCachedVolume.isValid()) {

		volume = mCachedVolume.dref();
		return;
	}

	volume.empty();

	for (MeshGeometryPartIndex::Type i = 0; i < mParts.size; i++) {

		if(mParts.el[i]->hasVB()) {

			GeometryAnalyzer::addToVolume(mParts.el[i]->getVB(), mParts.el[i]->getIBPtr(), volume);
		}
	}
	if (cache) {

		MMemNew(mCachedVolume.ptrRef(), AAB());
		mCachedVolume.dref() = volume;
	}
}

void MeshGeometry::destroyCache(bool volumeCache) {

	mCachedVolume.conditionalDestroy(volumeCache);
}

}
