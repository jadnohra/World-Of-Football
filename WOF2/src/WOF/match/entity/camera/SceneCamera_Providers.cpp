#include "SceneCamera_Providers.h"

namespace WOF { namespace match { namespace sceneCamera {

ControllerProvider_Distance_Impl::ControllerProvider_Distance_Impl()
	: mDistance(0.0f), mChanged(true) {
}

void ControllerProvider_Distance_Impl::setDistance(const Vector3& distance) {

	mChanged = !distance.equals(mDistance);
	mDistance = distance;
}

bool ControllerProvider_Distance_Impl::provideDistance(CamState& state, Vector3& distance) {
	
	distance = mDistance;

	bool changed = mChanged;
	mChanged = false;

	return changed;
}


} } }

