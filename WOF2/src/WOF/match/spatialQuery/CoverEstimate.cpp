#include "CoverEstimate.h"
#include "../../../WOFApp.h"
#include "../Match.h"

#include "WE3/math/WEDistance.h"
using namespace WE;



DECLARE_INSTANCE_TYPE_NAME(WOF::match::CoverEstimate2D, CCoverEstimate2D);

namespace WOF { namespace match {

const TCHAR* CoverEstimate2D::ScriptClassName = L"CCoverEstimate2D";

void CoverEstimate2D::declareInVM(SquirrelVM& target) {

	SqPlus::SQClassDef<CoverEstimate2D>(ScriptClassName).
		enumInt(CoverEstimate2D::Result_None, L"Result_None").
		enumInt(CoverEstimate2D::Result_Success, L"Success").
		enumInt(CoverEstimate2D::Result_AttackerIsGoingAway, L"AttackerIsGoingAway").
		enumInt(CoverEstimate2D::Result_AttackerIsCloser, L"AttackerIsCloser").
		enumInt(CoverEstimate2D::Result_AngleClose, L"AngleClose").
		var(&CoverEstimate2D::blockOffset, L"blockOffset").
		var(&CoverEstimate2D::toleranceDist, L"toleranceDist").
		var(&CoverEstimate2D::frameInterval, L"frameInterval").
		func(&CoverEstimate2D::holdIdealPos2D, L"holdIdealPos2D").
		func(&CoverEstimate2D::holdIdealPos3D, L"holdIdealPos3D").
		func(&CoverEstimate2D::script_idealPos2D, L"idealPos2D").
		func(&CoverEstimate2D::script_idealPos3D, L"idealPos3D").
		func(&CoverEstimate2D::script_estimate, L"estimate").
		func(&CoverEstimate2D::script_estimateWithRegion, L"estimateWithRegion").
		func(&CoverEstimate2D::script_estimate3DWithRegion, L"estimate3DWithRegion").
		func(&CoverEstimate2D::script_getResult, L"getResult").
		func(&CoverEstimate2D::script_getIdealSpeed, L"getIdealSpeed");
}

CoverEstimate2D::CoverEstimate2D() {

	result = Result_None;
	blockOffset = 0.0f;
	toleranceDist = 0.0f;
	frameInterval = 0.0f;
}

void CoverEstimate2D::holdIdealPos2D(bool hold) {

	if (hold) {

		WE_MMemNew(holderIdealPos2D.ptrRef(), Vector2());

	} else {

		holderIdealPos2D.destroy();
	}
}

void CoverEstimate2D::holdIdealPos3D(bool hold) {

	if (hold) {

		WE_MMemNew(holderIdealPos3D.ptrRef(), Vector3());

	} else {

		holderIdealPos3D.destroy();
	}
}

CoverEstimate2D::IdealBlockResult CoverEstimate2D::estimateWithRegion(Match& match, PitchRegionShape* pBlockerRegion, 
											CtVector3& attackerPos, CtVector3& attackerVel, 
											CtVector3& targetPos, CtVector3& targetVel, 
											CtVector3& blockerPos, float blockerSpeed, Vector3& idealBlockPos, float blockerHeight) {

	Vector2 _attackerPos;
	Vector2 _attackerVel;
	Vector2 _targetPos;
	Vector2 _targetVel;
	Vector2 _blockerPos;
	Vector2 _idealBlockPos;

	match.toScene2D(attackerPos, _attackerPos);
	match.toScene2D(attackerVel, _attackerVel);
	match.toScene2D(targetPos, _targetPos);
	match.toScene2D(targetVel, _targetVel);
	match.toScene2D(blockerPos, _blockerPos);

	IdealBlockResult ret = estimateWithRegion(pBlockerRegion, _attackerPos, _attackerVel, _targetPos, _targetVel, 
												_blockerPos, blockerSpeed, _idealBlockPos);

	match.toScene3D(_idealBlockPos, idealBlockPos, blockerHeight);

	return ret;
}


CoverEstimate2D::IdealBlockResult CoverEstimate2D::estimateWithRegion(PitchRegionShape* pBlockerRegion, CtVector2& attackerPos, CtVector2& attackerVel, 
									CtVector2& targetPos, CtVector2& targetVel, 
									CtVector2& blockerPos, float blockerSpeed, Vector2& idealBlockPos) {


	IdealBlockResult result = estimate(attackerPos, attackerVel, targetPos, targetVel, 
										blockerPos, blockerSpeed, idealBlockPos);

	bool changed = false;

	if (pBlockerRegion && !pBlockerRegion->contains(idealBlockPos)) {

		changed = true;

		if (result == Result_AngleClose) {

			pBlockerRegion->clip(Vector2(idealBlockPos), idealBlockPos);

		} else {

			pBlockerRegion->clipByDest(targetPos, Vector2(idealBlockPos), idealBlockPos);
		}
	}

	return result;
}


CoverEstimate2D::IdealBlockResult CoverEstimate2D::exitEstimate(CtVector2& attackerPos, CtVector2& attackerVel, CtVector2& targetPos, 
																CtVector2& blockerPos, CtVector2& idealBlockPos, const float& blockerSpeed, const IdealBlockResult& result) {

	this->result = result;
	this->idealSpeed = blockerSpeed;

	if (this->frameInterval > 0.0f) {

		Vector2 posDiff;
		idealBlockPos.subtract(blockerPos, posDiff);

		float blockerSpeedSq = blockerSpeed * blockerSpeed;
		float frameIntervalSq = this->frameInterval * this->frameInterval;
		float distSq = posDiff.magSquared();
		float timeToReachSq = distSq / blockerSpeedSq;

		if (timeToReachSq < frameIntervalSq) {

			this->idealSpeed = sqrtf(distSq / frameIntervalSq);
		} 
	}
	
	/*
	{
		attackerPos.subtract(targetPos, posDiff);

		posDiff.normalize();
		projectionRest(attackerVel, posDiff, attackerTangentialVel);
	}
	*/

	return result;
}

CoverEstimate2D::IdealBlockResult CoverEstimate2D::estimate(CtVector2& attackerPos, CtVector2& attackerVel, 
																	CtVector2& targetPos, CtVector2& targetVel, 
																	CtVector2& blockerPos, float blockerSpeed, Vector2& idealBlockPos) {

	float u;

	{
		if (distanceSq(attackerPos, targetPos) <  distanceSq(blockerPos, targetPos)) {

			idealBlockPos = targetPos;
			return exitEstimate(attackerPos, attackerVel, targetPos, blockerPos,  idealBlockPos, blockerSpeed, Result_AttackerIsCloser);
		}
	}

	float blockerSpeedSq = blockerSpeed * blockerSpeed;
	float toleranceDistSq = this->toleranceDist * this->toleranceDist;

	float idealBlockDist;
	float timeToBlock;
	float idealBlockDistT1;
	float timeToBlockT1;

	float idealBlockPosU;
	float idealBlockPosUT1;

	Vector2 attackerPosT1;
	Vector2 targetPosT1;

	bool attackerHasVel = !attackerVel.isZero();
	bool targetHasVel = !targetVel.isZero();

	if (!attackerHasVel)
		attackerPosT1 = attackerPos;

	if (!targetHasVel)
		targetPosT1 = targetPos;

	bool isDynamic = attackerHasVel || targetHasVel;

	Vector2 angleClosePoint;
	float angleCloseDistSq = distanceSq(attackerPos, targetPos, blockerPos, angleClosePoint, u);
	float timeToCloseAngle = sqrtf(angleCloseDistSq / blockerSpeedSq);
	
	idealBlockPosU = this->blockOffset / distance(targetPos, attackerPos);

	/*
	{
		bool takeAngleClose = (u > idealBlockPosU) && (angleCloseDistSq > toleranceDistSq);

		if (takeAngleClose) {

			idealBlockPos = angleClosePoint;
			return exitEstimate(attackerPos, attackerVel, targetPos, blockerPos,  idealBlockPosblockerSpeed, Result_AngleClose);
		}
	}
	*/

	lerp(attackerPos, targetPos, idealBlockPosU, idealBlockPos);

	idealBlockDist = tmax(distance(blockerPos, idealBlockPos) - this->toleranceDist, 0.0f);
	timeToBlock = idealBlockDist / blockerSpeed;

	if (attackerHasVel)
		attackerPos.addMultiplication(attackerVel, timeToBlock, attackerPosT1);
	if (targetHasVel)
		targetPos.addMultiplication(targetVel, timeToBlock, targetPosT1);

	Vector2 tempBlockPos;

	idealBlockPosUT1 = this->blockOffset / distance(targetPosT1, attackerPosT1);
	lerp(attackerPosT1, targetPosT1, idealBlockPosUT1, tempBlockPos);

	idealBlockDistT1 = tmax(distance(blockerPos, tempBlockPos) - this->toleranceDist, 0.0f);
	timeToBlockT1 = idealBlockDistT1 / blockerSpeed;

	if (timeToBlockT1 <= timeToBlock || timeToBlockT1 <= timeToCloseAngle) {

		return exitEstimate(attackerPos, attackerVel, targetPos, blockerPos, idealBlockPos, blockerSpeed, Result_Success);
	} 

	/*
	if (this->frameInterval > 0.0f) {

		Vector2 posDiff;
		angleClosePoint.subtract(blockerPos, posDiff);

		float blockerSpeedSq = blockerSpeed * blockerSpeed;
		float frameIntervalSq = this->frameInterval * this->frameInterval;
		float distSq = posDiff.magSquared();
		float timeToReachSq = distSq / blockerSpeedSq;

		if (timeToReachSq < frameIntervalSq) {

			Circle circle;
			circle.center = blockerPos;
			circle.radius = blockerSpeed * this->frameInterval;

			float interU[2];
			int interCount;
			int takeUIndex = -1;

			if (circle.rayIntersect(attackerPos, targetPos, interU, interCount)) {

				if (interU[0] >= idealBlockPosU) {

					takeUIndex = 0;


				} else {

					if (interCount >= 2 && interU[1] > idealBlockPosU) {

						takeUIndex = 1;
					}
				}
			}

			if (takeUIndex >= 0) {

				lerp(attackerPos, targetPos, interU[takeUIndex], idealBlockPos);
				return exitEstimate(attackerPos, attackerVel, targetPos, blockerPos,  idealBlockPos, blockerSpeed, Result_Success);
			}
		} 
	}
	*/

	if (angleCloseDistSq <= toleranceDistSq) {

		Vector2 posDiff;

		blockerPos.subtract(attackerPos, posDiff);

		if (attackerVel.dot(posDiff) < 0.0f) {

			return exitEstimate(attackerPos, attackerVel, targetPos, blockerPos,  idealBlockPos, blockerSpeed, Result_Success);
		}
	}

	idealBlockPos = angleClosePoint;
	return exitEstimate(attackerPos, attackerVel, targetPos, blockerPos,  idealBlockPos, blockerSpeed, Result_Success);
}

} }

