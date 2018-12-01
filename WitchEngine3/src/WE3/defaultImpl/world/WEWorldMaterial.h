#ifndef _WEWorldMaterial_h
#define _WEWorldMaterial_h

#include "../../string/WEString.h"
#include "../../WERef.h"
#include "../../phys/WEPhysMaterial.h"
#include "../../audio/WEAudioEngine.h"

namespace WE {

	typedef UINT16 WorldMaterialID; //need for hashing combo's in material manager

	class MaterialTrigger {
	public:

		virtual ~MaterialTrigger() {}

		virtual unsigned int getID() const { return -1; }
		virtual void execute(const Vector3* pPos) {}
	};

	struct WorldMaterial {
	
		WorldMaterialID ID;
		HardPtr<String> name;
		HardPtr<PhysMaterial> physProperties; //NULL if the material is trigger-only
		
		HardRef<AudioBuffer> sound;
		HardPtr<MaterialTrigger> trigger;
	};

	struct WorldMaterialCombo {

		SoftPtr<WorldMaterial> mats[2];

		HardPtr<PhysMaterial> physProperties; //NULL if the material is trigger-only

		bool useCombinedSound;
		HardRef<AudioBuffer> combinedSound;

		WorldMaterialCombo() : useCombinedSound(false) {}

		void setCombinedSound(AudioBuffer* pSound) {

			useCombinedSound = true;
			combinedSound = pSound;
		}
	};

}

#endif