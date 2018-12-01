#ifndef _WECollMaterialManager_h
#define _WECollMaterialManager_h

#include "../WECollDataTypes.h"
#include "../../WETL/WETLHashMap.h"
#include "../../string/WEString.h"
#include "../../WERef.h"
#include "../../phys/WEPhysMaterial.h"
#include "../../audio/WEAudioEngine.h"

namespace WE {

	class CollMaterialManager : public Ref {
	public:

		class Mat {
		public:

			CollMaterial collMat;
			PhysMaterial physMat;

			HardRef<AudioBuffer> sound;
		};

		struct CombinedMat {

			PhysMaterial physMat;

			HardRef<AudioBuffer> sounds[2];

			void setCombinedSound(AudioBuffer* pSound);
		};

	public:

		CollMaterialManager();
		virtual ~CollMaterialManager() {};

		//if false is returned this material isnt a collision material
		bool getCollMaterial(const StringHash& matName, CollMaterial& mat);
		bool isCollMaterial(const StringHash& matName);

		static void invalidateCollMaterial(CollMaterial& mat);
		static bool isValidCollMaterial(const CollMaterial& mat);

		bool putCollMaterial(const StringHash& matName, CollMaterial& mat);

		CollMaterialManager::Mat* getMat(const CollMaterial& mat);
		CollMaterialManager::CombinedMat* getCombinedMat(const CollMaterial& mat1, const CollMaterial& mat2);
	
	protected:

		CollMaterialManager::CombinedMat* createCombinedMat(const CollMaterial& mat1, const CollMaterial& mat2);

	protected:
		

		typedef UINT32 CombinedHash;

		typedef WETL::HashMap<StringHash, CollMaterial, CollMaterial> MatNameMap;
		typedef WETL::PtrHashMap<CollMaterial, Mat*, CollMaterial> MatMap;
		typedef WETL::PtrHashMap<CombinedHash, CombinedMat*, CollMaterial> CombinedMap;

		CollMaterial mMatIdCounter;
		MatNameMap mNameMap;
		MatMap mMatMap;
		CombinedMap mCombinedMap;
	};

}

#endif