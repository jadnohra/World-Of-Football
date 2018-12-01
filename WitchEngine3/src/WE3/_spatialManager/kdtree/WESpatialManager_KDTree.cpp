#include "WESpatialManager_KDTree.h"

#include "../../math/WEEnglobe.h"

namespace WE {

void SpatialManager_KDTree::destroy() {

	mTreeRoot.destroy();
	mTriObjects.destroy();
	mVolumes.destroy();
}

SpatialManagerBuilder* SpatialManager_KDTree::createBuilder() {

	SoftPtr<SpatialManagerBuilder> builder;

	WE_MMemNew(builder.ptrRef(), SpatialManagerBuilder_KDTree(this));

	return builder;
}

bool SpatialManager_KDTree::add(SpatialDynamicVolume& object) {

	if (object.isBoundData()) {

		assrt(false);
		return false;
	}

	SoftPtr<DynamicVolumeData> data;

	WE_MMemNew(data.ptrRef(), DynamicVolumeData());

	object.bindData(data.ptr());

	update(object);

	return true;
}

bool SpatialManager_KDTree::update(SpatialDynamicVolume& object) {

	void* pListenerContext = NULL;

	/*
	if (mListener.isValid()) {

		mListener->spatialStartUpdate(object, pListenerContext);
	}
	*/
	//object.spatialStartUpdate(pListenerContext);

	SoftPtr<DynamicVolumeData> data = (DynamicVolumeData*) object.data.ptr;
	SoftPtr<TreeNode> englobing;
	const Volume& volume = object.spatialGetVolume();

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

					data->nodes.el[i]->leaf->remove(data->nodes.el[i], mListener, object, data, pListenerContext);
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

	data->clear(mListener, object, pListenerContext);
	englobing->recurseAdd(mListener, object, data, volume, pListenerContext);

	/*
	if (mListener.isValid()) {

		mListener->spatialEndUpdate(object, pListenerContext);
	}
	*/
	//object.spatialEndUpdate(pListenerContext);

	return true;
}

bool SpatialManager_KDTree::remove(SpatialDynamicVolume& object) {

	if (!object.isBoundData()) {

		return false;
	}

	HardPtr<DynamicVolumeData> data = (DynamicVolumeData*) object.data.ptr;
	object.bindData(NULL);
	data.destroy();

	return true;
}

SpatialManager_KDTree::Index SpatialManager_KDTree::getCellCount(SpatialDynamicVolume& object) {

	SoftPtr<DynamicVolumeData> data = (DynamicVolumeData*) object.data.ptr;

	return data->nodes.count;
}

SpatialManagerCell& SpatialManager_KDTree::getCell(SpatialDynamicVolume& object, const Index& cellIndex) {

	SoftPtr<DynamicVolumeData> data = (DynamicVolumeData*) object.data.ptr;

	return data->nodes.el[cellIndex]->leaf.dref();
}

void SpatialManager_KDTree::DynamicVolumeData::clear(SpatialEventListener* pListener, SpatialDynamicVolume& object, void* pListenerContext) {

	for (NodePtrs::Index i = 0; i < nodes.count; ++i) {

		nodes.el[i]->leaf->onClear(object);
	}

	nodes.count = 0;

	/*
	if (pListener)
		pListener->spatialClearCells(object, pListenerContext);
		*/
	//object.spatialClearCells(pListenerContext);
}

bool SpatialManager_KDTree::TreeNode::englobes(const Volume& testVolume) {

	return WE::englobes(volume, testVolume);
}

void SpatialManager_KDTree::TreeNode::recurseAdd(SpatialEventListener* pListener, SpatialDynamicVolume& object, DynamicVolumeData& data, const Volume& volume, void* pListenerContext) {

	if (branch.isValid()) {

		switch (volume.classify(branch->plane)) {

			case PP_Back: {

				branch->child[PP_Back].recurseAdd(pListener, object, data, volume, pListenerContext);

			} break;

			case PP_Front: {

				branch->child[PP_Front].recurseAdd(pListener, object, data, volume, pListenerContext);

			} break;

			default: {

				branch->child[PP_Back].recurseAdd(pListener, object, data, volume, pListenerContext);
				branch->child[PP_Front].recurseAdd(pListener, object, data, volume, pListenerContext);
			}
		}

	} else {

		leaf->add(this, pListener, object, data, pListenerContext);
	}
}

void SpatialManager_KDTree::TreeNodeLeaf::add(TreeNode* pOwner, SpatialEventListener* pListener, SpatialDynamicVolume& object, DynamicVolumeData& data, void* pListenerContext) {

	mDynamicVolumes.addOne(&object);
	data.nodes.addOne(pOwner);

	/*
	if (pListener) {

		pListener->spatialEnteredCell(object, *this, pListenerContext);
	}
	*/
	//object.spatialEnteredCell(*this, pListenerContext);
}

void SpatialManager_KDTree::TreeNodeLeaf::remove(TreeNode* pOwner, SpatialEventListener* pListener, SpatialDynamicVolume& object, DynamicVolumeData& data, void* pListenerContext) {

	mDynamicVolumes.searchRemoveSwapWithLast(&object);
	data.nodes.searchRemoveSwapWithLast(pOwner);

	/*
	if (pListener) {

		pListener->spatialLeftCell(object, *this, pListenerContext);
	}
	*/
	//object.spatialLeftCell(*this, pListenerContext);
}

void SpatialManager_KDTree::TreeNodeLeaf::onClear(SpatialDynamicVolume& object) {

	mDynamicVolumes.searchRemoveSwapWithLast(&object);
}

void SpatialManager_KDTree::render(Renderer& renderer, 
								bool renderMain, const RenderColor* pMainColor, 
								bool renderContainers, const RenderColor* pContainersColor, 
								bool renderDetails, const RenderColor* pDetailColor) {

	if (mTreeRoot.isValid()) {

		mTreeRoot->render(renderer, renderMain, pMainColor, 
							renderContainers, pContainersColor, 
							renderDetails, pDetailColor, true);
	}
}

}