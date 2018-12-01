#include "SceneCamera_ControllerGroup.h"


namespace WOF { namespace match { namespace sceneCamera {

ControllerGroup::~ControllerGroup() {
}

void ControllerGroup::init(BufferString& tempStr, DataChunk& chunk, ControllerTable* pTable) {

	SoftRef<DataChunk> child = chunk.getFirstChild();

	while(child.isValid()) {

		if (child->getAttribute(L"name", tempStr)) {

			if (pTable) {

				SoftRef<ControllerBase> target;
			
				pTable->find(tempStr.hash(), target.ptrRef());

				add(target);
			}
		}

		toNextSibling(child);
	}
}

void ControllerGroup::add(ControllerBase* pController) {

	if (pController) {

		pController->AddRef();
		mControllers.addOne(pController);
	}
}

void ControllerGroup::forceStarted(CamState& state) {

	for (Controllers::Index i = 0; i < mControllers.count; ++i) {

		mControllers.el[i]->forceStarted(state);
	}
}

void ControllerGroup::start(CamState& state) {

	for (Controllers::Index i = 0; i < mControllers.count; ++i) {

		mControllers.el[i]->start(state);
	}
}

void ControllerGroup::stop(CamState& state) {

	for (Controllers::Index i = 0; i < mControllers.count; ++i) {

		mControllers.el[i]->stop(state);
	}
}

void ControllerGroup::update(CamState& state) {

	for (Controllers::Index i = 0; i < mControllers.count; ++i) {

		mControllers.el[i]->update(state);
	}
}

void ControllerGroup::getModifs(Vector3& eyePosModif, Vector3& targetPosModif, Vector3& targetOffsetModif) {

	for (Controllers::Index i = 0; i < mControllers.count; ++i) {

		mControllers.el[i]->getModifs(eyePosModif, targetPosModif, targetOffsetModif);
	}
}

void ControllerGroup::getPostModifs(Vector3& postModif) {

	for (Controllers::Index i = 0; i < mControllers.count; ++i) {

		mControllers.el[i]->getPostModifs(postModif);
	}
}

} } }