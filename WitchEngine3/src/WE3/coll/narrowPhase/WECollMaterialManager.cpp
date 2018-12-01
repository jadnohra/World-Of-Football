#include "WECollMaterialManager.h"

namespace WE {


void CollMaterialManager::invalidateCollMaterial(CollMaterial& mat) {

	mat = (CollMaterial) -1;
}

bool CollMaterialManager::isValidCollMaterial(const CollMaterial& mat) {

	return (mat != (CollMaterial) -1);
}

CollMaterialManager::CollMaterialManager() : mMatIdCounter(0) {

	assrt(sizeof(CollMaterial) == 16 / 8);
	assrt(sizeof(CombinedHash) == 32 / 8);
}

/*
bool CollMaterialManager::getCollMaterial(const TCHAR* matName, CollMaterial& mat) {

	return getCollMaterial(String::hash(matName), mat);
}
*/

bool CollMaterialManager::getCollMaterial(const StringHash& matName, CollMaterial& mat) {

	return (mNameMap.find(matName, mat));
}

bool CollMaterialManager::isCollMaterial(const StringHash& matName) {

	return mNameMap.hasKey(matName);
}

bool CollMaterialManager::putCollMaterial(const StringHash& matName, CollMaterial& mat) {

	if (getCollMaterial(matName, mat)) {

		assrt(false);
		return false;
	}

	Mat* pMat;
	MMemNew(pMat, Mat());
	pMat->collMat = mMatIdCounter++;
	mat = pMat->collMat;

	mNameMap.insert(matName, pMat->collMat);
	mMatMap.insert(pMat->collMat, pMat);

	return true;
}

CollMaterialManager::Mat* CollMaterialManager::getMat(const CollMaterial& mat) {

	Mat* pMat = NULL;

	mMatMap.find(mat, pMat);

	return pMat;
}

void CollMaterialManager::CombinedMat::setCombinedSound(AudioBuffer* pSound) {

	sounds[0] = pSound;
	sounds[1].destroy();
}

CollMaterialManager::CombinedMat* CollMaterialManager::createCombinedMat(const CollMaterial& mat1, const CollMaterial& mat2) {

	Mat* pMat1;
	Mat* pMat2;

	if (mMatMap.find(mat1, pMat1) && mMatMap.find(mat2, pMat2)) {

		CombinedMat* pCombined;
		MMemNew(pCombined, CombinedMat());

		physMatCombine(pMat1->physMat, pMat2->physMat, (pCombined->physMat));
		
		pCombined->sounds[0] = pMat1->sound.ptr();
		pCombined->sounds[1] = pMat2->sound.ptr();

		if (pCombined->sounds[0].isNull()) {

			pCombined->sounds[0] = pCombined->sounds[1].ptr();
			pCombined->sounds[1].destroy();
		}
		
		return pCombined;
	}

	return NULL;
}

CollMaterialManager::CombinedMat* CollMaterialManager::getCombinedMat(const CollMaterial& mat1, const CollMaterial& mat2) {

	CombinedHash hash = 0;
	
	if (mat1 < mat2) {

		hash += mat1;
		hash += ((CombinedHash) mat2) << 16;
	} else {

		hash += mat2;
		hash += ((CombinedHash) mat1) << 16;
	}

	CombinedMat* pMat;

	if (mCombinedMap.find(hash, pMat)) {

		return pMat;
	}

	pMat = createCombinedMat(mat1, mat2);

	if (pMat) {

		mCombinedMap.insert(hash, pMat);
	} 

	return pMat;
}

}