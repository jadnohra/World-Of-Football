#include "WEWorldMaterialManager.h"

namespace WE {

void WorldMaterialManager::invalidateMaterialID(WorldMaterialID& ID) {

	ID = (WorldMaterialID) -1;
}

bool WorldMaterialManager::isValidMaterialID(const WorldMaterialID& ID) {

	return (ID != (WorldMaterialID) -1);
}

WorldMaterialManager::WorldMaterialManager() : mMatIdCounter(0) {

	assrt(sizeof(WorldMaterialID) == 16 / 8);
	assrt(sizeof(CombinedHash) == 32 / 8);
}

/*
bool WorldMaterialManager::getWorldMaterial(const TCHAR* matName, WorldMaterial& mat) {

	return getWorldMaterial(String::hash(matName), mat);
}
*/

bool WorldMaterialManager::getMaterialID(const StringHash& matName, WorldMaterialID& ID) {

	return (mNameMap.find(matName, ID));
}

bool WorldMaterialManager::isMaterial(const StringHash& matName) {

	return mNameMap.hasKey(matName);
}

bool WorldMaterialManager::putMaterial(const StringHash& matName, WorldMaterialID& ID) {

	if (getMaterialID(matName, ID)) {

		assrt(false);
		return false;
	}

	WorldMaterial* pMat;
	MMemNew(pMat, WorldMaterial());
	pMat->ID = mMatIdCounter++;
	ID = pMat->ID;

	mNameMap.insert(matName, pMat->ID);
	mMatMap.insert(pMat->ID, pMat);

	return true;
}

WorldMaterial* WorldMaterialManager::getMaterial(const WorldMaterialID& ID) {

	WorldMaterial* pMat = NULL;

	mMatMap.find(ID, pMat);

	return pMat;
}

WorldMaterialCombo* WorldMaterialManager::createCombinedMat(const WorldMaterialID& ID1, const WorldMaterialID& ID2) {

	WorldMaterial* pMat1;
	WorldMaterial* pMat2;

	if (mMatMap.find(ID1, pMat1) && mMatMap.find(ID2, pMat2)) {

		WorldMaterialCombo* pCombined;
		MMemNew(pCombined, WorldMaterialCombo());

		pCombined->mats[0] = pMat1;
		pCombined->mats[1] = pMat2;

		if (pMat1->physProperties.isValid() && pMat2->physProperties.isValid()) {

			if (!pCombined->physProperties.isValid()) {

				WE_MMemNew(pCombined->physProperties.ptrRef(), PhysMaterial());
			}

			physMatCombine(pMat1->physProperties, pMat2->physProperties, (pCombined->physProperties));

		} else {

			pCombined->physProperties.destroy();
		}


		pCombined->useCombinedSound = false;

		return pCombined;
	}

	return NULL;
}

WorldMaterialCombo* WorldMaterialManager::getCombinedMaterial(const WorldMaterialID& ID1, const WorldMaterialID& ID2) {

	CombinedHash hash = 0;
	
	if (ID1 < ID2) {

		hash += ID1;
		hash += ((CombinedHash) ID2) << 16;
	} else {

		hash += ID2;
		hash += ((CombinedHash) ID1) << 16;
	}

	WorldMaterialCombo* pMat;

	if (mCombinedMap.find(hash, pMat)) {

		return pMat;
	}

	pMat = createCombinedMat(ID1, ID2);

	if (pMat) {

		mCombinedMap.insert(hash, pMat);
	} 

	return pMat;
}

}