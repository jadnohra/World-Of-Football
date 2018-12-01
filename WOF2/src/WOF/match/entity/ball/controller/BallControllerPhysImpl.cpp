#include "BallControllerPhysImpl.h"

#include "BallControllerType.h"

#include "../../../Match.h"
#include "../Ball.h"

#include "SpecializedCollisionLaws.h"

namespace WOF { namespace match { 

//in meters
static float penetrationCorrectionMaxLimitMeters = 0.002f;
static float penetrationCorrectionMaxLimit = 0.0f;

//#define BallControllerPhysImpl_GENERATE_BALL_LOG

int BallControllerPhysImpl::getControllerType() {

	return BallController_Phys;
}

BallControllerPhysImpl::BallControllerPhysImpl() {

	mStaticWorld.init(true);

	mContactGeometry.init(10);

	mSweptIntersections.reserve(10);
	mSweptIntersectionPtrs.reserve(10);

	mIntersectionStates.reserve(10);

	mRestingIntersections.reserve(10);

}

void BallControllerPhysImpl::init(Match& match) {

	Vector3 gravityAcc;

	const FastUnitCoordSys& coordSys = match.getCoordSys();

	coordSys.getUnitVector(CSD_Up, gravityAcc.el);
	gravityAcc.mul(coordSys.convUnit(-9.8f) * match.mTweakGravityCoeff);
	mGravity.init(gravityAcc, true);

	penetrationCorrectionMaxLimit = coordSys.convUnit(penetrationCorrectionMaxLimitMeters);

	mAlmostRestingSpeed = coordSys.convUnit(0.13f);
}

BallControllerPhysImpl::~BallControllerPhysImpl() {
}


void BallControllerPhysImpl::checkAndSetResting(Ball& ball, const bool& allResting) {

	if (allResting 
		/*&& ball.mPhysBody.w.isZero() */
		&& (ball.mPhysBody.w.magSquared() <= (mAlmostRestingSpeed * mAlmostRestingSpeed) / (18.0f * 18.0f))
		&& (ball.mPhysBody.v.magSquared() <= mAlmostRestingSpeed * mAlmostRestingSpeed)
		/*&& (fabs(ball.mPhysBody.v.el[Scene_Right]) <= mAlmostRestingSpeed)
		&& (fabs(ball.mPhysBody.v.el[Scene_Forward]) <= mAlmostRestingSpeed)
		&& (fabs(ball.mPhysBody.v.el[Scene_Up]) <= mAlmostRestingSpeed)*/) {

		ball.physSetResting(true);

	} else {

		ball.physSetResting(false);
	}
}

void BallControllerPhysImpl::wakeUpBall() {
}

void BallControllerPhysImpl::setPosition(Ball& ball, const Point& pos, const BallCommand* pCommand) {

	ball.mPhysBody.pos = pos;
	ball.physSyncNodeFromPhysBody();

	ball.mPhysBody.v.zero();
	ball.mPhysBody.setLinMomFromV();

	ball.mPhysBody.w.zero();
	ball.mPhysBody.setAngMomFromW();

	ball.notifySimulListeners(pCommand);
}

void BallControllerPhysImpl::setPosition(const Point& pos, const BallCommand* pCommand) {

	wakeUpBall();
	setPosition(mBall, pos, pCommand);
}


void BallControllerPhysImpl::attach(Match& match, Ball* pBall) {

	BallController::attach(match, pBall);

	if (mBall.isValid()) {
		
		mBall->physMarkDesynced();

		wakeUpBall();

		mBall->mPhysBody.v.zero();
		mBall->mPhysBody.setLinMomFromV();

		mBall->mPhysBody.w.zero();
		mBall->mPhysBody.setAngMomFromW();

		mBall->physSyncPhysBodyFromNode();
		mBall->mPhysBody.force.zero();
		mGravity.apply(mBall->mPhysBody);
	}
}

static int _letCollCount = 0;
static int _letColl = 1;
static bool letMax = false;

void BallControllerPhysImpl::setVelocities(Ball& ball, const Vector& v, const Vector& w, const BallCommand* pCommand) {

	ball.mPhysBody.v = v;
	ball.mPhysBody.setLinMomFromV();

	ball.mPhysBody.w = w;
	ball.mPhysBody.setAngMomFromW();

	ball.notifySimulListeners(pCommand);
}

void BallControllerPhysImpl::setVelocities(const Vector& v, const Vector& w, const BallCommand* pCommand) {

	wakeUpBall();
	setVelocities(mBall, v, w, pCommand);
}

void BallControllerPhysImpl::setV(const Vector& v, const BallCommand* pCommand) {

	wakeUpBall();

	mBall->mPhysBody.v = v;
	mBall->mPhysBody.setLinMomFromV();

	mBall->notifySimulListeners(pCommand);
}

//bool brk = false;

void BallControllerPhysImpl::blend(const Ball& ball, const Vector& currV, const Vector* pCurrW, const Vector& v, float blendCurrVRatio, const Vector& normal, float rollSyncRatio, bool blendCurrW, float blendRatio, Vector& outV, Vector& outW) {

	//brk = true;

	lerp(v, currV, blendCurrVRatio, outV);
	
	if (blendCurrW) {

		Vector3 temp;
		Vector blendW;

		normal.cross(v, temp);
		temp.mul(rollSyncRatio / ball.mSphereLocal.radius, blendW);

		lerp(blendW, *pCurrW, blendRatio, outW);

		
	} else {

		Vector3 temp;

		normal.cross(v, temp);
		temp.mul(rollSyncRatio / ball.mSphereLocal.radius, outW);
	}
}

void BallControllerPhysImpl::applyAfterTouch(const Vector3& acceleration, float rotSyncRatio, float dt, const BallCommand* pCommand) {

	Vector3 diff;
	Ball& ball = mBall;

	acceleration.mul(dt, diff);
	ball.mPhysBody.v.add(diff);

	{
		ball.accessAfterTouchVel().add(diff);
	}
	
	if (rotSyncRatio != 0.0f) {

		Vector3 rotAxis;
		Vector3 temp;

		acceleration.cross(ball.mNodeMatch->getCoordAxis(Scene_Up), rotAxis);

		if (rotAxis.isZero()) {

			ball.mNodeMatch->getCoordAxis(Scene_Forward).cross(acceleration, rotAxis);
		} 

		rotAxis.normalize();

		acceleration.mul(rotSyncRatio * dt / ball.mSphereLocal.radius, temp);
		temp.abs();
		temp.cross(rotAxis, diff);
		ball.mPhysBody.w.add(diff);
	}

	ball.mPhysBody.setLinMomFromV();
	ball.mPhysBody.setAngMomFromW();

	ball.notifySimulListeners(pCommand);
}

bool BallControllerPhysImpl::executeCommand(const BallCommand& command) {

	bool ret = false;

	switch (command.type) {

		case BallCommand_Pos: {

			/*
			Vector3 v;
			
			const Clock& timing = mBall->mNodeMatch->getClock();
			float dt = timing.getFramePhysTickCount() * timing.getFramePhysTickLength();

			//sync phys props
			command.pos.pos.subtract(mBall->mPhysBody.pos, v);
			v.div(dt);

			setV(v, 0.0f, command.pos.rotateNormal, command.pos.rotate ? 1.0f : 0.0f, false, 0.0f, &command);
			*/
			/*
			log(L"command pos %f,%f,%f %f,%f,%f", 
				command.pos.v.x, command.pos.v.y, command.pos.v.z, 
				command.pos.w.x, command.pos.w.y, command.pos.w.z);
				*/

			setVelocities(command.pos.v, command.pos.w, &command);

			ret = true;

		} break;

		case BallCommand_Vel: {

			/*
			setV(command.vel.v, command.vel.blendCurrVRatio, command.vel.rotSyncNormal, command.vel.rotSyncRatio, (command.vel.blendCurrRotRatio != 0.0f), command.vel.blendCurrRotRatio, &command); 
			*/

			/*
			log(L"command vel %f,%f,%f %f,%f,%f", 
				command.vel.v.x, command.vel.v.y, command.vel.v.z, 
				command.vel.w.x, command.vel.w.y, command.vel.w.z);
				*/


			setVelocities(command.vel.v, command.vel.w, &command);

			ret = true;

		} break;

		case BallCommand_AfterTouch: {

			if (mBall->isAfterTouchOwner(&command)) {

				const Clock& timing = mBall->mNodeMatch->getClock();
				float dt = timing.getFramePhysTickCount() * timing.getFramePhysTickLength();

				applyAfterTouch(command.afterTouch.acceleration, command.afterTouch.rotSyncRatio, dt, &command);

				ret = true;

			} else {

				ret = false;
			}

		} break;

		case BallCommand_Guard: {

			ret = true;

		} break;
	}

	return ret;
}

void BallControllerPhysImpl::setV(const Vector& v, float blendCurrVRatio, const Vector& normal, float rollSyncRatio, bool blendCurrW, float blendRatio, const BallCommand* pCommand) {

	wakeUpBall();

	blend(mBall, mBall->mPhysBody.v, &mBall->mPhysBody.w, v, blendCurrVRatio, normal, rollSyncRatio, blendCurrW, blendRatio, mBall->mPhysBody.v, mBall->mPhysBody.w);

	mBall->mPhysBody.setLinMomFromV();
	mBall->mPhysBody.setAngMomFromW();
	
	mBall->notifySimulListeners(pCommand);
}

void BallControllerPhysImpl::setW(const Vector& w, const BallCommand* pCommand) {

	wakeUpBall();

	mBall->mPhysBody.w = w;
	mBall->mPhysBody.setAngMomFromW();

	mBall->notifySimulListeners(pCommand);
}

template<class T>
int compareSweptFractionMember(const void *_pContact1, const void *_pContact2) {

	T* pContact1 = *((T**) _pContact1);
	T* pContact2 = *((T**) _pContact2);

	float diff = (pContact1->sweptFraction) - (pContact2->sweptFraction);

	if (diff < 0.0f) {

		return -1;
	} else if (diff > 0.0f) {

		return 1;
	}

	return 0;
}

void BallControllerPhysImpl::setupCurrContactBody(SweptIntersection& intersection) {

	/*
	if (intersection.contactHasVelocity) {

		mDynamicBox.isFixed = false;

		mCurrContactBody = &mDynamicBox;
		mDynamicBox.mass = intersection.contactMass;
		mDynamicBox.massInv = 1.0f / mDynamicBox.mass;

		mDynamicBox.ITensor.type = IT_Simple;

		const float& radius2 = intersection.contactBodyRadiusSq;
		mDynamicBox.ITensor.tensorSimple.IBody = (0.67f * mDynamicBox.mass * (radius2));
		mDynamicBox.ITensor.tensorSimple.IBodyInv = 1.0f / mDynamicBox.ITensor.tensorSimple.IBody;

		mDynamicBox.pos = intersection.contactBodyPos;
		mDynamicBox.v = intersection.contactVelocity;
		mDynamicBox.w.zero();

	} else {

		mCurrContactBody = &mStaticWorld;
	}
	*/

	mCurrContactBody = &mStaticWorld;
}

void BallControllerPhysImpl::determinIntersectionDynamics(SweptIntersection& intersection, Ball& ball, Vector& sweptVector, SweptContactDynamics& dynamics) {
	
	sweptVector.mul(intersection.sweptFraction, dynamics.sweptVectorAtContact);
	ball.mPhysBody.pos.add(dynamics.sweptVectorAtContact, dynamics.posAtContact);

	
	//hack for now, for dynamic objects which override contactNormal
	//if (intersection.contactHasVelocity) {
//
//		intersection.contactVelocity.normalize(dynamics.normal);
//		dynamics.penetration = ball.mSphereLocal.radius - dynamics.normal.mag();
//
//		dynamics.relVel.add(intersection.contactVelocity);
//		dynamics.relVelNMag += intersection.contactVelocity.mag();
//
//	} else { 


		//works for both normal and edge contacts because of sphericity
		//also automatically supports double sided faces because we are 
		//actually ignoring the direction of the face normal
		dynamics.posAtContact.subtract(intersection.contactPoint, dynamics.normal);
		dynamics.penetration = ball.mSphereLocal.radius - dynamics.normal.mag();
		dynamics.normal.normalize();
//	}

	//flexible materials
	/*
	it seems it is not enough to tweak the normal, the radius vector
	also nees to be tweeked look in 
	PhysCollisionLawFrictionlessNewton::computeImpulse
	static bool allowFlex = false;
	if (allowFlex && intersection.pMaterial->isFlexible) {

		float vMag;
		Vector3 cancelDir;
		Vector3 flexibleNormal;
		
		ball.v.normalize(cancelDir, vMag);

		if (vMag != 0.0f) {

			cancelDir.mul(-1.0f);
			lerp(dynamics.normal, cancelDir, intersection.pMaterial->flexible, flexibleNormal);
			dynamics.normal = flexibleNormal;
		}
	}
	*/

	setupCurrContactBody(intersection);

	ball.mPhysBody.savePos(dynamics.posAtContact);
	PhysConstraintContact::getContactVelocity(mCurrContactBody, ball.mPhysBody, dynamics.normal, intersection.contactPoint, dynamics.relVel, dynamics.relVelNMag);
	ball.mPhysBody.loadSavedPos();
}


SweptContactDynamicType determinContactDynamicType(const PhysScalar& vrel, const float& tolerance/*, const bool& contactHasVelocity*/) {
	
	/*
	if (contactHasVelocity) {

		return SCDT_Colliding;
	}
	*/

	if (physAbs(vrel) <= tolerance) {

		return SCDT_Resting;
	}

	if (vrel > 0.0f) {

		//if (sweptFraction == 0.0f) {

		//	return MCD_Resting;
		//} else {

			return SCDT_Separating;
		//}
	}

	return SCDT_Colliding;
}


SweptFractionType determinFractionType(float fraction, float tolerance) {

	if (fraction == 0.0f) {
		
		return SFT_Zero;

	} else if (fraction - tolerance < 0.0f) {

		return SFT_ZeroEpsilon;

	} 
	
	return SFT_Delta;
}

void BallControllerPhysImpl::balanceBallPointVelocity2(Ball& ball, SweptIntersection& intersection, SweptContactDynamics& dynamics, PhysMaterial& material, const float& dt) {

	Vector3 r;
	Vector3 vtFromV;
	Vector3 vtFromW;
	Vector3 vtFromVDir;
	Vector3 vtFromWDir;
	float vtFromVMag;
	float vtFromWMag;
	PhysScalar maxLinMomMag;
	PhysScalar maxForceMag;
	PhysScalar maxTorqueMag;

	
	float balancedVtMag;
	float lerpFactor;
	Vector3 lerpExtreme1;
	Vector3 lerpExtreme2;
	Vector3 balancedVtFromVDir;
	Vector3 balancedVtFromWDir;
	Vector3 balancedVt;
	float neededMag;

	Vector3 temp;
	Vector3 temp1;
	float rMag;
	Vector3 diff;
	
	if (dynamics.relVelNMag >= 0.0f) {

		return;
	}

	//if (brk) {
	//	brk = brk;
	//}

	//algorithmically finds the approximate end Vt from w and v 
	//and changes v and w to reflect it

	ball.mPhysBody.getBodyPointVector(dynamics.posAtContact, intersection.contactPoint, r);
	ball.mPhysBody.getBodyPointVelocitiesT(r, dynamics.normal, vtFromV, vtFromW);

	//not really, approximation? would need compare mass and Inertia Tensor?
	vtFromVMag = vtFromV.mag();
	vtFromWMag = vtFromW.mag();
	balancedVtMag = (vtFromVMag + vtFromWMag) * 0.5f;

	if (fabs(vtFromWMag - vtFromVMag) <= mAlmostRestingSpeed) {

		return;
	}

	if (vtFromWMag < vtFromVMag) {

		if (vtFromWMag != 0.0f) {

			vtFromV.div(-vtFromVMag, lerpExtreme1);
			lerpFactor = vtFromWMag / vtFromVMag;

			vtFromW.div(vtFromWMag, lerpExtreme2);
			lerp(lerpExtreme1, lerpExtreme2, lerpFactor, balancedVtFromWDir);

		} else {

			vtFromV.div(-vtFromVMag, balancedVtFromWDir);
		}

		balancedVtFromWDir.mul(-1.0f, balancedVtFromVDir);

	} else {

		if (vtFromVMag != 0.0f) {

			vtFromW.div(-vtFromWMag, lerpExtreme1);
			lerpFactor = vtFromVMag / vtFromWMag;

			vtFromV.div(vtFromVMag, lerpExtreme2);
			lerp(lerpExtreme1, lerpExtreme2, lerpFactor, balancedVtFromVDir);

		} else {

			vtFromW.div(-vtFromWMag, balancedVtFromVDir);
		}

		balancedVtFromVDir.mul(-1.0f, balancedVtFromWDir);
	}


	rMag = r.mag();

	maxLinMomMag = material.frictionCoeff * -(dynamics.relVelNMag + (dynamics.relVelNMag * material.nRestCoeff));
	maxForceMag = (ball.mPhysBody.mass / dt) * maxLinMomMag;
	maxTorqueMag = rMag * maxForceMag;


	//set new v and w
	//remove old vt and add new vt
	balancedVtFromVDir.mul(balancedVtMag, balancedVt);
	balancedVt.subtract(vtFromV, diff);

	//momentum
	neededMag = diff.mag();

	if (neededMag > maxLinMomMag) {

		lerp(vtFromV, balancedVt, maxLinMomMag / neededMag, temp);
		temp.subtract(vtFromV, diff);
	}
	
	ball.mPhysBody.v += diff;


	balancedVtFromWDir.mul(balancedVtMag, balancedVt);
	balancedVt.subtract(vtFromW, diff);

	ball.mPhysBody.ITensor.computeMom(diff, temp1);
	//needed torque
	neededMag = temp1.mag() / (rMag * dt); 

	if (neededMag > maxTorqueMag) {

		lerp(vtFromW, balancedVt, maxTorqueMag / neededMag, temp1);
		temp1.subtract(vtFromW, diff);
	}

	r.div(rMag, temp1); 
	temp1.cross(diff, temp);
	temp.div(rMag);
	ball.mPhysBody.w += temp;

	ball.mPhysBody.getBodyPointVelocitiesT(r, dynamics.normal, vtFromV, vtFromW);
}

void applyContactLosses(Ball& ball, SweptIntersection& intersection, SweptContactDynamics& dynamics, PhysMaterial& material, const float& dt) {

	const float& radius = ball.mSphereLocal.radius;

	float rollLoss = (material.movePenalty + material.moveDrag * ball.mPhysBody.v.mag() )* dt;
	ball.mPhysBody.v.substractNoReverse(rollLoss);
	ball.mPhysBody.w.substractNoReverse(rollLoss / radius);


	Vector3 wNormal;
	
	//maxShockForceMag = ball.mass * (dynamics.relVelNMag + dynamics.relVelNMag * material.nRestCoeff) / dt;
	project(ball.mPhysBody.w, dynamics.normal, wNormal);
	ball.mPhysBody.w.subtract(wNormal);
	
	//working in radians no need to convert
	//static float lossRadPerSec = degToRad(20.0f);
	//static float lossWRatio = 0.05f;

	//circumfer = 2 Pi R -> 1 rad = r -> divide meters/sec over r
	float lossRadPerSec =  material.pureRotFrictionCoeff * material.movePenalty / radius;
	float lossWRatio =  material.pureRotFrictionCoeff * material.moveDrag / radius;

	float loss = (lossRadPerSec + (lossWRatio * wNormal.mag())) * dt;
	wNormal.substractNoReverse(loss);

	ball.mPhysBody.w.add(wNormal);
}



void balanceBallPointVelocity(Ball& ball, SweptIntersection& intersection, SweptContactDynamics& dynamics, PhysMaterial& material, const float& dt) {

	int iterCount = 7;
	float iterDt = dt / (float) iterCount;
	PhysScalar shockForceMag;
	PhysScalar shockForceMagIter;
	Vector3 r;
	Vector3 origpvelt;
	Vector3 pvelt;
	Vector3 frict;
	Vector3 diff;
	Vector3 temp;
	
	assert(dt != 0.0f);

	if (dynamics.relVelNMag > 0.0f) {

		return;
	}

	shockForceMag = ball.mPhysBody.mass * (dynamics.relVelNMag + dynamics.relVelNMag * material.nRestCoeff) / dt;
	shockForceMagIter = (shockForceMag / (float) iterCount) * iterDt * 3.0f;  //material.frictionCoeff instead of 3.0f
	

	ball.mPhysBody.getBodyPointVector(dynamics.posAtContact, intersection.contactPoint, r);
	ball.mPhysBody.getBodyPointVelocityT(r, dynamics.normal, origpvelt);
	pvelt = origpvelt;

	int i;
	for (i = 0; i < iterCount; i++) {
	
		//if (pvelt.magSquared() < 0.0008f) {
		if (pvelt.magSquared() < 0.08f) {
			//String::debug(L"fricion not generated\n");
			break;
		}

		pvelt.normalize(frict);
		frict *= shockForceMagIter;

		frict.mul(ball.mPhysBody.massInv, diff);
		ball.mPhysBody.v += diff;
		
		r.cross(frict, temp);
		ball.mPhysBody.ITensor.invMulVector(temp, diff);
		ball.mPhysBody.w += diff;

		ball.mPhysBody.getBodyPointVelocityT(r, dynamics.normal, pvelt);

		if (fabs(pvelt.dot(origpvelt)) < 0.01f) {
			static int ct = 0;
			String::debug(L"fricion limited %d\n", ct++);
			break;
		}
	}

	//if (i == iterCount) {
	//	static int ct = 0;
	//	String::debug(L"max friction %d\n", ct++);
	//}
}


void BallControllerPhysImpl::applyCollidingContact(Ball& ball, SweptIntersection& intersection, SweptContactDynamics& dynamics, const float& sweepTime, float& impulseMag) {

	float dt = sweepTime - (sweepTime * intersection.sweptFraction);
	Vector3 impulse;

	
	//String::debug(L"n %f, %f, %f\n", MExpand3(dynamics.normal));
	
	ball.mPhysBody.savePos(dynamics.posAtContact);

	/*
	PhysCollisionLawFrictionNewton::computeImpulse(mCurrContactBody, *mpBall, &mTestMaterial, dynamics.normal, intersection.contactPoint, 
													dynamics.relVel, dynamics.relVelNMag, impulse);
	PhysCollisionLawBall::applyImpulse(*mpBall, impulse, dynamics.normal, intersection.contactPoint, 0.0f, false);
	*/
	PhysCollisionLawFrictionlessNewton::computeImpulse(mCurrContactBody, ball.mPhysBody, intersection.pMaterial->physProperties, dynamics.normal, intersection.contactPoint, 
													dynamics.relVel, dynamics.relVelNMag, impulse);

	//hack as well
	/*
	if (intersection.contactHasVelocity) {

		impulse.add(intersection.contactVelocity);
	}
	*/

	impulseMag = impulse.mag();

	PhysCollisionLawFrictionlessNewton::applyImpulse(ball.mPhysBody, impulse, intersection.contactPoint, false);
	
	//if (letMax) {
		//balanceBallPointVelocity(mBall.dref(), intersection, dynamics, *intersection.pMaterial, dt);
		balanceBallPointVelocity2(ball, intersection, dynamics, intersection.pMaterial->physProperties, dt);
		applyContactLosses(ball, intersection, dynamics, intersection.pMaterial->physProperties, dt);
	//}

	ball.mPhysBody.loadSavedPos();
}


bool BallControllerPhysImpl::detectAndProcessContacts(Match& match, Ball& ball, const Time& tickLength, const Time& startTime, const TickCount& tickIndex, BallSimulation* pSimul2, const bool& executeMaterials, SimulationID& ballMoveTickID) {

	static bool adaptiveSliceSize = true;
	float startMinSliceSize;
	float minSliceSize;
	Vector3 sweptVector;
	Vector3 sweptFromForce;
	Vector3 velDiff;
	float processedTimeFraction;
	int sliceCounter;
	Time sliceEndTime;
	Time sweepTime;
	Time dt;
	Time currTime = startTime;
	float reachedFraction;
	SweptFractionType fractionType;
	ContactIndexType sliceCollidingCount;
	ContactIndexType sliceRestingCount;
	ContactIndexType frameContactCount;
	IntersectionState* pIntersectionState;
	float impulseMag;
	Vector3 contactPenetrationCorrection;
	Vector3 penetrationCorrection;
	float penetrationCorrectionMag;
	//static float penetrationCorrectionMaxLimit = 0.02f;
	//static float penetrationCorrectionMaxLimit = 0.2f;
	float restingEpsilon;

	short restingBasisIndex;
	Vector3 restingBasisTest[2];

	Vector3 testPos;

	CollEngine& collEngine = match.getCollEngine();

	//for swept vector we could use : V(t), V(t) & fext(t), V(t) & ftotal(t) [determined from last time]

	startMinSliceSize = 0.04f;
	minSliceSize = startMinSliceSize;
	frameContactCount = 0;
	sliceCounter = 0;
	sliceEndTime = 0.0f;
	processedTimeFraction = 0.0f;
	dt = 0.0f;

	bool allResting = true;
	bool lastCollIsPitchResting = false; 
	bool endedSimul = false;

	while (sliceEndTime < tickLength && !endedSimul) {

		sweepTime = tickLength - sliceEndTime;

		if (sweepTime < 0.00001f) {

			reachedFraction = 1.0f;
			break;
		}

		ball.mPhysBody.v.mul(sweepTime, sweptVector);
		ball.mPhysBody.force.mul(ball.mPhysBody.massInv * sweepTime, velDiff);
		velDiff.mul(sweepTime, sweptFromForce);
		sweptVector += sweptFromForce;
		

		//restingEpsilon = MMax(0.6f, velDiff.mag());
		restingEpsilon = MMax(13.0f, velDiff.mag());

		penetrationCorrection.zero();
		mSweptIntersections.count = 0;
		//ball.mPhysBody.pos.add(sweptVector, testPos);
		
		ball.physGetSweepMoveIntersections(sweptVector, mSweptIntersections, pSimul2, executeMaterials);
		//collEngine.testSweptContacts(ball, testPos, sweptVector, mSweptIntersections);
		//ball.detectSweptContacts(sweptVector, mSweptIntersections);

		mSweptIntersectionPtrs.count = 0;
		for (ContactIndexType i = 0; i < mSweptIntersections.count; i++) {
			
			mSweptIntersectionPtrs.addOne(&(mSweptIntersections.el[i]));
		}

		qsort(mSweptIntersectionPtrs.el, mSweptIntersectionPtrs.count, 
				sizeof(SweptIntersection*), compareSweptFractionMember<SweptIntersection>);
		
		bool reachedDelta = false;
		bool reachedSeparating;
		reachedFraction = 1.0f;

		restingBasisIndex = 0;
		sliceRestingCount = 0;
		sliceCollidingCount = 0;
		mIntersectionStates.count = 0;
		mIntersectionStates.reserve(mSweptIntersectionPtrs.count);

	
		if (mSweptIntersectionPtrs.count == 0) 
			allResting = false;

		for (ContactIndexType i = 0; i < mSweptIntersectionPtrs.count; i++) {

			pIntersectionState = &(mIntersectionStates.addOne());
			WOF::match::SweptIntersection& sweptIntersection = dref(mSweptIntersectionPtrs[i]);

			fractionType = determinFractionType(sweptIntersection.sweptFraction, minSliceSize);

			pIntersectionState->fractionType = fractionType;

			if (fractionType == SFT_Delta) {

				pIntersectionState->wasProcessed = true; //not te be taken as resting
				reachedFraction = sweptIntersection.sweptFraction;
				reachedDelta = true;

				if (adaptiveSliceSize) {
					if(sweptIntersection.sweptFraction < 2.0f * minSliceSize) {
						minSliceSize *= 2.0f;
					}
				}
			}

			//really 'active control' velocity
			if (sweptIntersection.contactHasVelocity) {

				assrt(false);
				reachedFraction = sweptIntersection.sweptFraction;
				reachedDelta = true;

				ball.mPhysBody.v.add(sweptIntersection.contactVelocity);
			}

			if (reachedDelta) {
				break;
			}

			lastCollIsPitchResting = false; 

			//ball.onPhysCollision(sweptIntersection.staticObj, sweptIntersection.objectPartID);

			switch (ball.onPrePhysCollision(sweptIntersection.staticObj, sweptIntersection.objectPartID)) {

				case Ball::ImmCollResp_Continue: break;
				case Ball::ImmCollResp_IgnoreCollAndContinute: break;
				case Ball::ImmCollResp_ExitFrameMove: break;
			}

			if (sweptIntersection.pMaterial->physProperties.isValid()) {

				determinIntersectionDynamics(sweptIntersection, ball, sweptVector, pIntersectionState->dynamics);
				pIntersectionState->dynamicType = determinContactDynamicType(pIntersectionState->dynamics.relVelNMag, restingEpsilon/*, sweptIntersection.contactHasVelocity*/);

			} else {

				pIntersectionState->dynamicType = SCDT_Trigger;
			}

			reachedSeparating = false;

			switch (pIntersectionState->dynamicType) {
				case SCDT_Resting:

					
					//stable Resting detection using momentum + restingBasis
					/*
					if (mBallStateLowDcreasingMomentum) {

						
						float dot = pIntersectionState->dynamics.normal.dot(ball.force);
						if (dot < 0.0f) {
							
							switch(restingBasisIndex) {
								case 0:
								restingBasisTest[0] = pIntersectionState->dynamics.normal;

								if ((fabs(dot + ball.force.mag()) < 0.01f) && (ball.v.mag() < 13.0f) ) {
									String::debug(L"Single Counter Force Contact Basis\n");

									ball.v.zero();
									ball.w.zero();
									mBallStateSleeping = true;
								}
								break;
							case 1:
								pIntersectionState->dynamics.normal.cross(restingBasisTest[0], restingBasisTest[1]);
								break;	
							default:
								dot = pIntersectionState->dynamics.normal.dot(restingBasisTest[1]);
								if (fabs(dot) < 0.001f) {
  									String::debug(L"Valid Contact Basis\n");

									//ball.v.zero();
									//ball.w.zero();
									//mBallStateSleeping = true;
								}
								break;
							}

							restingBasisIndex++;
						}
					}
					*/
					
					if (mSweptIntersectionPtrs.el[i]->staticObj->objectType == Node_Pitch
						|| mSweptIntersectionPtrs.el[i]->staticObj->objectType == Node_SimulPitch
						)
						lastCollIsPitchResting = true; 

					sliceRestingCount++;
					pIntersectionState->wasProcessed = false;

					pIntersectionState->dynamics.normal.mul(pIntersectionState->dynamics.penetration, contactPenetrationCorrection);
					penetrationCorrection.x = MMax(penetrationCorrection.x, contactPenetrationCorrection.x);
					penetrationCorrection.y = MMax(penetrationCorrection.y, contactPenetrationCorrection.y);
					penetrationCorrection.z = MMax(penetrationCorrection.z, contactPenetrationCorrection.z);
					
					//testing
					applyCollidingContact(ball, *mSweptIntersectionPtrs.el[i], pIntersectionState->dynamics, sweepTime, impulseMag);

					if (executeMaterials && sweptIntersection.pMaterial)
						match.executeMaterial(dref(sweptIntersection.pMaterial), sweptIntersection.contactPoint, 0.0f);

					break;
				case SCDT_Colliding: {

					#ifdef BallControllerPhysImpl_GENERATE_BALL_LOG
					if (pSimul2 == NULL) {

						String dbgStr;

						dbgStr.assignEx(L"coll! t: %f,  pos: %f, %f, %f, vel: %f, %f, %f", currTime, 
											ball.mPhysBody.pos.x, ball.mPhysBody.pos.y, ball.mPhysBody.pos.z,
											ball.mPhysBody.v.x, ball.mPhysBody.v.y, ball.mPhysBody.v.z);

						log(dbgStr.c_tstr());
					}
					#endif

					allResting = false;

					if (pSimul2 && !pSimul2->signalPreCollision(sweptIntersection.staticObj, currTime, ball)) {
							
						endedSimul = true;
						break;
					}

					//we might also check if colliding 'might become' resting and mark accordingly
					//if (sweptContactDynamics.relVelNMag * mTestMaterial.

					applyCollidingContact(ball, *mSweptIntersectionPtrs.el[i], pIntersectionState->dynamics, sweepTime, impulseMag);
					pIntersectionState->wasProcessed = true;
					sliceCollidingCount++;

					if (executeMaterials && sweptIntersection.pMaterial)
						match.executeMaterial(dref(sweptIntersection.pMaterial), sweptIntersection.contactPoint, impulseMag);

					if (pSimul2 && !pSimul2->signalPostCollision(sweptIntersection.staticObj, currTime, ball)) {
							
						endedSimul = true;
						break;
					}

				}	break;

				case SCDT_Trigger: {

					if (pSimul2 && !pSimul2->signalPreTrigger(sweptIntersection.pMaterial, currTime, ball, ballMoveTickID)) {
							
						endedSimul = true;
						break;
					}

					pIntersectionState->wasProcessed = true;
				
					if (executeMaterials && sweptIntersection.pMaterial)
						match.executeMaterial(dref(sweptIntersection.pMaterial), sweptIntersection.contactPoint, impulseMag);

					if (pSimul2 && !pSimul2->signalPostTrigger(sweptIntersection.pMaterial, currTime, ball)) {
							
						endedSimul = true;
						break;
					}

				}	break;

				case SCDT_Separating:

					allResting = false;

					//String::debug(L"separ.\n");
					pIntersectionState->wasProcessed = true;
					if (fractionType == SFT_ZeroEpsilon) {
						reachedSeparating = true;
						//String::debug(L"separ.\n");
					}
					break;
			}

			if (pIntersectionState->dynamicType != SCDT_Separating) {

				switch (ball.onPostPhysCollision(sweptIntersection.staticObj, sweptIntersection.objectPartID)) {

					case Ball::ImmCollResp_Continue: break;
					case Ball::ImmCollResp_ExitFrameMove: break;
				}
			}

			//if (reachedSeparating) {
			//	break;
			//}
		}

		frameContactCount += (sliceCollidingCount + sliceRestingCount);

		if ((reachedFraction != 0.0f) && (reachedFraction < 1.0f)) {

			dt = (reachedFraction * sweepTime);
			if (dt == 0.0f) {

				assrt(false);
			}

			sliceEndTime += dt;

		} else {

			dt = sweepTime;
			sliceEndTime = tickLength;
		}

		currTime += dt;
		
		/*
		//resting contact for now the samne impulses for collisions are used
		if (sliceRestingCount) {

			mContactGeometry.begin(ball.pos, ball.mCollSphere.radius, sweptVector);

			for (ContactIndexType i = 0; i < mIntersectionStates.count; i++) {
				
				pIntersectionState = &(mIntersectionStates.el[i]);
				
				assert(pIntersectionState->fractionType != SFT_Delta);

				//for now we blindly treat all unprocessed as resting, can be improved by retesting...
				//only retest when collisions have occured!
				if (pIntersectionState->wasProcessed == false) {
					
					//treat tZero and tEpsilon differently?
					mContactGeometry.addCandidate(mSweptIntersectionPtrs.el[i]);
				}
			}

			mRestingIntersections.count = 0;
			mContactGeometry.getContacts(mRestingIntersections);
			mContactGeometry.end();

			//get resulting resting force

			if (mContactGeometry.geometryChanged() 
				//|| (sliceCollidingCount != 0)
				) {
			
				for (ContactIndexType i = 0; i < mRestingIntersections.count; i++) {
					
					
					//determinIntersectionDynamics(*mRestingIntersections.el[i], *mpBall, sweptVector, sweptContactDynamics);
					//sweptContactDynamicType = determinContactDynamicType(sweptContactDynamics.relVelNMag, 0.0f);

					//switch (sweptContactDynamicType) {
					//	case SCDT_Resting:
					//		break;
					//}
					

				}

			} else {
			}
		}
		*/

		ball.mPhysBody.force.mul(ball.mPhysBody.massInv * sweepTime, velDiff);
		ball.mPhysBody.v += velDiff;
		PhysBodyGroup::doPositionUpdate(ball.mPhysBody, dt);
		ball.physMarkDesynced();
		++ballMoveTickID;

		if (ball.hasAfterTouchOwner())
			ball.accessAfterTouchVel().mulAndAdd(dt, ball.accessAfterTouchPosDiff());

		//constraint penalties
		penetrationCorrectionMag = penetrationCorrection.mag();
		if (penetrationCorrectionMag > penetrationCorrectionMaxLimit) {
			
			penetrationCorrection *= (penetrationCorrectionMaxLimit / penetrationCorrectionMag);
		} 
		ball.mPhysBody.pos += penetrationCorrection;

		//or not?!?!?
		//syncBallNodeToPhys();
		
		sliceCounter++;
	}

	checkAndSetResting(ball, allResting);
	ball.physSetIsOnPitch(lastCollIsPitchResting);

	if (pSimul2) {

		if (!pSimul2->signalSimulTickEnd(currTime, tickIndex, ball))
			return false;
	}

	if (frameContactCount > 0) {

		//ball.cancelAfterTouch();

		/*
		Vector3 diff;

		ball.v.mul(0.3f * tickLength, diff);
		ball.v.subtract(diff);
		
		ball.w.mul(0.3f * tickLength, diff);
		ball.w.subtract(diff);
		*/

		//ct roll loss
		/*
		done in applyContactLosses
		static float lossMetPerSec = 35.0f; //CONV!!!
		static float lossVRatio = 0.08f;
		
		float rollLoss = (lossMetPerSec + lossVRatio * ball.v.mag() )* tickLength;
		ball.v.substractNoReverse(rollLoss);
		ball.w.substractNoReverse(rollLoss / ball.mCollSphere.radius);
		*/


		/*
		//stable resting contact momentum detection
		//not very good improve by tracking and detecting
		//low v and low angular swinging
		static PhysScalar lastMomentum = PhysInf;
		PhysScalar newMomentum;

		if (mBallStateLowDcreasingMomentum == false) {

			float vmag = ball.v.mag();
			float wmag = ball.w.mag();

			newMomentum = vmag + wmag;

			if (lastMomentum == PhysInf) {

				lastMomentum = newMomentum;

			} else {


				if (newMomentum <= lastMomentum && newMomentum < restingEpsilon) {

					mBallStateLowDcreasingMomentum = true;
					String::debug(L"low decreasing momentum = %f\n", newMomentum);

					//ball.v.zero();
					//ball.w.zero();
					//mBallStateSleeping = true;
				}
			}
		}
		*/

	}
	
	//if (sliceCounter > 5) {
	//	String::debug(L"%d slices\n", sliceCounter);
	//}

	return true;
}

void BallControllerPhysImpl::applyEnvEffects(Ball& ball, bool applyDrag, bool applyMagnus, const Time& dt) {

	if (applyMagnus) {

		/*
		Vector accelDiff;

		ball.mPhysEnv.calcMagnusEffect(ball.mPhysBody.v, ball.mPhysBody.w, accelDiff);

		now we need to apply this, but applying it is messy ... so disable for now...
			also notice isnt there sthg wierd in PhysForce.apply ???
		*/
	}

	if (applyDrag) {

		Vector vDiff;

		ball.mPhysEnv.calcDragEffect(ball.mPhysBody.v, dt, vDiff);

		if (vDiff.magSquared() > ball.mPhysBody.v.magSquared()) {

			ball.mPhysBody.v.zero();

			if (ball.hasAfterTouchOwner()) {

				ball.accessAfterTouchVel().zero();
			}

		} else {

			if (ball.hasAfterTouchOwner()) {

				Vector3 newV;

				ball.mPhysBody.v.add(vDiff, newV);

				ball.accessAfterTouchVel().mul(newV.mag() / ball.mPhysBody.v.mag());

				ball.mPhysBody.v = newV;

			} else {

				ball.mPhysBody.v.add(vDiff);
			}
		}
	}
}

void BallControllerPhysImpl::frameMove(Match& match, Ball& ball, const TickCount& tickCount, const Time& tickLength, const Time& startTime, const TickCount& startFrameIndex, BallSimulation* pSimul2, const bool& executeMaterials, Time& outEndTime, TickCount& outEndTick) {

	TickCount currFrame = startFrameIndex; 
	Time currTime = startTime;
	bool ended = false;

	SimulationID ballMoveTickID = 0;

	for (unsigned int tick = 0; tick < tickCount && !ended; ++tick) {
		
		if (true /*iterCheckContacts*/) {

			#ifdef BallControllerPhysImpl_GENERATE_BALL_LOG
			if (pSimul2 == NULL) {

				String dbgStr;

				dbgStr.assignEx(L"t: %f,  pos: %f, %f, %f, vel: %f, %f, %f", currTime, 
									ball.mPhysBody.pos.x, ball.mPhysBody.pos.y, ball.mPhysBody.pos.z,
									ball.mPhysBody.v.x, ball.mPhysBody.v.y, ball.mPhysBody.v.z);

				log(dbgStr.c_tstr());
			}
			#endif

			applyEnvEffects(ball, match.mTweakEnableAirDrag, false, tickLength);
			
			if (!detectAndProcessContacts(match, ball, tickLength, currTime, currFrame, pSimul2, executeMaterials, ballMoveTickID))
				ended = true;
			
		} else {

			/*
			Vector3 velDiff;

			applyEnvEffects(match.mTweakEnableAirDrag, false, ballPhysTime.tickLength);
			
			ball.mPhysBody.force.mul(ball.mPhysBody.massInv * ballPhysTime.tickLength, velDiff);
			ball.mPhysBody.v += velDiff;
			mPhysGroup.doPositionUpdate(ball.mPhysBody, ballPhysTime.tickLength);
			ball.physMarkDesynced();
			*/

			/*
			applyEnvEffects(match.mTweakEnableAirDrag, match.mTweakEnableAirMagnus, ballPhysTime.tickLength);

			//ball.setPosAsLast();
			//mBallState.simpleStep(ballPhysTime.tickLength);
			mPhysGroup.doPositionUpdate(ball.mPhysBody, ballPhysTime.tickLength);
			ball.physMarkDesynced();
			//mBallState.recomputeDistance();
			*/
		}
		
		currTime += tickLength;
		++currFrame;
	}

	outEndTime = currTime;
	outEndTick = currFrame;
}

void BallControllerPhysImpl::frameMove(Match& match, const Clock& time, Time& outEndTime) {

	TickCount endTick;
	
	frameMove(match, mBall, time.mBallPhysTime.frameTicks, time.mBallPhysTime.tickLength, time.getPhysTime(), time.mBallPhysTime.frames, NULL, true, outEndTime, endTick);

	mBall->physSyncNodeFromPhysBody();
}

void BallControllerPhysImpl::simulate(Match& match, BallSimulation* pSimul2, Ball& ball, const Time& startTime, const TickCount& startTick, const Time& duration, const Time& tickLength, Time& outEndTime, TickCount& outEndTick) {

	ball.mPhysBody.force.zero();
	mGravity.apply(ball.mPhysBody);

	ball.physSyncNodeFromPhysBody();

	TickCount outTickCount = (TickCount) (duration / tickLength);

	frameMove(match, ball, outTickCount, tickLength, startTime, startTick, pSimul2, false, outEndTime, outEndTick);

	ball.physSyncNodeFromPhysBody();
}

void BallControllerPhysImpl::debugReset() {
}

void BallControllerPhysImpl::debugRender(Renderer& renderer, bool validOnly) {
}


} }