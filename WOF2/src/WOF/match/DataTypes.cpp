#include "DataTypes.h"

#include "entity/SceneEntityMesh.h"
#include "entity/footballer/Footballer.h"
#include "entity/ball/Ball.h"
#include "Match.h"
#include "spatialQuery/PitchRegionShape.h"

namespace WOF { namespace match {

bool objectHasProxy(Object* pObject) {

	if (typeIsEntMesh(pObject->objectType)) {

		return SceneEntityMesh::entMeshFromObject(pObject)->hasProxyVol();
	}

	return false;
}

SceneDynVol* objectGetProxy(Object* pObject) {

	if (typeIsEntMesh(pObject->objectType)) {

		return SceneEntityMesh::entMeshFromObject(pObject)->getProxyVol();
	}

	return NULL;
}

ScriptObject ScriptGameObject::script_toFootballer() {

	if (isFootballer()) {

		Footballer& footballer = dref(Footballer::footballerFromObject(mObject));

		return footballer.getScriptObject();
	}

	return ScriptObject();
}

PitchInfo::PitchInfo() 
	: isValid(false), 

		height(-1.0f), 
		
		width(-1.0f), length(-1.0f), 
		halfWidth(-1.0f), halfLength(-1.0f),

		penaltyBoxWidth(-1.0f), penaltyBoxLength(-1.0f), 
		penaltyBoxHalfWidth(-1.0f), penaltyBoxHalfLength(-1.0f), 
		penaltyBoxFwdMax(-1.0f),

		goalBoxWidth(-1.0f), goalBoxLength(-1.0f), 
		goalBoxHalfWidth(-1.0f), goalBoxHalfLength(-1.0f), 

		goalBoxFwdMax(-1.0f),

		goalWidth(-1.0f), goalHalfWidth(-1.0f) {
}

void PitchInfo::update(Match& match) {

	this->halfWidth = 0.5f * this->width;
	this->halfLength = 0.5f * this->length;

	this->penaltyBoxHalfWidth = 0.5f * this->penaltyBoxWidth;
	this->penaltyBoxHalfLength = 0.5f * this->penaltyBoxLength;

	this->penaltyBoxFwdMax = -this->halfLength + this->penaltyBoxLength;

	this->goalBoxHalfWidth = 0.5f * this->goalBoxWidth;
	this->goalBoxHalfLength = 0.5f * this->goalBoxLength;

	this->goalBoxFwdMax = -this->halfLength + this->goalBoxLength;
	this->goalHalfWidth = 0.5f * this->goalWidth;

	{
		Vector3 pos;

		pos.zero();
		pos.el[Scene_Up] = height;

		pitchPlane.init(pos, match.getCoordAxis(Scene_Up), true);
	}

	goalCenterBack.zero();
	goalCenterBack.el[Scene_Up] = height;
	goalCenterBack.el[Scene_Forward] = -halfLength;
	goalCenterBack.el[Scene_Right] = 0.0f;

	goalSideBarBackLeft.zero();
	goalSideBarBackLeft.el[Scene_Up] = height;
	goalSideBarBackLeft.el[Scene_Forward] = -halfLength;
	goalSideBarBackLeft.el[Scene_Right] = -goalHalfWidth;

	goalSideBarBackRight.zero();
	goalSideBarBackRight.el[Scene_Up] = height;
	goalSideBarBackRight.el[Scene_Forward] = -halfLength;
	goalSideBarBackRight.el[Scene_Right] = goalHalfWidth;

	goalCenterFwd.zero();
	goalCenterFwd.el[Scene_Up] = height;
	goalCenterFwd.el[Scene_Forward] = halfLength;
	goalCenterFwd.el[Scene_Right] = 0.0f;

	goalSideBarFwdLeft.zero();
	goalSideBarFwdLeft.el[Scene_Up] = height;
	goalSideBarFwdLeft.el[Scene_Forward] = halfLength;
	goalSideBarFwdLeft.el[Scene_Right] = -goalHalfWidth;

	goalSideBarFwdRight.zero();
	goalSideBarFwdRight.el[Scene_Up] = height;
	goalSideBarFwdRight.el[Scene_Forward] = halfLength;
	goalSideBarFwdRight.el[Scene_Right] = goalHalfWidth;

	match.toScene2D(goalCenterBack, goalCenterBack2D);
	match.toScene2D(goalSideBarBackLeft, goalSideBarBackLeft2D);
	match.toScene2D(goalSideBarBackRight, goalSideBarBackRight2D);
	match.toScene2D(goalCenterFwd, goalCenterFwd2D);
	match.toScene2D(goalSideBarFwdLeft, goalSideBarFwdLeft2D);
	match.toScene2D(goalSideBarFwdRight, goalSideBarFwdRight2D);

	{
		regionPitch.destroy();

		SoftPtr<PitchRegionShape_AAQuad> region;

		WE_MMemNew(region.ptrRef(), PitchRegionShape_AAQuad());

		region->shape().min.el[Scene2D_Right] = -this->halfWidth;
		region->shape().max.el[Scene2D_Right] = this->halfWidth;

		region->shape().min.el[Scene2D_Forward] = -this->halfLength;
		region->shape().max.el[Scene2D_Forward] = this->halfLength;

		regionPitch.ptrRef() = region.ptr();
	}


	{
		regionPenaltyBack.destroy();

		SoftPtr<PitchRegionShape_AAQuad> region;

		WE_MMemNew(region.ptrRef(), PitchRegionShape_AAQuad());

		region->shape().min.el[Scene2D_Right] = -this->penaltyBoxHalfWidth;
		region->shape().max.el[Scene2D_Right] = this->penaltyBoxHalfWidth;

		region->shape().min.el[Scene2D_Forward] = -this->halfLength;
		region->shape().max.el[Scene2D_Forward] = -this->halfLength + penaltyBoxLength;

		regionPenaltyBack.ptrRef() = region.ptr();
	}

	{
		regionGoalBoxBack.destroy();

		SoftPtr<PitchRegionShape_AAQuad> region;

		WE_MMemNew(region.ptrRef(), PitchRegionShape_AAQuad());

		region->shape().min.el[Scene2D_Right] = -this->goalBoxHalfWidth;
		region->shape().max.el[Scene2D_Right] = this->goalBoxHalfWidth;

		region->shape().min.el[Scene2D_Forward] = -this->halfLength;
		region->shape().max.el[Scene2D_Forward] = -this->halfLength + goalBoxLength;

		regionGoalBoxBack.ptrRef() = region.ptr();
	}

	{
		regionPenaltyFwd.destroy();

		SoftPtr<PitchRegionShape_AAQuad> region;

		WE_MMemNew(region.ptrRef(), PitchRegionShape_AAQuad());

		region->shape().min.el[Scene2D_Right] = -this->penaltyBoxHalfWidth;
		region->shape().max.el[Scene2D_Right] = this->penaltyBoxHalfWidth;

		region->shape().min.el[Scene2D_Forward] = this->halfLength - penaltyBoxLength;
		region->shape().max.el[Scene2D_Forward] = this->halfLength;

		regionPenaltyFwd.ptrRef() = region.ptr();
	}

	{
		regionGoalBoxFwd.destroy();

		SoftPtr<PitchRegionShape_AAQuad> region;

		WE_MMemNew(region.ptrRef(), PitchRegionShape_AAQuad());

		region->shape().min.el[Scene2D_Right] = -this->goalBoxHalfWidth;
		region->shape().max.el[Scene2D_Right] = this->goalBoxHalfWidth;

		region->shape().min.el[Scene2D_Forward] = this->halfLength - goalBoxLength;
		region->shape().max.el[Scene2D_Forward] = this->halfLength;

		regionGoalBoxFwd.ptrRef() = region.ptr();
	}
}

bool PitchInfo::contains(CtVector2& from, Vector2& exitPoint, CtVector2* pTo, CoordSysDirection* pExitDir1, CoordSysDirection* pExitDir2) {

	bool wasClipped = false;

	if (!regionPitch->contains(from)) {

		regionPitch->clip(from, exitPoint);
		wasClipped = true;

	} else {

		if (pTo) {

			wasClipped = (regionPitch->clipByDest(from, dref(pTo), exitPoint) == true);
		}
	}

	if (wasClipped) {

		if (pExitDir1 && pExitDir2) {

			float diff;


			if (fabs(exitPoint.el[Scene2D_Right]) < halfWidth)
				diff = 0.0f;
			else
				diff = from.el[Scene2D_Right] - exitPoint.el[Scene2D_Right];
			
			if (diff > 0.0f) {

				*pExitDir1 = CSD_Right;

				
			} else if (diff < 0.0f) {

				*pExitDir1 = CSD_Left;
				
			} else {

				*pExitDir1 = CSD_Unused;
			}



			if (fabs(exitPoint.el[Scene2D_Forward]) < halfLength)
				diff = 0.0f;
			else
				diff = from.el[Scene2D_Forward] - exitPoint.el[Scene2D_Forward];

			if (diff > 0.0f) {

				*pExitDir2 = CSD_Forward;

				
			} else if (diff < 0.0f) {

				*pExitDir2 = CSD_Back;
				
			} else {

				*pExitDir2 = CSD_Unused;
			}
		}

		return false;
	}

	return true;
}

} }


DECLARE_INSTANCE_TYPE_NAME(WOF::match::BallOwnership, BallOwnership);
DECLARE_INSTANCE_TYPE_NAME(WOF::match::BallRelativeDist, BallRelativeDist);
DECLARE_INSTANCE_TYPE_NAME(WOF::match::BallInteraction, BallInteraction);
DECLARE_INSTANCE_TYPE_NAME(WOF::match::ScriptGameObject, GameObject);

namespace WOF { namespace match {


void BallRelativeDist::update(Match& match, SceneTransform& footballerTransform, CtVector3& footballerRelCOCOffset, CtVector3& _ballPos, float ballRadius) {

	Vector3 ballPos;

	ballPos.el[Scene_Right] = _ballPos.el[Scene_Right];
	ballPos.el[Scene_Up] = _ballPos.el[Scene_Up] - ballRadius;
	ballPos.el[Scene_Forward] = _ballPos.el[Scene_Forward];

	ballPos.subtract(footballerTransform.getPosition(), footballerOffset);
	
	footballerDist = footballerOffset.mag();
	footballerHorizAngle = angle(footballerTransform.getDirection(Scene_Forward), (CtVector3&) footballerOffset, match.getCoordAxis(Scene_Up));
	footballerHorizAngle = wrapPi(footballerHorizAngle);
	inFrontOfFootballer = fabs(footballerHorizAngle) <= kPiOver2;

	footballerHorizDist = sqrtf(footballerOffset.el[Scene_Forward] * footballerOffset.el[Scene_Forward] + footballerOffset.el[Scene_Right] * footballerOffset.el[Scene_Right]);

	Vector3 COCPos;

	footballerTransform.transformPoint(footballerRelCOCOffset, COCPos);
	ballPos.subtract(COCPos, COCOffset);

	COCDist = COCOffset.mag();
	COCHorizDist = sqrtf(COCOffset.el[Scene_Forward] * COCOffset.el[Scene_Forward] + COCOffset.el[Scene_Right] * COCOffset.el[Scene_Right]);
}

float BallRelativeDist::calcFootballerBallHorizDistSq(CtVector3& footballerPos, CtVector3& _ballPos, float ballRadius) {

	Vector3 ballPos;

	ballPos.el[Scene_Right] = _ballPos.el[Scene_Right];
	ballPos.el[Scene_Up] = _ballPos.el[Scene_Up] - ballRadius;
	ballPos.el[Scene_Forward] = _ballPos.el[Scene_Forward];

	Vector3 footballerOffset;

	ballPos.subtract(footballerPos, footballerOffset);

	return footballerOffset.el[Scene_Forward] * footballerOffset.el[Scene_Forward] + footballerOffset.el[Scene_Right] * footballerOffset.el[Scene_Right];
}

float BallRelativeDist::calcFootballerBallHeight(CtVector3& footballerPos, CtVector3& _ballPos, float ballRadius) {

	return (_ballPos.el[Scene_Up] - footballerPos.el[Scene_Up]) - ballRadius;
}


bool BallRelativeDist::isInCOC(Match& match) {

	return (getCOCHorizDist() <= match.mCOCSetup.radius
					&& getHeight() <= match.mCOCSetup.height 
					&& isInFrontOfFootballer());
}

bool BallRelativeDist::calcIsInCOC(Match& match, SceneTransform& footballerTransform, CtVector3& footballerRelCOCOffset, CtVector3& ballPos, float ballRadius) {

	BallRelativeDist temp;

	temp.update(match, footballerTransform, footballerRelCOCOffset, ballPos, ballRadius);

	return temp.isInCOC(match);
}

bool BallRelativeDist::calcIsInCOC2(Match& match, SceneTransform& footballerTransform, CtVector3& footballerRelCOCOffset, CtVector3& ballPos, float ballRadius, bool& isInFront) {

	BallRelativeDist temp;

	temp.update(match, footballerTransform, footballerRelCOCOffset, ballPos, ballRadius);
	isInFront = temp.isInFrontOfFootballer();

	return temp.isInCOC(match);
}

const TCHAR* BallOwnership::ScriptClassName = L"BallOwnership";
const TCHAR* BallRelativeDist::ScriptClassName = L"BallRelativeDist";
const TCHAR* BallInteraction::ScriptClassName = L"BallInteraction";
const TCHAR* ScriptGameObject::ScriptClassName = L"GameObject";


void BallOwnership::declareInVM(SquirrelVM& target) {

	SqPlus::SQClassDef<BallOwnership>(ScriptClassName).
		var(&BallOwnership::ownershipStartTime, L"ownershipStartTime").
		func(&BallOwnership::getType, L"getType").
		func(&BallOwnership::setType, L"setType");
}


void BallRelativeDist::declareInVM(SquirrelVM& target) {

	SqPlus::SQClassDef<BallRelativeDist>(ScriptClassName).
		func(&BallRelativeDist::update, L"update").
		func(&BallRelativeDist::script_getCOCDist, L"getCOCDist").
		func(&BallRelativeDist::script_getBallHeight, L"getBallHeight").
		func(&BallRelativeDist::script_getFootballerHorizDist, L"getFootballerHorizDist").
		staticFunc(&BallRelativeDist::calcFootballerBallHorizDistSq, L"calcFootballerBallHorizDistSq").
		staticFunc(&BallRelativeDist::calcFootballerBallHeight, L"calcFootballerBallHeight").
		staticFunc(&BallRelativeDist::calcIsInCOC, L"calcIsInCOC");
}

void BallInteraction::declareInVM(SquirrelVM& target) {

	SqPlus::SQClassDef<BallInteraction>(ScriptClassName).
		var(&BallInteraction::ownership, L"ownership").
		var(&BallInteraction::relativeDist, L"relativeDist");
}

void ScriptGameObject::declareInVM(SquirrelVM& target) {

	SqPlus::SQClassDef<ScriptGameObject>(ScriptClassName).
		func(&ScriptGameObject::isNull, L"isNull").
		func(&ScriptGameObject::equals, L"equals").
		func(&ScriptGameObject::isPitch, L"isPitch").
		func(&ScriptGameObject::isFootballer, L"isFootballer").
		func(&ScriptGameObject::script_toFootballer, L"toFootballer");
	
}

void SoftCOCSkin::setIfEmpty(COCSkin* pSkin, bool fallbackActiveCOC) {

	if (pSkin) {

		COC.setIfEmpty(pSkin->COC);
		activeCOC.setIfEmpty(pSkin->activeCOC);

		if (fallbackActiveCOC && pSkin->COC.isValid() && pSkin->COC->getColor()) {

			if (!activeCOC.getColor()) {

				activeCOC.mColor = pSkin->COC->getColor();
			}
		}
	}
}

void SoftPlayerSkin::setIfEmpty(PlayerSkin* pSkin, bool fallbackActiveCOC, bool fallbackScanner, bool fallbackArrow) {

	if (pSkin) {

		scanner.setIfEmpty(pSkin->scanner);
		arrow.setIfEmpty(pSkin->arrow);
		SoftCOCSkin::setIfEmpty(pSkin, fallbackActiveCOC);

		if (!arrow.getColor() && fallbackArrow && scanner.getColor()) {

			arrow.mColor = scanner.getColor();
		}

		if (!scanner.getColor() && fallbackScanner && pSkin->COC.isValid() && pSkin->COC->getColor()) {

			scanner.mColor = pSkin->COC->getColor();
		}

		if (!arrow.getColor() && fallbackArrow && pSkin->COC.isValid() && pSkin->COC->getColor()) {

			arrow.mColor = pSkin->COC->getColor();
		}
	}
}

} }