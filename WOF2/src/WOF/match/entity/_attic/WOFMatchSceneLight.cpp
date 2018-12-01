#include "WOFMatchSceneLight.h"

#include "WE3/binding/directx/9/d3d9/renderer/WERendererD3D9Convert.h"
#include "WE3/helper/load/WEMathDataLoadHelper.h"
using namespace WE;

#include "../WOFMatch.h"

namespace WOF {

bool MatchSceneLight::_init_settings(BufferString& tempStr, DataChunk& chunk, CoordSysConv* pConv) {

	/*
	if (chunk.scanAttribute(tempStr, L"fov", L"%f", &mSettings.FOV) == false) {

		mSettings.FOV = 45.0f;
	}
	mSettings.FOV = degToRad(mSettings.FOV);

	bool rangeLoaded = false;

	if (chunk.getAttribute(L"range", tempStr)) {

		if (_stscanf(tempStr.c_tstr(), L"%f, %f", &mSettings.rangeMin, &mSettings.rangeMax) == 2) {

			rangeLoaded = true;
		}
	}

	if (rangeLoaded == false) {

		mSettings.rangeMin = 0.01f;
		mSettings.rangeMax = 1000.0f;

		assrt(false);
	}

	if (chunk.scanAttribute(tempStr, L"velocity", L"%f", &mSettings.velocity) == false) {

		mSettings.velocity = 1.0f;
	}

	if (chunk.scanAttribute(tempStr, L"rotation", L"%f", &mSettings.rotation) == false) {

		mSettings.rotation = 1.0f;
	}

	if (MathDataLoadHelper::extract(tempStr, chunk, mTrackOffset, true, true, true, L"trackOffset") == false) {

		mTrackOffset.zero();
	}

	if (MathDataLoadHelper::extract(tempStr, chunk, mLookAtOffset, true, true, true, L"lookAtOffset") == false) {

		mLookAtOffset.zero();
	}

	
	if (pConv) {

		pConv->toTargetUnits(mSettings.rangeMin);
		pConv->toTargetUnits(mSettings.rangeMax);
		pConv->toTargetUnits(mSettings.velocity);
		pConv->toTargetPoint(mTrackOffset.el);
		pConv->toTargetPoint(mLookAtOffset.el);
	}
	*/

	return true;
}

bool MatchSceneLight::_init_lookAt(BufferString& tempStr, DataChunk& chunk, CoordSysConv* pConv) {

	MathDataLoadHelper::extract(tempStr, chunk, mEye, true, true, false, L"pos");
	
	if (MathDataLoadHelper::extract(tempStr, chunk, mLookAt, true, true, false, L"lookAt")) {
	} if (MathDataLoadHelper::extract(tempStr, chunk, mLookAt, true, true, false, L"direction")) {

	} else {

		log(L"missing direction for camera [%s]", getNodeName());

		assrt(false);
		return false;
	}
	

	if (pConv) {

		pConv->toTargetPoint(mEye.el);
		pConv->toTargetPoint(mLookAt.el);
	}

	return true;
}

MatchSceneLight::MatchSceneLight() {

}

bool MatchSceneLight::_init_camera() {

	Renderer& renderer = mNodeMatch->getRenderer();

	/*
	mLight.SetEnablePositionMovement(true);
	mLight.SetEnableYAxisMovement(true);
	mLight.SetScalers(0.003f * mSettings.rotation, mSettings.velocity);
	*/

	return true;
}

bool MatchSceneLight::init_create(Match& match, BufferString& tempStr, DataChunk& chunk, CoordSysConv* pConv) {

	bool initSuccess = true;

	chunk.getValue(tempStr);

	initSuccess = initSuccess &&  nodeInit(match, MNT_Light, match.genNodeId(), tempStr.c_tstr());
	initSuccess = initSuccess &&  _init_settings(tempStr, chunk, pConv);

	return initSuccess;
}

bool MatchSceneLight::init_nodeAttached(BufferString& tempStr, DataChunk& chunk, CoordSysConv* pConv) {

	_init_lookAt(tempStr, chunk, pConv);
	_init_camera();

	return true;
}


void MatchSceneLight::init_prepareScene() {
}

void MatchSceneLight::clean() {

	bool doUpdate = false;

	if (mNodeLocalTransformIsDirty) {

		doUpdate = true;

		assrt(pParent->mNodeLocalTransformIsDirty == false);
		transformUpdateWorld(pParent->mTransformWorld);

	} else if (mNodeWorldTransformChangedFlag) {

		doUpdate = true;
	}

	if (doUpdate) {
		
		mNodeLocalTransformIsDirty = false;
		mNodeWorldTransformChangedFlag = false;
	}
}


void MatchSceneLight::setLight(Renderer& renderer, bool cleanIfNeeded, RenderPassEnum pass) {

	if (pass != RP_Normal) {

		return;
	}

	if (cleanIfNeeded) {
		
		clean();
	}


	/*
	RendererD3D9_convert(*mLight.GetProjMatrix(), mMatProj);
	renderer.setTransform(mMatProj, RT_Projection);

	RendererD3D9_convert(*mLight.GetViewMatrix(), mMatView);
	renderer.setTransform(mMatView, RT_View);
	*/
}

}