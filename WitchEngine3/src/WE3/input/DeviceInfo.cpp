#include "DeviceInfo.h"

namespace WE { namespace Input {

DeviceInfo::DeviceInfo(DeviceInfo* pRef) : index(0), cloneIndex(0), enable(true), deadZone(-1.0f) {

	if (pRef) {

		if (pRef->name.isValid()) {

			WE_MMemNew(name.ptrRef(), String(pRef->name));
		}


		if (pRef->profile.isValid()) {

			WE_MMemNew(profile.ptrRef(), String(pRef->profile));
		}

		index = pRef->index;
		cloneIndex = pRef->cloneIndex;
		enable = pRef->enable;
		deadZone = pRef->deadZone;
	}

}

void DeviceInfo::reset() {

	name.destroy();
	profile.destroy();

	index = 0;
	cloneIndex = 0;
	enable = true;
	deadZone = -1.0f;
}

} }