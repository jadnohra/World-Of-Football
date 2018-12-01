#include "WESpatialManager_KDTree.h"

#include "WESpatialManager_KDTreeBuilder.h"
#include "WESpatialObjectGroup_KDTree.h"

namespace WE {

SpatialManagerBuilder* SpatialManager_KDTree::createBuilder() {

	SoftPtr<SpatialManager_KDTreeBuilder> builder;

	MMemNew(builder.ptrRef(), SpatialManager_KDTreeBuilder(this));

	return builder;
}

void SpatialManager_KDTree::destroy() {

	mTreeRoot.destroy();
	mGlobalTriBuffer.destroy();
	mGlobalStaticVolumes.destroy();
}

bool SpatialManager_KDTree::addDynamicVolume(VolumeStruct& spatial, SpatialVolumeBinding* pBinding) {

	if (pBinding->isBoundData()) {

		assrt(false);
		return false;
	}

	SoftPtr<DynamicVolume> data;

	WE_MMemNew(data.ptrRef(), DynamicVolume());

	data->binding = pBinding;
	pBinding->bindData(data.ptr());

	SoftPtr<Volume> volume = updateStartDynamicVolume(pBinding);

	volume->init(spatial);

	return updateEndDynamicVolume(volume, pBinding);
}

Volume* SpatialManager_KDTree::updateStartDynamicVolume(SpatialVolumeBinding* pBinding) {

	SoftPtr<DynamicVolume> data = (DynamicVolume*) pBinding->data.ptr;

	return &data->volume();
}

bool SpatialManager_KDTree::updateEndDynamicVolume(Volume* pVolume, SpatialVolumeBinding* pBinding) {

	void* pListenerContext = NULL;
	SpatialDynamicListener* pListener = NULL;

	/*
	if (pListener.isValid()) {

		pListener->spatialStartUpdate(object, pListenerContext);
	}
	*/
	//object.spatialStartUpdate(pListenerContext);

	Volume& volume = dref(pVolume);
	SoftPtr<DynamicVolume> data = (DynamicVolume*) pBinding->data.ptr;
	SoftPtr<TreeNode> englobing;

	if (data->nodes.count) {

		for (NodePtrs::Index i = 0; i < data->nodes.count; ++i) {

			if (data->nodes.el[i]->englobes(volume)) {

				englobing = data->nodes.el[i];
				break;
			}
		}

		if (englobing.isValid()) {

			for (NodePtrs::Index i = 0; i < data->nodes.count; ++i) {

				if (data->nodes.el[i] != englobing.ptr()) {

					data->nodes.el[i]->leaf->remove(data->nodes.el[i], pListener, data, pListenerContext);
					--i;
									
					break;
				}
			}

			return true;
		}

		englobing = data->nodes.el[0]->parent;

		while (englobing.isValid()) {

			if (englobing->englobes(volume)) {

				break;
			}

			englobing = englobing->parent;
		}
	}

	if (!englobing.isValid()) {

		englobing = mTreeRoot;
	}

	data->clear(pListener, pListenerContext);
	englobing->recurseAdd(pListener, data, volume, pListenerContext);

	/*
	if (pListener.isValid()) {

		pListener->spatialEndUpdate(object, pListenerContext);
	}
	*/
	//object.spatialEndUpdate(pListenerContext);

	return true;
}

bool SpatialManager_KDTree::removeDynamicVolume(SpatialVolumeBinding* pBinding) {

	if (!pBinding->isBoundData()) {

		return false;
	}

	HardPtr<DynamicVolume> data = (DynamicVolume*) pBinding->data.ptr;
	
	pBinding->bindData(NULL);
	data.destroy();

	return true;
}

SpatialObjectGroup* SpatialManager_KDTree::createSpatialObjectGroup() {

	SoftPtr<SpatialObjectGroup_KDTree> group;

	MMemNew(group.ptrRef(), SpatialObjectGroup_KDTree());

	return group;
}

bool SpatialManager_KDTree::queryDynamicVolume(SpatialVolumeBinding* pBinding, VolumeStruct*& pOutVolume, SpatialObjectGroup& result) {

	SoftPtr<DynamicVolume> data = (DynamicVolume*) pBinding->data.ptr;

	pOutVolume = &pBinding->volume;

	result.reset();
	data->appendToQuery((SpatialObjectGroup_KDTree&) result);

	return true;
}

void SpatialManager_KDTree::render(Renderer& renderer) {

	if (mTreeRoot.isValid()) {

		mTreeRoot->render(renderer);
	}
}

}