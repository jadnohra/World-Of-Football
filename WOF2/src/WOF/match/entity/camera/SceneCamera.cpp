#include "SceneCamera.h"

#include "windowsx.h"
#include "WE3/binding/directx/9/d3d9/renderer/WERendererD3D9Convert.h"
#include "WE3/helper/load/WEMathDataLoadHelper.h"
using namespace WE;

#include "../../Match.h"
#include "../../Player.h"
#include "../footballer/Footballer.h"

#include "SceneCamera_ControllerBufferZone.h"
#include "SceneCamera_ControllerPointTrajectory.h"
#include "SceneCamera_ControllerGroup.h"
#include "SceneCamera_ActivatorTrend.h"
#include "SceneCamera_ControllerConstraint.h"
#include "SceneCamera_ControllerSmoother.h"

#include "../SceneEntityMesh.h"

namespace WOF { namespace match {

SceneCamera::~SceneCamera() {

	/*
	if (mCamPointCollider.isValid()) {

		CollEngine& collEng = mNodeMatch->getCollEngine();

		if (mCamPointCollider->collider.isInited(*this, collEng)) {

			collEng.destroyRegistry(mCamPointCollider->collRegistry);
			mCamPointCollider->collider.destroy(*this, collEng);
		}
	}
	*/
}

void SceneCamera::notifySwitchedFrom(SceneCamera* pOldCam) {

	if (pOldCam) {

		track(pOldCam->getTrackedNode(), pOldCam->mState.track.settings.constrainUp ? &pOldCam->mState.track.settings.constrainUpValue : NULL);
		track(pOldCam->getTrackedPlayer(), pOldCam->mState.track.settings.constrainUp ? &pOldCam->mState.track.settings.constrainUpValue : NULL);
	}
}

void SceneCamera::Settings::updateFOVs(const RenderViewportInfo& viewport) {

	if (FOVwIsRef) {

		FOVh = 2.0f * atanf( tanf(FOVw * 0.5f) * viewport.height / viewport.width);

	} else {

		FOVw = 2.0f * atanf( tanf(FOVh * 0.5f) * viewport.width / viewport.height);
	}
}

bool SceneCamera::_init_settings(BufferString& tempStr, DataChunk& camChunk, CoordSysConv* pConv) {

	mState.coordSysConv = pConv;

#ifdef _DEBUG
	mIsInitedSettings = true;
#endif

	mFrustumIsDirty = true;

	bool fovhSet = false;

	mSettings.FOVwIsRef = false;

	if (camChunk.scanAttribute(tempStr, L"fovh", L"%f", &mSettings.FOVh)) {

		fovhSet = true;
	}

	if (camChunk.scanAttribute(tempStr, L"fovw", L"%f", &mSettings.FOVw)) {

		mSettings.FOVwIsRef = true;
	}

	if (camChunk.scanAttribute(tempStr, L"fov", L"%f", &mSettings.FOVw)) {

		mSettings.FOVwIsRef = true;
	}

	if (!fovhSet && !mSettings.FOVwIsRef) {

		mSettings.FOVw = 45.0f;
		mSettings.FOVwIsRef = true;
	}

	mSettings.FOVw = degToRad(mSettings.FOVw);
	mSettings.FOVh = degToRad(mSettings.FOVh);

	

	bool rangeLoaded = false;

	if (camChunk.getAttribute(L"range", tempStr)) {

		if (_stscanf(tempStr.c_tstr(), L"%f, %f", &mSettings.rangeMin, &mSettings.rangeMax) == 2) {

			rangeLoaded = true;
		}
	}

	if (rangeLoaded == false) {

		mSettings.rangeMin = 0.01f;
		mSettings.rangeMax = 1000.0f;

		assrt(false);
	}

	if (camChunk.scanAttribute(tempStr, L"velocity", L"%f", &mSettings.velocity) == false) {

		mSettings.velocity = 1.0f;
	}

	if (camChunk.scanAttribute(tempStr, L"rotation", L"%f", &mSettings.rotation) == false) {

		mSettings.rotation = 1.0f;
	}


	if (MathDataLoadHelper::extract(tempStr, camChunk, mTrackOffset, true, true, true, L"trackOffset") == false) {

		mTrackOffset.zero();
	}

	
	if (MathDataLoadHelper::extract(tempStr, camChunk, mLookAtOffset, true, true, true, L"lookAtOffset") == false) {

		mLookAtOffset.zero();
	}

	
	SoftRef<DataChunk> controllersChunk = camChunk.getChild(L"controllers");

	if (controllersChunk.isValid()) {

		sceneCamera::ControllerTable table;

		SoftRef<DataChunk> controllerChunk = controllersChunk->getFirstChild();

		while (controllerChunk.isValid()) {

			if (controllerChunk->getAttribute(L"type", tempStr)) {

				SoftRef<sceneCamera::ControllerBase> contRef;
				SoftPtr<sceneCamera::ControllerBase> bindCandidate;

				if (tempStr.equals(L"bufferZone")) {

					SoftPtr<sceneCamera::Controller_BufferZone> cont;

					MMemNew(cont.ptrRef(), sceneCamera::Controller_BufferZone());
					contRef = cont.ptr();

					cont->init(tempStr, controllerChunk, pConv);

					bindCandidate = cont.ptr();

					if (controllerChunk->getAttribute(L"gameBind", tempStr)) {

						if (tempStr.equals(L"bufferZone")) {

							mBoundBufferZone = cont.ptr();
						}
					}

				} else if (tempStr.equals(L"pointTrajectory")) {

					SoftPtr<sceneCamera::Controller_PointTrajectory> cont;

					MMemNew(cont.ptrRef(), sceneCamera::Controller_PointTrajectory());
					contRef = cont.ptr();

					cont->init(tempStr, controllerChunk, pConv);

					bindCandidate = cont.ptr();

				} else if (tempStr.equals(L"controllerGroup")) {

					SoftPtr<sceneCamera::ControllerGroup> cont;

					MMemNew(cont.ptrRef(), sceneCamera::ControllerGroup());
					contRef = cont.ptr();

					cont->init(tempStr, controllerChunk, &table);

					bindCandidate = cont.ptr();

				} else if (tempStr.equals(L"trendActivator")) {

					SoftPtr<sceneCamera::Controller_ActivatorTrend> cont;

					MMemNew(cont.ptrRef(), sceneCamera::Controller_ActivatorTrend());
					contRef = cont.ptr();

					cont->init(mNodeMatch, tempStr, controllerChunk, pConv, &table);

					bindCandidate = cont.ptr();

				} else if (tempStr.equals(L"constraint")) {

					SoftPtr<sceneCamera::Controller_Constraint> cont;

					MMemNew(cont.ptrRef(), sceneCamera::Controller_Constraint());
					contRef = cont.ptr();

					cont->init(tempStr, controllerChunk, pConv);

					bindCandidate = cont.ptr();

				} else if (tempStr.equals(L"smoother")) {

					SoftPtr<sceneCamera::Controller_Smoother> cont;

					MMemNew(cont.ptrRef(), sceneCamera::Controller_Smoother());
					contRef = cont.ptr();

					cont->init(tempStr, controllerChunk, pConv);

					bindCandidate = cont.ptr();
				}

				if (bindCandidate.isValid()) {

					if (controllerChunk->getAttribute(L"gameBind", tempStr)) {

						if (tempStr.equals(L"controller")) {

							mBoundController = bindCandidate.ptr();
						}

						if (tempStr.equals(L"postController")) {

							mBoundPostController = bindCandidate.ptr();
						}
					}

					if (controllerChunk->getAttribute(L"name", tempStr)) {

						table.insert(tempStr.hash(), bindCandidate.ptr());
					}
				}

				contRef.destroy();
			}

			toNextSibling(controllerChunk);
		}
	}
	
	if (pConv) {

		pConv->toTargetUnits(mSettings.rangeMin);
		pConv->toTargetUnits(mSettings.rangeMax);
		pConv->toTargetUnits(mSettings.velocity);
		pConv->toTargetPoint(mTrackOffset.el);
		pConv->toTargetPoint(mLookAtOffset.el);
	}

	mOrigTrackOffset = mTrackOffset;

	SoftRef<DataChunk> limitChunk = camChunk.getChild(L"limit");

	if (limitChunk.isValid()) {

		bool enable = false;

		limitChunk->scanAttribute(tempStr, L"enable", enable);

		float lookAtCollRadius = 0.0f;

		if (enable) {

			if (limitChunk->scanAttribute(tempStr, L"lookAtCollRadius", L"%f", &lookAtCollRadius)) {

				if (pConv) 
					pConv->toTargetUnits(lookAtCollRadius);
				
			} else {

				assrt(false);
				enable = false;
			}
		}

		if (mBoundController.isValid() && enable) {

			/*
			CollMask collMask;
			CollEngine::initCollMaskCameraPoint(collMask);

			MMemNew(mCamPointCollider.ptrRef(), CamPointCollider());

			mCamPointCollider->radius = lookAtCollRadius;
			
			if (!mCamPointCollider->collider.init(*this, mNodeMatch->getCollEngine(), collMask)) {

				mCamPointCollider.destroy();
			}
			*/
		}
	}

	return true;
}

bool SceneCamera::_init_lookAt(BufferString& tempStr, DataChunk& camChunk, CoordSysConv* pConv) {

	if (MathDataLoadHelper::extract(tempStr, camChunk, mEye, true, true, true, L"eye") == false) {

		MathDataLoadHelper::extract(tempStr, camChunk, mEye, true, true, false, L"pos");
	}

	MathDataLoadHelper::extract(tempStr, camChunk, mLookAt, true, true, false, L"lookAt");
	

	if (pConv) {

		pConv->toTargetPoint(mEye.el);
		pConv->toTargetPoint(mLookAt.el);
	}

	return true;
}

SceneCamera::SceneCamera() {

	//mSettings.trackTechnique = TT_Tech1;

#ifdef _DEBUG
	mIsInitedSettings = false;
#endif

	mState.parent = this;
}

bool SceneCamera::_init_camera() {

#ifdef _DEBUG
	assrt(mIsInitedSettings == true);
#endif

	Renderer& renderer = mNodeMatch->getRenderer();

	mCamera.SetEnablePositionMovement(true);
	mCamera.SetEnableYAxisMovement(true);
	mCamera.SetScalers(0.003f * mSettings.rotation, mSettings.velocity);

	return true;
}

bool SceneCamera::init_create(Match& match, BufferString& tempStr, DataChunk& chunk, CoordSysConv* pConv) {

	bool initSuccess = true;

	chunk.getValue(tempStr);

	initSuccess = initSuccess &&  nodeInit(match, Node_Camera, match.genNodeId(), tempStr.c_tstr());
	initSuccess = initSuccess &&  _init_settings(tempStr, chunk, pConv);

	return initSuccess;
}

bool SceneCamera::init_nodeAttached(BufferString& tempStr, DataChunk& chunk, CoordSysConv* pConv) {

	_init_lookAt(tempStr, chunk, pConv);
	_init_camera();

	return true;
}


void SceneCamera::init_prepareScene() {

	//Renderer& renderer = mNodeMatch->getRenderer();
	//DXUT::setupCamera(mCamera, renderer, mSettings.FOV, mSettings.rangeMin, mSettings.rangeMax, mEye, mLookAt);

	resetProjParams();

	D3DXVECTOR3 lookat_vector = D3DXVECTOR3(MExpand3(mLookAt));
	D3DXVECTOR3 eye_vector = D3DXVECTOR3(MExpand3(mEye));

	mCamera.SetViewParams(&eye_vector, &lookat_vector);
}

void SceneCamera::updateFrustum() {

	if (mFrustumIsDirty) {

		mFrustumIsDirty = false;

		mFrustum.extents[Frustum::Near] = mSettings.rangeMin;
		mFrustum.extents[Frustum::Far] = mSettings.rangeMax;

		mFrustum.extents[Frustum::Left] = tanf(mSettings.FOVw * 0.5f) * mSettings.rangeMin;
		mFrustum.extents[Frustum::Up] = tanf(mSettings.FOVh * 0.5f) * mSettings.rangeMin;

		mFrustum.coordFrame.row[Frustum::Origin] = mTransformWorld.getPosition();
		mFrustum.coordFrame.row[Frustum::LVector] = mTransformWorld.row[Scene_Right];
		mFrustum.coordFrame.row[Frustum::LVector].negate();
		mFrustum.coordFrame.row[Frustum::DVector] = mTransformWorld.row[Scene_Forward];
		mFrustum.coordFrame.row[Frustum::UVector] = mTransformWorld.row[Scene_Up];
		
		mFrustum.invalidate();

	
		{
			const Plane& terrainPlane = mNodeMatch->getTerrainPlane();

			float u;

			mIsValidFrustTerrainLookAtPoint = intersect(terrainPlane, mTransformWorld.getPosition(), mTransformWorld.row[Scene_Forward], u, &mFrustTerrainLookAtPoint);
		}

		{
			const Plane& terrainPlane = mNodeMatch->getTerrainPlane();

			mFrustum.updateExtraPoints();

			bool allValid = true;
			Vector3 dir[4];
			float u[4];

			int pointIndex = 0;
			int validIndex = 0;

			const Vector3& origin = mFrustum.origin();

			mFrustum.extraPoints[Frustum::EP_NearLeftUp].subtract(origin, dir[pointIndex]);
			if (intersect(terrainPlane, origin, dir[pointIndex], u[pointIndex], &mFrustTerrainIntersectionPoints[pointIndex])) {

				mFrustTerrainIntersectionPointValid[pointIndex] = (u[pointIndex] >= 0.0f);

				if (mFrustTerrainIntersectionPointValid[pointIndex])
					++validIndex;
			}
			++pointIndex;

			mFrustum.extraPoints[Frustum::EP_NearRightUp].subtract(origin, dir[pointIndex]);
			if (intersect(terrainPlane, origin, dir[pointIndex], u[pointIndex], &mFrustTerrainIntersectionPoints[pointIndex])) {

				mFrustTerrainIntersectionPointValid[pointIndex] = (u[pointIndex] >= 0.0f);

				if (mFrustTerrainIntersectionPointValid[pointIndex])
					++validIndex;
			}
			++pointIndex;

			mFrustum.extraPoints[Frustum::EP_NearRightDown].subtract(origin, dir[pointIndex]);
			if (intersect(terrainPlane, origin, dir[pointIndex], u[pointIndex], &mFrustTerrainIntersectionPoints[pointIndex])) {

				mFrustTerrainIntersectionPointValid[pointIndex] = (u[pointIndex] >= 0.0f);

				if (mFrustTerrainIntersectionPointValid[pointIndex])
					++validIndex;
			}
			++pointIndex;

			mFrustum.extraPoints[Frustum::EP_NearLeftDown].subtract(origin, dir[pointIndex]);
			if (intersect(terrainPlane, origin, dir[pointIndex], u[pointIndex], &mFrustTerrainIntersectionPoints[pointIndex])) {

				mFrustTerrainIntersectionPointValid[pointIndex] = (u[pointIndex] >= 0.0f);

				if (mFrustTerrainIntersectionPointValid[pointIndex])
					++validIndex;
			}
			++pointIndex;

			if (validIndex > 0 && validIndex < 4) {

				for (pointIndex = 0; pointIndex < 4; ++pointIndex) {

					dir[pointIndex].normalize();
				}

				for (pointIndex = 0; pointIndex < 4; ++pointIndex) {

					if (!mFrustTerrainIntersectionPointValid[pointIndex]) {

						int refIndex; 
						Vector3 tempVect;
						Vector3 planeParallelVect;
						bool updateDir = false;

						refIndex = (pointIndex + 3) % 4;

						if (mFrustTerrainIntersectionPointValid[refIndex]) {

							terrainPlane.normal.cross(dir[refIndex], tempVect);
							tempVect.cross(terrainPlane.normal, planeParallelVect);

							updateDir = true;
						}

						if (!updateDir) {

							refIndex = (pointIndex + 1) % 4;

							if (mFrustTerrainIntersectionPointValid[refIndex]) {

								terrainPlane.normal.cross(dir[refIndex], tempVect);
								tempVect.cross(terrainPlane.normal, planeParallelVect);

								updateDir = true;
							}
						}

						if (!updateDir) {

							refIndex = (pointIndex + 2) % 4;

							if (mFrustTerrainIntersectionPointValid[refIndex]) {

								terrainPlane.normal.cross(dir[refIndex], tempVect);
								tempVect.cross(terrainPlane.normal, planeParallelVect);

								updateDir = true;
							}
						}

						if (updateDir) {

							//maybe theres a better way when pooint is not projectable use plane somehow
							lerp(planeParallelVect, dir[refIndex], 0.005f, dir[pointIndex]);

							if (intersect(terrainPlane, origin, dir[pointIndex], u[pointIndex], &mFrustTerrainIntersectionPoints[pointIndex])) {

								//dont set here becuase this will make this point used as ref dir for interpolation
								//do this in a loop later
								//mFrustTerrainIntersectionPointValid[pointIndex] = (u >= 0.0f);
							}
						}
					}
				}

				for (pointIndex = 0; pointIndex < 4; ++pointIndex) {

					mFrustTerrainIntersectionPointValid[pointIndex] = (u[pointIndex] >= 0.0f);
				}

			}

			mIsValidFrustTerrainIntersectionPoints = true;
		}

		
	}
}

Frustum* SceneCamera::getFrustum() {

	updateFrustum();

	return mFrustumIsDirty ? NULL : &mFrustum;
}

const Vector3* SceneCamera::getFrustTerrainLookAtPoint() {

	updateFrustum();

	return !mFrustumIsDirty && mIsValidFrustTerrainLookAtPoint ? &mFrustTerrainLookAtPoint : NULL;
}

const Vector3* SceneCamera::getFrustTerrainIntersectionPoints(bool*& pointValidity) {

	updateFrustum();

	pointValidity = mFrustTerrainIntersectionPointValid;

	return !mFrustumIsDirty && mIsValidFrustTerrainIntersectionPoints ? mFrustTerrainIntersectionPoints : NULL;
}

void SceneCamera::getMainDirection2D(Match& match, SceneDirectionEnum* pOutDir, bool* pOutNeg, Vector3* pOutDirAxis) {

	enum SelectEnum {

		Select_None = -1, Select_FwdFromFwd, Select_FwdFromUp, Select_RightFromFwd, Select_RightFromUp
	};

	float fwdDot[4];

	fwdDot[Select_FwdFromFwd] = mTransformWorld.row[Scene_Forward].dot(match.getCoordAxis(Scene_Forward));
	fwdDot[Select_FwdFromUp] = mTransformWorld.row[Scene_Up].dot(match.getCoordAxis(Scene_Forward));
	fwdDot[Select_RightFromFwd] = mTransformWorld.row[Scene_Forward].dot(match.getCoordAxis(Scene_Right));
	fwdDot[Select_RightFromUp] = mTransformWorld.row[Scene_Up].dot(match.getCoordAxis(Scene_Right));

	int maxi = 0;

	for (int i = 1; i < 4; ++i) {

		if (fabs(fwdDot[i]) > fabs(fwdDot[maxi])) {

			maxi = i;
		}
	}

	if (pOutNeg)
		*pOutNeg = fwdDot[maxi] < 0.0f;

	switch (maxi) {

		case Select_FwdFromFwd:
		case Select_FwdFromUp: {

			if (pOutDir)
				*pOutDir = Scene_Forward;

			if (pOutDirAxis) {

				pOutDirAxis->zero();
				pOutDirAxis->el[Scene_Forward] = fwdDot[maxi] >= 0.0f ? 1.0f : -1.0f;
			}

		} break;

		case Select_RightFromFwd:
		case Select_RightFromUp: {
		
			if (pOutDir)
				*pOutDir = Scene_Right;

			if (pOutDirAxis) {

				pOutDirAxis->zero();
				pOutDirAxis->el[Scene_Right] = fwdDot[maxi] >= 0.0f ? 1.0f : -1.0f;
			}

		} break;
	}
}

void SceneCamera::updateAsPitchCamera(Match& match, SceneCamera& refCam) {

	enum SelectEnum {

		Select_None = -1, Select_FwdFromFwd, Select_FwdFromUp, Select_RightFromFwd, Select_RightFromUp
	};

	float fwdDot[4];

	fwdDot[Select_FwdFromFwd] = refCam.mTransformWorld.row[Scene_Forward].dot(match.getCoordAxis(Scene_Forward));
	fwdDot[Select_FwdFromUp] = refCam.mTransformWorld.row[Scene_Up].dot(match.getCoordAxis(Scene_Forward));
	fwdDot[Select_RightFromFwd] = refCam.mTransformWorld.row[Scene_Forward].dot(match.getCoordAxis(Scene_Right));
	fwdDot[Select_RightFromUp] = refCam.mTransformWorld.row[Scene_Up].dot(match.getCoordAxis(Scene_Right));

	int maxi = 0;

	for (int i = 1; i < 4; ++i) {

		if (fabs(fwdDot[i]) > fabs(fwdDot[maxi])) {

			maxi = i;
		}
	}

	mTransformWorld.row[Scene_Forward].zero();
	mTransformWorld.row[Scene_Forward].el[Scene_Up] = -1.0f;

	switch (maxi) {

		case Select_FwdFromFwd: {

			mTransformWorld.row[Scene_Up].zero();
			mTransformWorld.row[Scene_Up].el[Scene_Forward] = fwdDot[maxi] >= 0.0f ? 1.0f : -1.0f;

		} break;

		case Select_FwdFromUp: {

			mTransformWorld.row[Scene_Up].zero();
			mTransformWorld.row[Scene_Up].el[Scene_Forward] = fwdDot[maxi] >= 0.0f ? 1.0f : -1.0f;

		} break;

		case Select_RightFromFwd: {

			mTransformWorld.row[Scene_Up].zero();
			mTransformWorld.row[Scene_Up].el[Scene_Right] = fwdDot[maxi] >= 0.0f ? 1.0f : -1.0f;

		} break;

		case Select_RightFromUp: {

			mTransformWorld.row[Scene_Up].zero();
			mTransformWorld.row[Scene_Up].el[Scene_Right] = fwdDot[maxi] >= 0.0f ? 1.0f : -1.0f;

		} break;
	}

	mTransformWorld.row[Scene_Forward].cross(mTransformWorld.row[Scene_Right], mTransformWorld.row[Scene_Up]);

	mTransformWorld.row[Scene_Up].normalize();
	
	mTransformWorld.row[3].el[Scene_Forward] = 0.0f;
	mTransformWorld.row[3].el[Scene_Right] = 0.0f;
	mTransformWorld.row[3].el[Scene_Up] = match.getCoordSys().convUnit(100.0f);

	mTransformLocal = mTransformWorld;
	inverse(mTransformWorld, mMatView);

	const PitchInfo& pitchInfo = match.getPitchInfo();

	mMatProj.setupOrtho(pitchInfo.width, pitchInfo.height, mSettings.rangeMin, mSettings.rangeMax);
}

bool SceneCamera::projectOnScreen(const Vector3& point, float& x, float& y) {

	Vector3 camRelPt;
	Vector3 projSpacePt;
	float w = 1.0f;

	mMatView.transformPoint(point, camRelPt);
	mMatProj.transformPoint(camRelPt, projSpacePt, w);

	//when w is negative we have to reverse it otherwize 
	//the x and y values have the wrong sign
	//this is because the point is behind the camera plane
	float divW = fabs(w);

	x = projSpacePt.x / (divW);
	y = projSpacePt.y / (divW);

	bool isOnScreen = (w >= 0) && ((fabs(x) <= 1.0f && fabs(y) <= 1.0f));

	return isOnScreen;
}

bool SceneCamera::estimatePitchCameraWidth(float& width) {

	const PitchInfo& pitchInfo = mNodeMatch->getPitchInfo();

	float u;
	Vector3 intersection;

	if (intersect(pitchInfo.pitchPlane, mTransformWorld.getPosition(), mTransformWorld.getDirection(Scene_Forward), u, &intersection)
		&& (u >= 0.0f)) {

		float pitchDist = distance(mTransformWorld.getPosition(), intersection);
		width = 2.0f * tan(mSettings.FOVw * 0.5f) * pitchDist;

		return true;
	}

	return false;
}

bool SceneCamera::projectOrthoOnScreen(const Vector3& point, float& x, float& y, float width) {

	Vector3 camRelPt;
	Vector3 projSpacePt;
	float w = 1.0f;

	Matrix4x4 orthoProj;
	orthoProj.setupOrtho(width, width * mState.resHeight / mState.resWidth, mSettings.rangeMin, mSettings.rangeMax);

	mMatView.transformPoint(point, camRelPt);
	orthoProj.transformPoint(camRelPt, projSpacePt, w);

	//when w is negative we have to reverse it otherwize 
	//the x and y values have the wrong sign
	//this is becuase the point is behind the camera plane
	float divW = fabs(w);

	x = projSpacePt.x / (divW);
	y = projSpacePt.y / (divW);

	bool isOnScreen = (w >= 0) && ((fabs(x) <= 1.0f && fabs(y) <= 1.0f));

	return isOnScreen;
}

void SceneCamera::getDirectionTransform(bool constrainUp, bool discreeteDirections, SceneTransform& outTransform) {

	Vector3 lookDir;

	SceneTransform mx;
	RendererD3D9_convert(*mCamera.GetWorldMatrix(), mx);

	{
		lookDir = mx.getDirection(Scene_Forward);
		
		if (constrainUp) {

			lookDir.el[Scene_Up] = 0.0f;
		} 
	}

	if (discreeteDirections) {

		if (fabs(lookDir.el[Scene_Forward]) >= fabs(lookDir.el[Scene_Right])) {

			if (lookDir.el[Scene_Forward] == 0.0f) {

				//the lookDir is in the Scene_Up component only
				//use the Camera's Up vector as the lookDir vector
				//there still something wrong here with the cam's Up Vector...

				lookDir = mx.getDirection(Scene_Up);
			}

			lookDir.el[Scene_Right] = 0.0f;

		} else {

			lookDir.el[Scene_Forward] = 0.0f;
		}
	}

	lookDir.normalize();

	outTransform.setOrientation(lookDir, mNodeMatch->getCoordAxis()[Scene_Up]);
	outTransform.position().zero();
}

void SceneCamera::relativiseDirection(const Vector3& dir, Vector3& relDir, bool constrainUp, bool discreeteDirections, bool inverse) {

	Vector3 lookDir;

	SceneTransform mx;
	RendererD3D9_convert(*mCamera.GetWorldMatrix(), mx);

	{
		lookDir = mx.getDirection(Scene_Forward);
		
		if (constrainUp) {

			lookDir.el[Scene_Up] = 0.0f;
		} 
	}

	if (discreeteDirections) {

		if (fabs(lookDir.el[Scene_Forward]) >= fabs(lookDir.el[Scene_Right])) {

			if (lookDir.el[Scene_Forward] == 0.0f) {

				//the lookDir is in the Scene_Up component only
				//use the Camera's Up vector as the lookDir vector
				//there still something wrong here with the cam's Up Vector...

				lookDir = mx.getDirection(Scene_Up);
			}

			lookDir.el[Scene_Right] = 0.0f;

		} else {

			lookDir.el[Scene_Forward] = 0.0f;
		}
	}

	lookDir.normalize();


	SceneTransform trans;

	trans.setOrientation(lookDir, mNodeMatch->getCoordAxis()[Scene_Up]);

	if (inverse)
		trans.invTransformVector(dir, relDir);
	else
		trans.transformVector(dir, relDir);
}

/*
http://www.panotools.info/mediawiki/index.php?title=Field_of_View

For rectilinear images:

 Aspect Ratio = tan (HFoV / 2)  / tan (VFoV / 2)

For fisheye images (approximation):

 Aspect Ratio = HFoV / VFoV

 Field of View = 2 x atan ( 35 / ( 2 x Focal Length ) )

*/

void SceneCamera::resetProjParams() {

	Renderer& renderer = mNodeMatch->getRenderer();
	RenderViewportInfo viewInfo;

	mFrustumIsDirty = true;

	if (renderer.getViewportInfo(viewInfo)) {

		mCamera.SetProjParams(mSettings.FOVh, viewInfo.getAspectRatio(), mSettings.rangeMin, mSettings.rangeMax);

	} else {

		assrt(false);
	}
}

void SceneCamera::getCamViewParams(Vector3* pEye, Vector3* pLookAt) {

	CBaseCamera* pBaseCam = &mCamera;

	if (pEye) {

		const D3DXVECTOR3& currEye_vector = *(pBaseCam->GetEyePt());
		pEye->set(currEye_vector.x, currEye_vector.y, currEye_vector.z);
	}

	if (pLookAt) {

		const D3DXVECTOR3& currLookAt_vector = *(pBaseCam->GetLookAtPt());
		pLookAt->set(currLookAt_vector.x, currLookAt_vector.y, currLookAt_vector.z);
	}
}

void SceneCamera::setCamViewParams(const Vector3& eyePos, const Vector3& lookAt) {

	D3DXVECTOR3 lookat_vector = D3DXVECTOR3(MExpand3(lookAt));
	D3DXVECTOR3 eye_vector = D3DXVECTOR3(MExpand3(eyePos));
			
	mCamera.SetViewParams(&eye_vector, &lookat_vector);
	mCamera.clean();

	mFrustumIsDirty = true;
	nodeMarkDirty();
}


/*
void SceneCamera::moveCamera(const Point& lookAtPos) {

	Point targetLookAtPos;
	Point targetEyePos;

	lookAtPos.add(mLookAtOffset, targetLookAtPos);
	lookAtPos.add(mTrackOffset, targetEyePos);

	setCamViewParams(targetEyePos, targetLookAtPos);

	//mState.camera.eye.setPos(targetEyePos, mState.updateDt, mState.resetFlag);
	//mState.camera.target.setPos(targetLookAtPos, mState.updateDt, mState.resetFlag);

	nodeMarkDirty();
}
*/


void SceneCamera::moveCamera(const Point& lookAtPos) {

	Point targetLookAtPos;
	Point targetEyePos;

	lookAtPos.add(mLookAtOffset, targetLookAtPos);
	lookAtPos.add(mTrackOffset, targetEyePos);

	setCamViewParams(targetEyePos, targetLookAtPos);
}

const Vector3& SceneCamera::getConstrainedTrackPoint() {

	return mState.track.pos.currPos;
}

void SceneCamera::syncToTrackedNode() {

	SoftPtr<SceneNode> trackedNode = getTrackedNode();
	Point trackedPos;

	if (trackedNode.isValid()) {

		if (trackedNode->objectType == Node_Ball) {

			Ball& ball = dref(Ball::ballFromNode(trackedNode));

			if (ball.isAttachedTo(this))
				return;
		}

		//DIRTY HACK, this should not be done here

		mState.t = mNodeMatch->getClock().getTime();

		mState.track.constrainPos(trackedNode->mTransformLocal.getPosition(), trackedPos);
		mState.track.pos.setPos(trackedPos, mState.updateDt, mState.resetFlag);
		
		Vector3 eyeModif(0.0f);
		Vector3 lookAtModif(0.0f);
		Vector3 lookAtOffsetModif(0.0f);
		Vector3 postModif(0.0f);

		Point lookAtBasePos;

		if (mState.resetFlag) {

			if (mBoundController.isValid()) {

				mBoundController->forceStarted(mState);
			}

			if (mBoundPostController.isValid()) {

				mBoundPostController->forceStarted(mState);
			}

			/*
			if (mCamPointCollider.isValid()) {
				
				mCamPointCollider->correction.zero();

				Sphere sphere;

				sphere.center = mTransformWorld.getPosition();
				sphere.radius = mCamPointCollider->radius;

				mCamPointCollider->collider.mVolumeWorld.init(&sphere);
				mCamPointCollider->collider.addToDetector(*this, mNodeMatch->getCollEngine());
			}
			*/

		} else {

			if (mBoundController.isValid()) {

				mBoundController->update(mState);
				mBoundController->getModifs(eyeModif, lookAtModif, lookAtOffsetModif);
			}
		}

		trackedPos.add(lookAtModif, lookAtBasePos);

		mState.track.lookAtBasePos.setPos(lookAtBasePos, mState.updateDt, mState.resetFlag);

		Point targetLookAtPos;
		Point targetEyePos;

		lookAtBasePos.add(mLookAtOffset, targetLookAtPos);
		lookAtBasePos.add(mTrackOffset, targetEyePos);

		targetEyePos.add(eyeModif);
		targetLookAtPos.add(lookAtOffsetModif);

		mState.camera.eye.setPos(targetEyePos, mState.updateDt, mState.resetFlag);
		mState.camera.target.setPos(targetLookAtPos, mState.updateDt, mState.resetFlag);

		if (!mState.resetFlag) {

			if (mBoundPostController.isValid()) {

				mBoundPostController->update(mState);
				mBoundPostController->getPostModifs(postModif);
			}
		}

		targetEyePos.add(postModif);
		targetLookAtPos.add(postModif);

		mState.cameraPost.eye.setPos(targetEyePos, mState.updateDt, mState.resetFlag);
		mState.cameraPost.target.setPos(targetLookAtPos, mState.updateDt, mState.resetFlag);

		/*
		if (mCamPointCollider.isValid()) {

			Sphere& sphere = mCamPointCollider->collider.mVolumeWorld.toSphere();
			sphere.center = targetLookAtPos;
			sphere.radius = mCamPointCollider->radius;
			
			mCamPointCollider->collider.updateWorldVolumeInDetector(*this, mNodeMatch->getCollEngine());

			{
				Sphere& sphere = mCamPointCollider->collider.mVolumeWorld.toSphere();
				OBB obb;

				transform(sphere, obb, true);
				CollRegistry::Records& recs = mCamPointCollider->collRegistry.mRecords;

				CollEngine::helperConstrainByOBBContacts(obb, recs, NULL, &mCamPointCollider->correction);

				targetLookAtPos.add(mCamPointCollider->correction);
				mCamPointCollider->correctedLookAtPos = targetLookAtPos;
			}
		}
		*/

		setCamViewParams(targetEyePos, targetLookAtPos);

		mState.setResetFlag(false);
	} 
}

/*
void SceneCamera::collCamPointBroadPhaseOnStart(Object* pObject, CollRecordPool& collPool) {

	bool takeObject = (pObject->objectType == Node_EntMesh || pObject->objectType == Node_PitchLimit);

	if (takeObject) {

		bool alreadyFound;

   		mCamPointCollider->collRegistry.add(pObject, collPool, alreadyFound);
	}
}

void SceneCamera::collCamPointBroadPhaseOnEnd(Object* pObject, CollRecordPool& collPool) {

	bool takeObject = (pObject->objectType == Node_EntMesh || pObject->objectType == Node_PitchLimit);

	if (takeObject) {
		
		mCamPointCollider->collRegistry.remove(pObject, collPool);
	}
}
*/

void SceneCamera::syncToTracked() {

	mState.resetFlag = true;
}

void SceneCamera::clean() {

	RenderViewportInfo viewport;

	if (mNodeMatch->getRenderer().getViewportInfo(viewport)) {

		if ((viewport.width != mState.resWidth) || (viewport.height != mState.resHeight)) {

			mState.resWidth = viewport.width;
			mState.resHeight = viewport.height;

			mSettings.updateFOVs(viewport);
			resetProjParams();

			mState.resolutionChangedFlag = true;
		}

	} else {

		assrt(false);
	}

	syncToTrackedNode();

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

	RendererD3D9_convert(*mCamera.GetProjMatrix(), mMatProj);
	RendererD3D9_convert(*mCamera.GetViewMatrix(), mMatView);

	mState.resolutionChangedFlag = false;
}

bool SceneCamera::processWndMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {

	bool process = false;

	switch (uMsg) {

		case WM_KEYDOWN:
		case WM_KEYUP:
			{
				switch (wParam) {
					case VK_NUMPAD4:
					case VK_NUMPAD6:	
					case VK_NUMPAD8:
						process = true;
						wParam = wParam;
					break;
					case VK_NUMPAD5:	
						process = true;
						wParam = VK_NUMPAD2;
						break;
				}
			}
		break;

		
        case WM_RBUTTONDBLCLK:
        case WM_MBUTTONDBLCLK:
        case WM_LBUTTONDBLCLK:
		case WM_RBUTTONUP: 
        case WM_LBUTTONUP:   
			wParam = wParam;
			process = true;
			break;

		case WM_RBUTTONDOWN:
		case WM_LBUTTONDOWN: {

			if (mNodeMatch->mTweakEnableManipulateCam && getTrackedNode()) {

				mManipulLastPosX = GET_X_LPARAM(lParam); 
				mManipulLastPosY = GET_Y_LPARAM(lParam); 
				mManipulTrackOffset = mTrackOffset;
			} 
			
			wParam = wParam;
			process = true;
		} break;

		case WM_MOUSEMOVE: {

			if (getTrackedNode()) {

				if (mNodeMatch->mTweakEnableManipulateCam && ((wParam & MK_LBUTTON) || (wParam & MK_RBUTTON))) {

					int xDiff = GET_X_LPARAM(lParam) - mManipulLastPosX; 
					int yDiff = GET_Y_LPARAM(lParam) - mManipulLastPosY; 

					if (wParam & MK_RBUTTON) {

						if (fabs((float) xDiff) > fabs((float) yDiff)) {

							yDiff = 0;

						} else {

							xDiff = 0;
						}
					}

					Vector3 yRotAxis;

					mNodeMatch->getCoordAxis(Scene_Up).cross(mManipulTrackOffset, yRotAxis);

					if (!yRotAxis.isZero()) {

						yRotAxis.normalize();

						RigidMatrix4x3 rotMatrix[2];
						RigidMatrix4x3 transfMatrix;

						rotMatrix[0].setupRotation(mNodeMatch->getCoordAxis(Scene_Up), (degToRad((float) xDiff * 0.1f)));
						rotMatrix[1].setupRotation(yRotAxis, (degToRad((float) yDiff * 0.1f)));

						mul(rotMatrix[0], rotMatrix[1], transfMatrix);

						transfMatrix.transformVector(mManipulTrackOffset, mTrackOffset);
					}
				}

			} else {

				wParam = wParam;
				process = true;
			}

		} break;

		case WM_MBUTTONDOWN: 
			break;

		case WM_MBUTTONUP: {

			if (mNodeMatch->mTweakEnableManipulateCam && getTrackedNode()) {

				mTrackOffset = mOrigTrackOffset;
			}

		} break;

		case WM_MOUSEWHEEL: {

			if (mNodeMatch->mTweakEnableManipulateCam && getTrackedNode()) {

				short zDelta = (short) (HIWORD(wParam)); // wheel rotation 
							
				float offset = mSettings.velocity * mNodeMatch->getClock().getFrameMoveTickLength();
				
				if (zDelta != 0) {

					float currMag = mTrackOffset.mag();

					if (zDelta < 0) {

						if ((currMag - offset) > mSettings.rangeMin)
							mTrackOffset.mul((currMag - offset) /currMag);

					} else {

						if ((currMag + offset) < mSettings.rangeMax)
							mTrackOffset.mul((currMag + offset) / currMag);
					}
				}
			}

		} break;
	}
	
	if (process) {

		if (!mNodeMatch->mTweakEnableDebugKeys && (getTrackedNode() != NULL))
			return false;

		mCamera.HandleMessages(0, uMsg, wParam, lParam);
		return true;
	}
	
	return false;

}

void SceneCamera::frameMove(const Time& t, const Time& dt) {

	SoftPtr<SceneNode> trackedNode = getTrackedNode();

	if (trackedNode.isNull()) {

		//matchUIEvtQueueIterate(mNodeMatch->getUIEvtQueue(), *this, t);

		mCamera.FrameMove(dt);

	} else {

		mState.updateDt = dt;

		//moved this to clean() as a hack against jittering for now
		//because of order of frameMove calls.
		//syncToTrackedNode();
	}

	signalCameraChanged();
}

void SceneCamera::signalCameraChanged() {
	
	nodeMarkDirty();

	/*
	Vector3 eye(MDXExpandVect(*mCamera.GetEyePt()));
	Vector3 lookAt(MDXExpandVect(*mCamera.GetLookAtPt()));

	mTransformLocal.setPosition(eye);
	mTransformLocal.setOrientation(eye, lookAt, mNodeMatch->getCoordAxis(Scene_Up));
	*/

	RendererD3D9_convert(*mCamera.GetWorldMatrix(), mTransformLocal);
}

void SceneCamera::cancelTrack() {

	mState.setResetFlag(true);
	mTrackedPlayer.destroy();
	mTrackedNode.destroy();
}

void SceneCamera::track(Player* pPlayer, float* pConstrainUpValue) {

	mState.setResetFlag(true);
	mTrackedPlayer = pPlayer;
	
	if (pConstrainUpValue) {

		mState.track.settings.constrainUp = true;
		mState.track.settings.constrainUpValue = *pConstrainUpValue;

	} else {

		mState.track.settings.constrainUp = false;
	}

	/*
	if (mTrackedPlayer.isValid()) {

		mTrackedNode.destroy();
	}
	*/
}

void SceneCamera::track(SceneNode* pNode, float* pConstrainUpValue) {

	mState.setResetFlag(true);
	mTrackedNode = pNode;

	if (pConstrainUpValue) {

		mState.track.settings.constrainUp = true;
		mState.track.settings.constrainUpValue = *pConstrainUpValue;

	} else {

		mState.track.settings.constrainUp = false;
	}

	/*
	if (getTrackedNode == NULL && mCamPointCollider.isValid()) {

		mCamPointCollider->collider.removeFromDetector(*this, mNodeMatch->getCollEngine(), false);
	}
	*/

	if (mTrackedNode.isValid()) {

		mTrackedPlayer.destroy();
	}
}

SceneNode* SceneCamera::getTrackedNode() {

	return mTrackedPlayer.isValid() ? mTrackedPlayer->getAttachedFootballer() : mTrackedNode.ptr();
}

void SceneCamera::setMatrices(Renderer& renderer, bool cleanIfNeeded, RenderPassEnum pass) {

	if (pass != RP_Normal) {

		return;
	}

	if (cleanIfNeeded) {
		
		clean();
	}

	resetProjParams();

#ifdef _DEBUG
	assrt(mIsInitedSettings == true);
#endif

	//RendererD3D9_convert(*mCamera.GetProjMatrix(), mMatProj);
	renderer.setTransform(mMatProj, RT_Projection);

	//RendererD3D9_convert(*mCamera.GetViewMatrix(), mMatView);
	renderer.setTransform(mMatView, RT_View);
}

void SceneCamera::render(Renderer& renderer, const bool& cleanIfNeeded, const bool& flagExtraRenders) {

	SoftPtr<SceneNode> trackedNode = getTrackedNode();

	if (false) {

		mFrustum.updatePlanes();

		for (int i = 0; i < Frustum::PlaneCount; ++i) {

			debugPlanes[i] = mFrustum.planes[i];
		}
	}

	if (flagExtraRenders) {

		/*
		renderer.draw(debugPlanes[Frustum::Pl_Left], 500.0f, &Matrix4x3Base::kIdentity, &RenderColor::kBlue);
		renderer.draw(debugPlanes[Frustum::Pl_Right], 500.0f, &Matrix4x3Base::kIdentity, &RenderColor::kBlue);
		renderer.draw(debugPlanes[Frustum::Pl_Top], 500.0f, &Matrix4x3Base::kIdentity, &RenderColor::kRed);
		renderer.draw(debugPlanes[Frustum::Pl_Bottom], 500.0f, &Matrix4x3Base::kIdentity, &RenderColor::kRed);

		renderer.draw(debugPlanes[Frustum::Pl_Near], 250.0f, &Matrix4x3Base::kIdentity, &RenderColor::kYellow);
		renderer.draw(debugPlanes[Frustum::Pl_Far], 1000.0f, &Matrix4x3Base::kIdentity, &RenderColor::kBlack);
		*/
	
		if (trackedNode.isValid()) {

			Sphere sphere;

			sphere.center = mState.track.pos.currPos;
			sphere.radius = 25.0f;
			renderer.draw(sphere, &Matrix::kIdentity, &RenderColor::kGreen);

			sphere.radius = 25.0f;
			sphere.center = mState.track.lookAtBasePos.currPos;
			renderer.draw(sphere, &Matrix::kIdentity, &RenderColor::kBlue);
			
			if (mBoundBufferZone.isValid()) {

				sphere.radius = mBoundBufferZone->getRadius();
				renderer.draw(sphere, &Matrix::kIdentity, &RenderColor::kYellow);
			}

			sphere.radius = 30.0f;
			sphere.center = mState.camera.target.currPos;
			renderer.draw(sphere, &Matrix::kIdentity, &RenderColor::kRed);

			sphere.radius = 40.0f;
			sphere.center = mState.cameraPost.target.currPos;
			renderer.draw(sphere, &Matrix::kIdentity, &RenderColor::kRed);

			/*
			if (mCamPointCollider.isValid()) {

				{
					Sphere& sph = mCamPointCollider->collider.mVolumeWorld.toSphere();
					OBB obb;

					transform(sph, obb, true);

					renderer.draw(obb, &Matrix::kIdentity, &RenderColor::kMagenta);

					sphere.radius = 20.0f;
					sphere.center = mCamPointCollider->correctedLookAtPos;
					renderer.draw(sphere, &Matrix::kIdentity, &RenderColor::kBluish1);
				}

				mCamPointCollider->collider.render(*this, mNodeMatch->getCollEngine(), renderer, 
									mNodeMatch->getFlagRenderBPVolumes(), mNodeMatch->getFlagRenderVolumes(), 
									&RenderColor::kGreen, &RenderColor::kWhite);
			} 
			*/
		}
		
	}
}

} }