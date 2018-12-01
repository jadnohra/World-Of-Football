#ifndef _WEWorldMaterialManager_h
#define _WEWorldMaterialManager_h

#include "WEWorldMaterial.h"

namespace WE {

	class WorldMaterialManager : public Ref {
	public:

		WorldMaterialManager();
		virtual ~WorldMaterialManager() {};

		//if false is returned this material isnt a collision material
		bool getMaterialID(const StringHash& matName, WorldMaterialID& ID);
		bool isMaterial(const StringHash& matName);

		static void invalidateMaterialID(WorldMaterialID& ID);
		static bool isValidMaterialID(const WorldMaterialID& ID);

		bool putMaterial(const StringHash& matName, WorldMaterialID& ID);

		WorldMaterial* getMaterial(const WorldMaterialID& ID);
		WorldMaterialCombo* getCombinedMaterial(const WorldMaterialID& ID1, const WorldMaterialID& ID2);
	
	protected:

		WorldMaterialCombo* createCombinedMat(const WorldMaterialID& ID1, const WorldMaterialID& ID2);

	protected:
		

		typedef UINT32 CombinedHash;

		typedef WETL::HashMap<StringHash, WorldMaterialID, WorldMaterialID> MatNameMap;
		typedef WETL::PtrHashMap<WorldMaterialID, WorldMaterial*, WorldMaterialID> MatMap;
		typedef WETL::PtrHashMap<CombinedHash, WorldMaterialCombo*, WorldMaterialID> CombinedMap;

		WorldMaterialID mMatIdCounter;
		MatNameMap mNameMap;
		MatMap mMatMap;
		CombinedMap mCombinedMap;
	};

}

#endif