#include "WESceneBase.h"


namespace WE {

SceneBase::SceneBase() : mIsInited(false) {
}

SceneBase::~SceneBase() {
}

bool SceneBase::baseInit(const CoordSys& coordSys, LoadContext& loadContext) {

	if (mIsInited) {

		assrt(false);
		return true;
	}

	mCoordSys.set(coordSys);

	if (mCoordSys.isValid() == false) {

		assrt(false);
	}

	if (mCoordSys.hasUnitsPerMeter() == false) {

		assrt(false);
	}

	mCoordSys.getUnitVector(CSD_Right, mCoordSysAxis[Scene_Right].el);
	mCoordSys.getUnitVector(CSD_Up, mCoordSysAxis[Scene_Up].el);
	mCoordSys.getUnitVector(CSD_Forward, mCoordSysAxis[Scene_Forward].el);

	mCoorSysRotLeftHanded = mCoordSys.rotationLeftHanded;


	mLoadContext = &loadContext;
	
	if (true) {

		assrt(mMeshes.isNull());
		mMeshes.destroy();

		MMemNew(mMeshes.ptrRef(), SceneMeshContainer());
	}

	mIsInited = true;

	return true;
}

Renderer& SceneBase::renderer() {

	return dref<Renderer>(mLoadContext->render().rendererPtr());
}

}