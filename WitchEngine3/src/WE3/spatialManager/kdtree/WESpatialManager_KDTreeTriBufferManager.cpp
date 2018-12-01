#include "WESpatialManager_KDTreeTriBufferManager.h"
#include "../WESpatialObjects.h"

namespace WE {

void SpatialManager_KDTreeTriBufferManager::add(SpatialTriContainer& spatial, ObjectBinding* pBinding) {

	SoftPtr<TriBuffer> buffer = mTriBuffers.addNewOne();

	buffer->binding = pBinding;

	buffer->append(spatial);
}


}