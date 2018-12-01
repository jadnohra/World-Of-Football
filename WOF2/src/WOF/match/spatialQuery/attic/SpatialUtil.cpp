#include "SpatialUtil.h"
#include "../../../WOFApp.h"

#include "WE3/math/WEDistance.h"
using namespace WE;

/*
DECLARE_INSTANCE_TYPE_NAME(WOF::match::SpatialBlockEstimate, CSpatialBlockEstimate);

namespace WOF { namespace match {

const TCHAR* SpatialBlockEstimate::ScriptClassName = L"CSpatialBlockEstimate";

void SpatialBlockEstimate::declareInVM(SquirrelVM& target) {

	SqPlus::SQClassDef<SpatialBlockEstimate>(ScriptClassName).
		enumInt(SpatialBlockEstimate::Result_None, L"Result_None").
		enumInt(SpatialBlockEstimate::Result_Success, L"Result_Success").
		enumInt(SpatialBlockEstimate::Result_SourceIsGoingAway, L"Result_SourceIsGoingAway").
		enumInt(SpatialBlockEstimate::Result_SourceIsCloser, L"Result_SourceIsCloser").
		enumInt(SpatialBlockEstimate::Result_SuccessAngleClose, L"Result_SuccessAngleClose").
		var(&SpatialBlockEstimate::blockOffset, L"blockOffset").
		var(&SpatialBlockEstimate::toleranceDist, L"toleranceDist").
		func(&SpatialBlockEstimate::script_estimate, L"estimate").
		func(&SpatialBlockEstimate::script_getResult, L"getResult").
		func(&SpatialBlockEstimate::script_isWithinToleranceDist, L"isWithinToleranceDist").
		func(&SpatialBlockEstimate::script_getIdealBlockPos, L"getIdealBlockPos").
		func(&SpatialBlockEstimate::script_idealBlockPos, L"idealBlockPos").
		func(&SpatialBlockEstimate::script_getBlockerDirDotSourceVel, L"getBlockerDirDotSourceVel");
}

SpatialBlockEstimate::SpatialBlockEstimate() {

	result = Result_None;
	blockOffset = 0.0f;
	fallbackCount = 0;
	toleranceDist = 0.0f;
	isWithinToleranceDist = false;
	blockerDirDotSourceVel = 0.0f;
	idealBlockPos.zero();
}

SpatialBlockEstimate::IdealBlockResult SpatialBlockEstimate::exitEstimate(CtVector3& blockerPos, CtVector3& sourceVel, const IdealBlockResult& result) {

	this->result = result;

	Vector3 newDir;

	idealBlockPos.subtract(blockerPos, newDir);
	blockerDirDotSourceVel = newDir.dot(sourceVel);

	return result;
}

SpatialBlockEstimate::IdealBlockResult SpatialBlockEstimate::estimate(CtVector3& sourcePos, CtVector3& sourceVel, 
																	CtVector3& targetPos, CtVector3& targetVel, 
																	CtVector3& blockerPos, float blockerSpeed) {

	this->fallbackCount = 0;
	float u;

	{
		if (distanceSq(sourcePos, targetPos) <  distanceSq(blockerPos, targetPos)) {

			this->idealBlockPos = targetPos;
			return exitEstimate(blockerPos, sourceVel, Result_SourceIsCloser);
		}
	}

	float blockerSpeedSq = blockerSpeed * blockerSpeed;
	float toleranceDistSq = this->toleranceDist * this->toleranceDist;

	float idealBlockDistSq;
	float timeToBlock;
	float idealBlockDistSqT1;
	float timeToBlockT1;

	float idealBlockPosU;
	float idealBlockPosUT1;

	Vector3 sourcePosT1;
	Vector3 targetPosT1;

	bool sourceHasVel = !sourceVel.isZero();
	bool targetHasVel = !targetVel.isZero();

	if (!sourceHasVel)
		sourcePosT1 = sourcePos;

	if (!targetHasVel)
		targetPosT1 = targetPos;

	bool isDynamic = sourceHasVel || targetHasVel;

	Vector3 angleClosePoint;
	float angleCloseDistSq = distanceSq(sourcePos, targetPos, blockerPos, angleClosePoint, u);
	float timeToCloseAngle = sqrtf(angleCloseDistSq / blockerSpeedSq);
	
	idealBlockPosU = this->blockOffset / distance(targetPos, sourcePos);

	{
		bool takeAngleClose = (u > idealBlockPosU) && (angleCloseDistSq > toleranceDistSq);

		if (takeAngleClose) {

			this->idealBlockPos = angleClosePoint;
			return exitEstimate(blockerPos, sourceVel, Result_SuccessAngleClose);
		}
	}


	lerp(sourcePos, targetPos, idealBlockPosU, this->idealBlockPos);

	idealBlockDistSq = distanceSq(blockerPos, this->idealBlockPos);
	timeToBlock = sqrtf(idealBlockDistSq / blockerSpeedSq);

	if (sourceHasVel)
		sourcePos.addMultiplication(sourceVel, timeToBlock, sourcePosT1);
	if (targetHasVel)
		targetPos.addMultiplication(targetVel, timeToBlock, targetPosT1);

	Vector3 tempBlockPos;

	idealBlockPosUT1 = this->blockOffset / distance(targetPosT1, sourcePosT1);
	lerp(sourcePosT1, targetPosT1, idealBlockPosUT1, tempBlockPos);

	idealBlockDistSqT1 = distanceSq(blockerPos, tempBlockPos);
	timeToBlockT1 = sqrtf(idealBlockDistSqT1 / blockerSpeedSq);

	this->isWithinToleranceDist = (idealBlockDistSqT1 <= toleranceDistSq);


	if (timeToBlockT1 <= timeToBlock) {

		return exitEstimate(blockerPos, sourceVel, Result_Success);
	} 

	if (timeToBlockT1 <= timeToCloseAngle) {

		return exitEstimate(blockerPos, sourceVel, Result_Success);
	}

	this->idealBlockPos = angleClosePoint;
	return exitEstimate(blockerPos, sourceVel, Result_Success);
}

SpatialBlockEstimate::IdealBlockResult SpatialBlockEstimate::estimateConstantSpeeds(CtVector3& sourcePos, CtVector3& realSourceVel, 
																	CtVector3& targetPos, CtVector3& targetVel, 
																	CtVector3& blockerPos, float blockerSpeed, 
																	float timeLimit, bool fallBackToZeroVelVelIfIsGoingAway, int iterCount, CtVector3* pSourceVelOverride) {

	const CtVector3& sourceVel = pSourceVelOverride ? *pSourceVelOverride : realSourceVel;

	if (iterCount == 0) {
		this->fallbackCount = 0;
	}

	if (!sourceVel.isZero()) {

		Vector3 sourceDir;

		targetPos.subtract(sourcePos, sourceDir);

		if (sourceDir.dot(sourceVel) <= 0.0f) {

			if (fallBackToZeroVelVelIfIsGoingAway) {

				++this->fallbackCount;

				return estimateConstantSpeeds(sourcePos, realSourceVel, targetPos, targetVel, 
									blockerPos, blockerSpeed, timeLimit, false, iterCount + 1, &CtVector3::kZero);
			}

			return exitEstimate(blockerPos, realSourceVel, Result_SourceIsGoingAway);
		}
	}

	{
		if (distanceSq(sourcePos, targetPos) <  distanceSq(blockerPos, targetPos)) {

			this->idealBlockPos = targetPos;
			return exitEstimate(blockerPos, realSourceVel, Result_SourceIsCloser);
		}
	}

	float blockerSpeedSq = blockerSpeed * blockerSpeed;
	float toleranceDistSq = this->toleranceDist * this->toleranceDist;

	float idealBlockDistSq;
	float timeToBlock;
	float idealBlockDistSqT1;
	float timeToBlockT1;

	float idealBlockPosU;
	float idealBlockPosUT1;

	float currTime;
	float currArrivalTime;

	Vector3 sourcePosT1;
	Vector3 targetPosT1;

	bool sourceHasVel = !sourceVel.isZero();
	bool targetHasVel = !targetVel.isZero();

	if (!sourceHasVel)
		sourcePosT1 = sourcePos;

	if (!targetHasVel)
		targetPosT1 = targetPos;

	bool isDynamic = sourceHasVel || targetHasVel;

	float u;
	Vector3 angleClosePoint;
	float angleCloseDistSq = distanceSq(sourcePos, targetPos, blockerPos, angleClosePoint, u);
	float timeToCloseAngle = sqrtf(angleCloseDistSq / blockerSpeedSq);
	
	idealBlockPosU = this->blockOffset / distance(targetPos, sourcePos);

	{
		bool takeAngleClose = (u > idealBlockPosU) && (angleCloseDistSq > toleranceDistSq);

		if (takeAngleClose) {

			this->idealBlockPos = angleClosePoint;
			return exitEstimate(blockerPos, realSourceVel, Result_SuccessAngleClose);
		}
	}


	//idealBlockPosU = this->blockOffset / distance(targetPos, sourcePos);
	lerp(sourcePos, targetPos, idealBlockPosU, this->idealBlockPos);

	idealBlockDistSq = distanceSq(blockerPos, this->idealBlockPos);
	timeToBlock = sqrtf(idealBlockDistSq / blockerSpeedSq);

	if (sourceHasVel)
		sourcePos.addMultiplication(sourceVel, timeToBlock, sourcePosT1);
	if (targetHasVel)
		targetPos.addMultiplication(targetVel, timeToBlock, targetPosT1);

	idealBlockPosUT1 = this->blockOffset / distance(targetPosT1, sourcePosT1);
	lerp(sourcePosT1, targetPosT1, idealBlockPosUT1, this->idealBlockPos);

	idealBlockDistSqT1 = distanceSq(blockerPos, this->idealBlockPos);
	timeToBlockT1 = sqrtf(idealBlockDistSqT1 / blockerSpeedSq);

	this->isWithinToleranceDist = (idealBlockDistSqT1 <= toleranceDistSq);

	currTime = timeToBlock;
	currArrivalTime = timeToBlockT1;

	while (currArrivalTime > currTime) {

		if (currTime > timeLimit) 
			break;

		currTime = currArrivalTime;

		if (sourceHasVel)
			sourcePos.addMultiplication(sourceVel, currTime, sourcePosT1);

		if (targetHasVel)
			targetPos.addMultiplication(targetVel, currTime, targetPosT1);

		idealBlockPosUT1 = this->blockOffset / distance(targetPosT1, sourcePosT1);
		lerp(sourcePosT1, targetPosT1, idealBlockPosUT1, this->idealBlockPos);

		idealBlockDistSqT1 = distanceSq(blockerPos, this->idealBlockPos);
		timeToBlockT1 = sqrtf(idealBlockDistSqT1 / blockerSpeedSq);

		this->isWithinToleranceDist = (idealBlockDistSqT1 <= toleranceDistSq);

		currArrivalTime = timeToBlockT1;
	}

	if (currTime > timeLimit) {

		this->idealBlockPos = angleClosePoint;

		return exitEstimate(blockerPos, realSourceVel, Result_SuccessAngleClose);
	}
	
	return exitEstimate(blockerPos, realSourceVel, Result_Success);
}

} }
*/


DECLARE_INSTANCE_TYPE_NAME(WOF::match::SpatialBlockEstimate2D, CSpatialBlockEstimate2D);

namespace WOF { namespace match {

const TCHAR* SpatialBlockEstimate2D::ScriptClassName = L"CSpatialBlockEstimate2D";

void SpatialBlockEstimate2D::declareInVM(SquirrelVM& target) {

	SqPlus::SQClassDef<SpatialBlockEstimate2D>(ScriptClassName).
		enumInt(SpatialBlockEstimate2D::Result_None, L"Result_None").
		enumInt(SpatialBlockEstimate2D::Result_Success, L"Result_Success").
		enumInt(SpatialBlockEstimate2D::Result_SourceIsGoingAway, L"Result_SourceIsGoingAway").
		enumInt(SpatialBlockEstimate2D::Result_SourceIsCloser, L"Result_SourceIsCloser").
		enumInt(SpatialBlockEstimate2D::Result_SuccessAngleClose, L"Result_SuccessAngleClose").
		var(&SpatialBlockEstimate2D::blockOffset, L"blockOffset").
		var(&SpatialBlockEstimate2D::toleranceDist, L"toleranceDist").
		var(&SpatialBlockEstimate2D::frameInterval, L"frameInterval").
		func(&SpatialBlockEstimate2D::script_estimate, L"estimate").
		func(&SpatialBlockEstimate2D::script_getResult, L"getResult").
		func(&SpatialBlockEstimate2D::script_estimateWithRegion, L"estimateWithRegion").
		func(&SpatialBlockEstimate2D::script_getIdealBlockPos, L"getIdealBlockPos").
		func(&SpatialBlockEstimate2D::script_idealBlockPos, L"idealBlockPos").
		func(&SpatialBlockEstimate2D::script_getIdealBlockerSpeed, L"getIdealBlockerSpeed").
		func(&SpatialBlockEstimate2D::script_getSourceTangentialVel, L"getSourceTangentialVel");
}

SpatialBlockEstimate2D::SpatialBlockEstimate2D() {

	result = Result_None;
	blockOffset = 0.0f;
	fallbackCount = 0;
	toleranceDist = 0.0f;
	idealBlockPos.zero();
	frameInterval = 0.0f;
}


int SpatialBlockEstimate2D::script_estimateWithRegion(CtVector2& sourcePos, CtVector2& sourceVel, 
									CtVector2& targetPos, CtVector2& targetVel, 
									CtVector2& blockerPos, float blockerSpeed, 
									PitchRegionShape* pBlockerRegion) {


	IdealBlockResult result = estimate(sourcePos, sourceVel, targetPos, targetVel, 
										blockerPos, blockerSpeed);

	bool changed = false;

	if (pBlockerRegion && !pBlockerRegion->contains(this->idealBlockPos)) {

		changed = true;

		if (result == Result_SuccessAngleClose) {

			pBlockerRegion->clip(Vector2(this->idealBlockPos), this->idealBlockPos);

		} else {

			pBlockerRegion->clipByDest(targetPos, Vector2(this->idealBlockPos), this->idealBlockPos);
		}
	}

	return result;
}


SpatialBlockEstimate2D::IdealBlockResult SpatialBlockEstimate2D::exitEstimate(CtVector2& sourcePos, CtVector2& sourceVel, CtVector2& targetPos, 
																				CtVector2& blockerPos, const float& blockerSpeed, const IdealBlockResult& result) {

	this->result = result;
	this->idealBlockerSpeed = blockerSpeed;

	Vector2 posDiff;
	idealBlockPos.subtract(blockerPos, posDiff);

	if (this->frameInterval > 0.0f) {

		float blockerSpeedSq = blockerSpeed * blockerSpeed;
		float frameIntervalSq = this->frameInterval * this->frameInterval;
		float distSq = posDiff.magSquared();
		float timeToReachSq = distSq / blockerSpeedSq;

		if (timeToReachSq < frameIntervalSq) {

			this->idealBlockerSpeed = sqrtf(distSq / frameIntervalSq);
		} 
	}

	{
		sourcePos.subtract(targetPos, posDiff);

		posDiff.normalize();
		projectionRest(sourceVel, posDiff, sourceTangentialVel);
	}

	return result;
}

SpatialBlockEstimate2D::IdealBlockResult SpatialBlockEstimate2D::estimate(CtVector2& sourcePos, CtVector2& sourceVel, 
																	CtVector2& targetPos, CtVector2& targetVel, 
																	CtVector2& blockerPos, float blockerSpeed) {

	float u;
	this->fallbackCount = 0;

	{
		if (distanceSq(sourcePos, targetPos) <  distanceSq(blockerPos, targetPos)) {

			this->idealBlockPos = targetPos;
			return exitEstimate(sourcePos, sourceVel, targetPos, blockerPos, blockerSpeed, Result_SourceIsCloser);
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

	Vector2 sourcePosT1;
	Vector2 targetPosT1;

	bool sourceHasVel = !sourceVel.isZero();
	bool targetHasVel = !targetVel.isZero();

	if (!sourceHasVel)
		sourcePosT1 = sourcePos;

	if (!targetHasVel)
		targetPosT1 = targetPos;

	bool isDynamic = sourceHasVel || targetHasVel;

	Vector2 angleClosePoint;
	float angleCloseDistSq = distanceSq(sourcePos, targetPos, blockerPos, angleClosePoint, u);
	float timeToCloseAngle = sqrtf(angleCloseDistSq / blockerSpeedSq);
	
	idealBlockPosU = this->blockOffset / distance(targetPos, sourcePos);

	/*
	{
		bool takeAngleClose = (u > idealBlockPosU) && (angleCloseDistSq > toleranceDistSq);

		if (takeAngleClose) {

			this->idealBlockPos = angleClosePoint;
			return exitEstimate(sourcePos, sourceVel, targetPos, blockerPos, blockerSpeed, Result_SuccessAngleClose);
		}
	}
	*/

	lerp(sourcePos, targetPos, idealBlockPosU, this->idealBlockPos);

	idealBlockDist = tmax(distance(blockerPos, this->idealBlockPos) - this->toleranceDist, 0.0f);
	timeToBlock = idealBlockDist / blockerSpeed;

	if (sourceHasVel)
		sourcePos.addMultiplication(sourceVel, timeToBlock, sourcePosT1);
	if (targetHasVel)
		targetPos.addMultiplication(targetVel, timeToBlock, targetPosT1);

	Vector2 tempBlockPos;

	idealBlockPosUT1 = this->blockOffset / distance(targetPosT1, sourcePosT1);
	lerp(sourcePosT1, targetPosT1, idealBlockPosUT1, tempBlockPos);

	idealBlockDistT1 = tmax(distance(blockerPos, tempBlockPos) - this->toleranceDist, 0.0f);
	timeToBlockT1 = idealBlockDistT1 / blockerSpeed;

	if (timeToBlockT1 <= timeToBlock || timeToBlockT1 <= timeToCloseAngle) {

		return exitEstimate(sourcePos, sourceVel, targetPos, blockerPos, blockerSpeed, Result_Success);
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

			if (circle.rayIntersect(sourcePos, targetPos, interU, interCount)) {

				if (interU[0] >= idealBlockPosU) {

					takeUIndex = 0;


				} else {

					if (interCount >= 2 && interU[1] > idealBlockPosU) {

						takeUIndex = 1;
					}
				}
			}

			if (takeUIndex >= 0) {

				lerp(sourcePos, targetPos, interU[takeUIndex], this->idealBlockPos);
				return exitEstimate(sourcePos, sourceVel, targetPos, blockerPos, blockerSpeed, Result_Success);
			}
		} 
	}
	*/

	this->idealBlockPos = angleClosePoint;
	return exitEstimate(sourcePos, sourceVel, targetPos, blockerPos, blockerSpeed, Result_Success);
}

/*
SpatialBlockEstimate2D::IdealBlockResult SpatialBlockEstimate2D::estimateConstantSpeeds(CtVector2& sourcePos, CtVector2& realSourceVel, 
																	CtVector2& targetPos, CtVector2& targetVel, 
																	CtVector2& blockerPos, float blockerSpeed, 
																	float timeLimit, bool fallBackToZeroVelVelIfIsGoingAway, int iterCount, CtVector2* pSourceVelOverride) {

	const CtVector2& sourceVel = pSourceVelOverride ? *pSourceVelOverride : realSourceVel;

	if (iterCount == 0) {
		this->fallbackCount = 0;
	}

	if (!sourceVel.isZero()) {

		Vector2 sourceDir;

		targetPos.subtract(sourcePos, sourceDir);

		if (sourceDir.dot(sourceVel) <= 0.0f) {

			if (fallBackToZeroVelVelIfIsGoingAway) {

				++this->fallbackCount;

				return estimateConstantSpeeds(sourcePos, realSourceVel, targetPos, targetVel, 
									blockerPos, blockerSpeed, timeLimit, false, iterCount + 1, &CtVector2::kZero);
			}

			return exitEstimate(blockerPos, realSourceVel, blockerSpeed, Result_SourceIsGoingAway);
		}
	}

	{
		if (distanceSq(sourcePos, targetPos) <  distanceSq(blockerPos, targetPos)) {

			this->idealBlockPos = targetPos;
			return exitEstimate(blockerPos, realSourceVel, blockerSpeed, Result_SourceIsCloser);
		}
	}

	float blockerSpeedSq = blockerSpeed * blockerSpeed;
	float toleranceDistSq = this->toleranceDist * this->toleranceDist;

	float idealBlockDistSq;
	float timeToBlock;
	float idealBlockDistSqT1;
	float timeToBlockT1;

	float idealBlockPosU;
	float idealBlockPosUT1;

	float currTime;
	float currArrivalTime;

	Vector2 sourcePosT1;
	Vector2 targetPosT1;

	bool sourceHasVel = !sourceVel.isZero();
	bool targetHasVel = !targetVel.isZero();

	if (!sourceHasVel)
		sourcePosT1 = sourcePos;

	if (!targetHasVel)
		targetPosT1 = targetPos;

	bool isDynamic = sourceHasVel || targetHasVel;

	float u;
	Vector2 angleClosePoint;
	float angleCloseDistSq = distanceSq(sourcePos, targetPos, blockerPos, angleClosePoint, u);
	float timeToCloseAngle = sqrtf(angleCloseDistSq / blockerSpeedSq);
	
	idealBlockPosU = this->blockOffset / distance(targetPos, sourcePos);

	{
		bool takeAngleClose = (u > idealBlockPosU) && (angleCloseDistSq > toleranceDistSq);

		if (takeAngleClose) {

			this->idealBlockPos = angleClosePoint;
			return exitEstimate(blockerPos, realSourceVel, blockerSpeed, Result_SuccessAngleClose);
		}
	}


	//idealBlockPosU = this->blockOffset / distance(targetPos, sourcePos);
	lerp(sourcePos, targetPos, idealBlockPosU, this->idealBlockPos);

	idealBlockDistSq = distanceSq(blockerPos, this->idealBlockPos);
	timeToBlock = sqrtf(idealBlockDistSq / blockerSpeedSq);

	if (sourceHasVel)
		sourcePos.addMultiplication(sourceVel, timeToBlock, sourcePosT1);
	if (targetHasVel)
		targetPos.addMultiplication(targetVel, timeToBlock, targetPosT1);

	idealBlockPosUT1 = this->blockOffset / distance(targetPosT1, sourcePosT1);
	lerp(sourcePosT1, targetPosT1, idealBlockPosUT1, this->idealBlockPos);

	idealBlockDistSqT1 = distanceSq(blockerPos, this->idealBlockPos);
	timeToBlockT1 = sqrtf(idealBlockDistSqT1 / blockerSpeedSq);

	this->isWithinToleranceDist = (idealBlockDistSqT1 <= toleranceDistSq);

	currTime = timeToBlock;
	currArrivalTime = timeToBlockT1;

	while (currArrivalTime > currTime) {

		if (currTime > timeLimit) 
			break;

		currTime = currArrivalTime;

		if (sourceHasVel)
			sourcePos.addMultiplication(sourceVel, currTime, sourcePosT1);

		if (targetHasVel)
			targetPos.addMultiplication(targetVel, currTime, targetPosT1);

		idealBlockPosUT1 = this->blockOffset / distance(targetPosT1, sourcePosT1);
		lerp(sourcePosT1, targetPosT1, idealBlockPosUT1, this->idealBlockPos);

		idealBlockDistSqT1 = distanceSq(blockerPos, this->idealBlockPos);
		timeToBlockT1 = sqrtf(idealBlockDistSqT1 / blockerSpeedSq);

		this->isWithinToleranceDist = (idealBlockDistSqT1 <= toleranceDistSq);

		currArrivalTime = timeToBlockT1;
	}

	if (currTime > timeLimit) {

		this->idealBlockPos = angleClosePoint;

		return exitEstimate(blockerPos, realSourceVel, blockerSpeed, Result_Success);
	}
	
	return exitEstimate(blockerPos, realSourceVel, blockerSpeed, Result_Success);
}
*/

} }

