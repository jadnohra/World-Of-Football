#include "../Match.h"

#include "BallShotEstimator.h"

DECLARE_INSTANCE_TYPE_NAME(WOF::match::BallShotEstimator, CBallShotEstimator);
DECLARE_INSTANCE_TYPE_NAME(WOF::match::BallShotEstimator::EstimResult, CBallShotEstimatorResult);

namespace WOF { namespace match {

const TCHAR* BallShotEstimator::ScriptClassName = L"CBallShotEstimator";
const TCHAR* BallShotEstimator::EstimResult::ScriptClassName = L"CBallShotEstimatorResult";

void BallShotEstimator::declareInVM(SquirrelVM& target) {

	BallShotEstimator::EstimResult::declareInVM(target);

	SqPlus::SQClassDef<BallShotEstimator>(BallShotEstimator::ScriptClassName).
		func(&BallShotEstimator::script_addRecord, L"addRecord").
		func(&BallShotEstimator::script_estimateNeededShotSpeedByTime, L"estimateNeededShotSpeedByTime").
		func(&BallShotEstimator::script_estimateNeededShotSpeedBySpeed, L"estimateNeededShotSpeedBySpeed");

}

void BallShotEstimator::EstimResult::declareInVM(SquirrelVM& target) {

	SqPlus::SQClassDef<EstimResult>(EstimResult::ScriptClassName).
		var(&EstimResult::isConstraintSatisfied, L"isConstraintSatisfied").
		var(&EstimResult::minWaitTime, L"minWaitTime").
		var(&EstimResult::speed, L"speed");
}

BallShotEstimator::Group::Record::Record() : speed(-1.0f), maxFwdDist(-1.0f), maxT(-1.0f) {
}

void BallShotEstimator::Group::Record::onSimulUpdated() {

	speed = simulation->getCommand().vel.v.mag();

	const BallSimulation::TrajSample* pLastSample = simulation->getLastSample();
	if (pLastSample) {

		maxFwdDist = pLastSample->pos.el[Scene_Forward];
		maxT = pLastSample->time;
	}
}

BallShotEstimator::Group::Group(const TCHAR* name) : mName(name) {

	mNameHash = mName.hash();
}

BallShotEstimator::BallShotEstimator() {

	assrt(false);
}

BallShotEstimator::BallShotEstimator(Match& match) : mMatch(&match), mStartPosSet(false) {

	BallSimulation ballSimul;

	ballSimul.enableSimul(true);
	ballSimul.setupUsingMatch(match);

	mSimulData = ballSimul.cloneSimulData();
}

const Vector3& BallShotEstimator::getStartPos() {

	if (mStartPosSet)
		return mStartPos;

	mStartPosSet = true;

	Ball& ball = mMatch->getSimulBall();
	mStartPos.zero();
	mStartPos.el[Scene_Up] = mMatch->getPitchInfo().height;
	mStartPos.el[Scene_Up] += ball.getRadius() + (0.05f * ball.getRadius());

	return mStartPos;
}

void BallShotEstimator::load(Match& match, BufferString& tempStr, DataChunk& mainChunk, CoordSysConv* pConv) {

	SoftRef<DataChunk> chunk = mainChunk.getChild(L"ballSimulSettings");

	if (chunk.isValid()) {

		BallSimulation ballSimul;

		ballSimul.enableSimul(true);
		ballSimul.setupUsingMatch(match);
		ballSimul.loadConfig(mMatch, tempStr, chunk, pConv);

		mSimulData = ballSimul.cloneSimulData();
	}
}

/*
bool BallShotEstimator::simulateApogee(const Vector3& velocity, float& outApogeeHeight, Time& outApogeeTime) {
	
	SoftPtr<BallSimulation> simulation = &mSimulPitchSimulation;

	Ball& ball = mMatch->getSimulBall();
	SoftPtr<BallControllerPhysImpl> physController = ball.getControllerPhys();
	physController->attach(mMatch, &ball);

	{
		//make ball simulable

		ball.setIsInPlay(true);
		ball.getCommandManager().clearCommands();

		physController->setPosition(getStartPos(), NULL);
		physController->setVelocities(Vector3::kZero, Vector3::kZero, NULL);
		ball.frameMove(mMatch->getClock()); 

		assrt(ball.isSimulable());
	}

	physController->setPosition(getStartPos(), NULL);
	physController->setVelocities(velocity, Vector3::kZero, NULL);

	simulation->enableSimul(true);
	SoftPtr<BallSimulation::SimulData> simulData = simulation->getSimulData();
	simulData->mRecordSamples = false;
	simulData->mEndConditionType = BallSimulation::EndCondition_Apogee;
	simulation->resetSimul();
	
	BallSimulation::SimulState simulState = BallSimulation::Simul_Simulating;
	Time simulStartTime = mMatch->getClock().getTime();

	while (simulState != BallSimulation::Simul_Simulated) {

		bool doExit = false;

		switch (simulState) {

			case BallSimulation::Simul_None: {

				assrt(false);
				doExit = true;

			} break;
		}

		if (doExit)
			break;

		simulState = simulation->updateSimulate(ball);
	}

	if (simulState == BallSimulation::Simul_Simulated) {
		
		outApogeeHeight = simulData->mApogeeHeight;
		outApogeeTime = simulData->mApogeeTime - simulStartTime;
		
		return true;
	} 

	return false;
}

bool BallShotEstimator::simulatePitchColl(const float& height, BallSimulation::CollSample& outColl) {
	
	SoftPtr<BallSimulation> simulation = &mSimulPitchSimulation;

	Ball& ball = mMatch->getSimulBall();
	SoftPtr<BallControllerPhysImpl> physController = ball.getControllerPhys();
	physController->attach(mMatch, &ball);

	{
		//make ball simulable

		ball.setIsInPlay(true);
		ball.getCommandManager().clearCommands();

		physController->setPosition(getStartPos(), NULL);
		physController->setVelocities(Vector3::kZero, Vector3::kZero, NULL);
		ball.frameMove(mMatch->getClock()); 

		assrt(ball.isSimulable());
	}

	Vector3 startPos = getStartPos();
	startPos.el[Scene_Up] = height;

	physController->setPosition(startPos, NULL);
	

	simulation->enableSimul(true);
	SoftPtr<BallSimulation::SimulData> simulData = simulation->getSimulData();
	simulData->mRecordSamples = false;
	simulData->mEndConditionType = BallSimulation::EndCondition_Coll;
	simulation->resetSimul();
	
	BallSimulation::SimulState simulState = BallSimulation::Simul_Simulating;
	Time simulStartTime = mMatch->getClock().getTime();

	while (simulState != BallSimulation::Simul_Simulated) {

		bool doExit = false;

		switch (simulState) {

			case BallSimulation::Simul_None: {

				assrt(false);
				doExit = true;

			} break;
		}

		if (doExit)
			break;

		simulState = simulation->updateSimulate(ball);
	}

	if ((simulState == BallSimulation::Simul_Simulated) && (simulation->getCollSampleCount() > 0)) {
		
		outColl = simulation->getCollSample(0);
		
		return true;
	} 

	return false;
}
*/

bool BallShotEstimator::simulateSingleVerticalBounce(const Vector3& startVelocity, BallSimulation& inOutSimul, bool recordSamples) {
	
	SoftPtr<BallSimulation> simulation = &inOutSimul;

	Ball& ball = mMatch->getSimulBall();
	SoftPtr<BallControllerPhysImpl> physController = ball.getControllerPhys();
	physController->attach(mMatch, &ball);

	{
		//make ball simulable

		ball.setIsInPlay(true);
		ball.getCommandManager().clearCommands();

		physController->setPosition(getStartPos(), NULL);
		physController->setVelocities(Vector3::kZero, Vector3::kZero, NULL);
		ball.frameMove(mMatch->getClock()); 

		assrt(ball.isSimulable());
	}

	physController->setPosition(getStartPos(), NULL);
	physController->setVelocities(startVelocity, Vector3::kZero, NULL);
	

	simulation->enableSimul(true);
	SoftPtr<BallSimulation::SimulData> simulData = simulation->getSimulData();
	simulData->mRecordSamples = recordSamples;
	simulData->mEndConditionType = BallSimulation::EndCondition_Coll;
	simulation->resetSimul();
	
	BallSimulation::SimulState simulState = BallSimulation::Simul_Simulating;

	while (simulState != BallSimulation::Simul_Simulated) {

		bool doExit = false;

		switch (simulState) {

			case BallSimulation::Simul_None: {

				assrt(false);
				doExit = true;

			} break;
		}

		if (doExit)
			break;

		simulState = simulation->updateSimulate(ball);
	}

	if ((simulState == BallSimulation::Simul_Simulated) && (simulation->getCollSampleCount() > 0)) {
		
		return true;
	} 

	return false;
}

bool BallShotEstimator::addRecord(const TCHAR* groupName, BallCommand& command, bool autoCreateGroup) {

	StringHash hash = String::hash(groupName);
	SoftPtr<Group> group;

	if (!mGroupMap.find(hash, group.ptrRef()) && autoCreateGroup) {

		WE_MMemNew(group.ptrRef(), Group(groupName));
		mGroupMap.insert(hash, group);
	}

	if (group.isValid()) {

		SoftPtr<Group::Record> record;
		
		WE_MMemNew(record.ptrRef(), Group::Record());
		WE_MMemNew(record->simulation.ptrRef(), BallSimulation_SimulPitch());


		Ball& ball = mMatch->getSimulBall();
		SoftPtr<BallControllerPhysImpl> physController = ball.getControllerPhys();
		physController->attach(mMatch, &ball);

		{
			//make ball simulable

			ball.setIsInPlay(true);
			ball.getCommandManager().clearCommands();

			physController->setPosition(getStartPos(), NULL);
			physController->setVelocities(Vector3::kZero, Vector3::kZero, NULL);
			ball.frameMove(mMatch->getClock()); 

			assrt(ball.isSimulable());
		}

		physController->setPosition(getStartPos(), NULL);

		assrt(command.type == BallCommand_Vel);
		physController->executeCommand(command);

		record->simulation->setSimulData(mSimulData);
		record->simulation->resetSimul();
		
		BallSimulation::SimulState simulState = BallSimulation::Simul_Simulating;

		while (simulState != BallSimulation::Simul_Simulated) {

			bool doExit = false;

			switch (simulState) {

				case BallSimulation::Simul_None: {

					assrt(false);
					doExit = true;

				} break;
			}

			if (doExit)
				break;

			simulState = record->simulation->updateSimulate(ball);
		}

		if (simulState == BallSimulation::Simul_Simulated) {
			
			record->simulation->setCommand(&command);
			record->onSimulUpdated();
			group->addRecord(record);

			return true;

		} else {

			HardPtr<Group::Record> recordDtor = record;

			return false;
		}

	} else {

		return false;
	}
}

bool BallShotEstimator::estimateNeededShotSpeedByTime(const TCHAR* groupName, CtVector3& dir, CtVector3& from, CtVector3& to, const float& posEpsilon, float t, const float& tEpsilon, EstimResult& result) {

	StringHash hash = String::hash(groupName);
	SoftPtr<Group> group;

	if (mGroupMap.find(hash, group.ptrRef())) {

		return group->estimateNeededShotSpeedByTime(dref(this), dir, from, to, posEpsilon, t, tEpsilon, result);
	}

	return false;
}

bool BallShotEstimator::estimateNeededShotSpeedBySpeed(const TCHAR* groupName, CtVector3& dir, CtVector3& from, CtVector3& to, const float& posEpsilon, float speed, const float& speedEpsilon, EstimResult& result) {

	StringHash hash = String::hash(groupName);
	SoftPtr<Group> group;

	if (mGroupMap.find(hash, group.ptrRef())) {

		return group->estimateNeededShotSpeedBySpeed(dref(this), dir, from, to, posEpsilon, speed, speedEpsilon, result);
	}

	return false;
}

bool BallShotEstimator::Group::toLocalPos(BallShotEstimator& estimator, CtVector3& dir, CtVector3& from, CtVector3& to, Vector3& outLocalTargetPos) {

	if (dir.el[Scene_Up] != 0.0f || dir.isZero())
		return false;

	SceneTransform transf;

	transf.setOrientation(dir, estimator.mMatch->getCoordAxis(Scene_Up));
	//estimator.getStartPos().subtract(from, transf.position());
	from.subtract(estimator.getStartPos(), transf.position());
	
	transf.invTransformPoint(to, outLocalTargetPos);
	outLocalTargetPos.el[Scene_Up] = to.el[Scene_Up];

	return true;
}

bool BallShotEstimator::Group::estimateNeededShotSpeedByTime(BallShotEstimator& estimator, CtVector3& dir, CtVector3& from, CtVector3& to, const float& posEpsilon, float t, const float& tEpsilon, EstimResult& result) {

	Vector3 localTargetPos;
	
	if (toLocalPos(estimator, dir, from, to, localTargetPos)) {

		return estimateNeededShotSpeedLocalByTime(estimator, from.el[Scene_Up] - estimator.getStartPos().el[Scene_Up], localTargetPos, posEpsilon, t, tEpsilon, result);
	}

	return false;
}

bool BallShotEstimator::Group::estimateNeededShotSpeedBySpeed(BallShotEstimator& estimator, CtVector3& dir, CtVector3& from, CtVector3& to, const float& posEpsilon, float speed, const float& speedEpsilon, EstimResult& result) {

	Vector3 localTargetPos;
	
	if (toLocalPos(estimator, dir, from, to, localTargetPos)) {

		return estimateNeededShotSpeedLocalBySpeed(estimator, from.el[Scene_Up] - estimator.getStartPos().el[Scene_Up], localTargetPos, posEpsilon, speed, speedEpsilon, result);
	}

	return false;
}

/*
v1
bool BallShotEstimator::Group::interpolateByTime(BallShotEstimator& estimator, const float& startHeightDiff, const Vector3& targetPos, const float& t, 
											Record& record, BallSimulation::TrajSample& trajSample, Record* pLastRecord, BallSimulation::TrajSample* pLastTrajSample, float& outSpeed) {

	float compDiff = targetPos.el[Scene_Forward] - trajSample.pos.el[Scene_Forward];

	if (pLastRecord) {

		float lastCompDiff = targetPos.el[Scene_Forward] - pLastTrajSample->pos.el[Scene_Forward];

		if (lastCompDiff > 0.0f) {

			float linearAdjust = (record.getMaxFwdDist() / pLastRecord->getMaxFwdDist()) / ((record.getSpeed() / pLastRecord->getSpeed()));
			float factor = -compDiff / (trajSample.pos.el[Scene_Forward] - pLastTrajSample->pos.el[Scene_Forward]);

			//factor = tmin(linearAdjust * factor, 1.0f);

			outSpeed = lerp(pLastRecord->getSpeed(), record.getSpeed(), factor);
			return outSpeed > 0.0f;

		} else {

			//last record was already ahead of target pos

			float distDiffPerUnitSpeed = (trajSample.pos.el[Scene_Forward] - pLastTrajSample->pos.el[Scene_Forward]) / (record.getSpeed() - pLastRecord->getSpeed());
			float linearAdjust = (record.getMaxFwdDist() / pLastRecord->getMaxFwdDist()) / ((record.getSpeed() / pLastRecord->getSpeed()));
			float speedDiffPerUnitDist = 1.0f / (linearAdjust * distDiffPerUnitSpeed);
			
			outSpeed = tmax(pLastRecord->getSpeed() + (lastCompDiff * speedDiffPerUnitDist), 0.0f);
			return outSpeed > 0.0f;
		}

	} else {

		float factor = -compDiff / (trajSample.pos.el[Scene_Forward] - 0.0f);

		outSpeed = lerp(0.0f, record.getSpeed(), factor);
		return outSpeed > 0.0f;
	}

	return false;
}
*/

/*
v2
bool BallShotEstimator::Group::interpolateByTime(BallShotEstimator& estimator, const float& startHeightDiff, const Vector3& targetPos, const float& t, 
											Record& record, BallSimulation::TrajSample& trajSample, Record* pLastRecord, BallSimulation::TrajSample* pLastTrajSample, float& outSpeed) {

	float compDiff = trajSample.time - t;

	if (pLastRecord) {

		float lastCompDiff = pLastTrajSample->time - t;

		if (lastCompDiff > 0.0f) {

			float linearAdjust = (record.getMaxFwdDist() / pLastRecord->getMaxFwdDist()) / ((record.getSpeed() / pLastRecord->getSpeed()));
			float factor = -compDiff / (trajSample.time - pLastTrajSample->time);

			//factor = tmin(linearAdjust * factor, 1.0f);

			outSpeed = lerp(pLastRecord->getSpeed(), record.getSpeed(), factor);
			return outSpeed > 0.0f;

		} else {

			//last record was already ahead of target pos
			float linearAdjust = (record.getMaxFwdDist() / pLastRecord->getMaxFwdDist()) / ((record.getSpeed() / pLastRecord->getSpeed()));

			float slope = (record.getSpeed() - pLastRecord->getSpeed()) / (trajSample.time - pLastTrajSample->time);
			slope *= linearAdjust;

			outSpeed = record.getSpeed() + slope * (t - trajSample.time);

			outSpeed = tmax(outSpeed, 0.0f);

			ok improve by using double linear adjusts from this and also sampel b4 last sample
				to scale the y's using linear adjusts
				after this reflect these improvements in all return paths and also in BySpeed fucntion

			return outSpeed > 0.0f;
		}

	} else {

		float factor = -compDiff / (trajSample.time - 0.0f);

		outSpeed = lerp(0.0f, record.getSpeed(), factor);
		return outSpeed > 0.0f;
	}

	return false;
}
*/

bool BallShotEstimator::Group::interpolateByTime(BallShotEstimator& estimator, const float& startHeightDiff, const Vector3& targetPos, const float& t, 
											Record& record, BallSimulation::TrajSample& trajSample, Record* pLastRecord, BallSimulation::TrajSample* pLastTrajSample, bool lastTrajSampleIsLastFallback, float& outSpeed) {

	float compDiff = trajSample.time - t;

	if (pLastRecord) {

		float lastCompDiff = pLastTrajSample->time - t;

		if (lastTrajSampleIsLastFallback) {

			float linearAdjust = (record.getMaxFwdDist() / pLastRecord->getMaxFwdDist()) / ((record.getSpeed() / pLastRecord->getSpeed()));

			float virtualLastSampleTime = pLastTrajSample->time + (trajSample.pos.el[Scene_Forward] - pLastTrajSample->pos.el[Scene_Forward]) / (0.5f * (record.getMaxFwdDist() + pLastRecord->getMaxFwdDist()));

			float slope = (record.getSpeed() - pLastRecord->getSpeed()) / (trajSample.time - virtualLastSampleTime);
			slope /= linearAdjust;

			outSpeed = record.getSpeed() + slope * (t - trajSample.time);

			return outSpeed > 0.0f;

		} else if (lastCompDiff > 0.0f) {

			float linearAdjust = (record.getMaxFwdDist() / pLastRecord->getMaxFwdDist()) / ((record.getSpeed() / pLastRecord->getSpeed()));

			float slope = (record.getSpeed() - pLastRecord->getSpeed()) / (trajSample.time - pLastTrajSample->time);
			slope /= linearAdjust;

			outSpeed = record.getSpeed() + slope * (t - trajSample.time);

			return outSpeed > 0.0f;

		} else {

			assrt(false);
			return false;
		}
	} 

	return false;
}


bool BallShotEstimator::Group::interpolateBySpeedSq(BallShotEstimator& estimator, const float& startHeightDiff, const Vector3& targetPos, const float& speedSq, 
													Record& record, BallSimulation::TrajSample& trajSample, Record* pLastRecord, BallSimulation::TrajSample* pLastTrajSample, bool lastTrajSampleIsLastFallback, float& outSpeed) {

	float velSq = trajSample.vel.magSquared();
	float compDiffSq = speedSq - velSq;

	if (pLastRecord) {

		float lastVelSq = pLastTrajSample->vel.magSquared();
		float lastCompDiffSq = speedSq - lastVelSq;

		if (lastTrajSampleIsLastFallback) {

			float vel = sqrtf(velSq);
			float speed = sqrtf(speedSq);
			float lastVel = sqrtf(lastVelSq);

			float linearAdjust = (record.getMaxFwdDist() / pLastRecord->getMaxFwdDist()) / ((record.getSpeed() / pLastRecord->getSpeed()));

			outSpeed = lerp(pLastRecord->getSpeed(), record.getSpeed(), tmin(linearAdjust * speed / vel, 1.0f));

			return outSpeed > 0.0f;

		} else if (lastCompDiffSq > 0.0f) {

			float vel = sqrtf(velSq);
			float speed = sqrtf(speedSq);
			float lastVel = sqrtf(lastVelSq);

			float linearAdjust = (record.getMaxFwdDist() / pLastRecord->getMaxFwdDist()) / ((record.getSpeed() / pLastRecord->getSpeed()));

			float slope = (record.getSpeed() - pLastRecord->getSpeed()) / (vel - lastVel);
			slope *= linearAdjust;

			outSpeed = record.getSpeed() + slope * (speed - vel);

			return outSpeed > 0.0f;

		} else {

			assrt(false);
			return false;
		}
	} 

	return false;
}

/*
bool BallShotEstimator::Group::interpolateBySpeedSq(BallShotEstimator& estimator, const float& startHeightDiff, const Vector3& targetPos, const float& speedSq, 
											Record& record, BallSimulation::TrajSample& trajSample, Record* pLastRecord, BallSimulation::TrajSample* pLastTrajSample, float& outSpeed) {

	float velSq = trajSample.vel.magSquared();
	float compDiffSq = speedSq - velSq;

	if (pLastRecord) {

		float lastVelSq = pLastTrajSample->vel.magSquared();
		float lastCompDiffSq = speedSq - lastVelSq;

		if (lastCompDiffSq > 0.0f) {

			float linearAdjust = (record.getMaxFwdDist() / pLastRecord->getMaxFwdDist()) / ((record.getSpeed() / pLastRecord->getSpeed()));
			float factor = -sqrtf(compDiffSq) / (sqrtf(velSq) - sqrtf(lastVelSq));

			//factor = tmin(linearAdjust * factor, 1.0f);

			outSpeed = lerp(pLastRecord->getSpeed(), record.getSpeed(), factor);
			return outSpeed > 0.0f;

		} else {

			//last record was already ahead of target pos

			float distDiffPerUnitSpeed = (sqrtf(velSq) - sqrtf(lastVelSq)) / (record.getSpeed() - pLastRecord->getSpeed());
			float linearAdjust = (record.getMaxFwdDist() / pLastRecord->getMaxFwdDist()) / ((record.getSpeed() / pLastRecord->getSpeed()));
			float speedDiffPerUnitDist = 1.0f / (linearAdjust * distDiffPerUnitSpeed);
			
			outSpeed = tmax(pLastRecord->getSpeed() + (sqrtf(lastCompDiffSq) * speedDiffPerUnitDist), 0.0f);
			return outSpeed > 0.0f;
		}

	} else {

		float factor = -sqrtf(compDiffSq) / (sqrtf(velSq) - 0.0f);

		outSpeed = lerp(0.0f, record.getSpeed(), factor);
		return outSpeed > 0.0f;
	}

	return false;
}
*/

/*
v1
bool BallShotEstimator::Group::estimateNeededShotSpeedLocalByTime(BallShotEstimator& estimator, const float& startHeightDiff, const Vector3& targetPos, const float& t, const float& epsilon, bool& outConstraintSatisfied, float& outSpeed) {

	if (t <= 0.0f)
		return false;

	BallSimulation::TrajSample trajSamples[2];
	
	bool hasLastValidRecordIndex = false;
	int currTrajSampleIndex = 0;
	
	outConstraintSatisfied = true;

	SoftPtr<Record> lastRecord;
	SoftPtr<BallSimulation::TrajSample> lastTrajSample;
	SoftPtr<BallSimulation::TrajSample> currTrajSample = &(trajSamples[currTrajSampleIndex]);
	
	float timeEpsilon = 0.1f;

	bool isWaitingForNextRecord = false;
	float compDiff;

	SoftPtr<Record> waitingForRecordRecord;
	SoftPtr<Record> waitingForRecordLastRecord;
	SoftPtr<BallSimulation::TrajSample> waitingForRecordSample;
	SoftPtr<BallSimulation::TrajSample> waitingForRecordLastSample;

	for (Index i = 0; i < mRecords.count; ++i) {
		
		Record& record = dref(mRecords[i]);
		bool consider;

		if (record.maxT > 0.0f && record.maxT < t) {

			consider = true;
			currTrajSample.dref() = dref(record.simulation->getLastSample());

			compDiff = targetPos.el[Scene_Forward] - currTrajSample->pos.el[Scene_Forward];

			if (fabs(compDiff) <= epsilon) {

				outConstraintSatisfied = (fabs(t - currTrajSample->time) <= timeEpsilon);
				outSpeed = record.getSpeed();

				return outSpeed > 0.0f;

			} else {

				if (compDiff < 0.0f) {

					//we need a really slow shot 
					outConstraintSatisfied = false;

					return interpolateByTime(estimator, startHeightDiff, targetPos, t, record, currTrajSample, 
										lastRecord, lastTrajSample, outSpeed);
				}
			}


		} else {

			consider = record.simulation->estimateStateAtTime(t, false, currTrajSample, NULL, timeEpsilon);
		}

		if (consider) {

			compDiff = targetPos.el[Scene_Forward] - currTrajSample->pos.el[Scene_Forward];
			
			if (fabs(compDiff) <= epsilon) {

				outSpeed = record.getSpeed();
				return outSpeed > 0.0f;

			} else {

				if (compDiff < 0.0f) {

					if (lastTrajSample.isValid()) {

						return interpolateByTime(estimator, startHeightDiff, targetPos, t, record, currTrajSample, 
										lastRecord, lastTrajSample, outSpeed);

					} else {

						isWaitingForNextRecord = true;
						waitingForRecordRecord = &record;
						waitingForRecordLastRecord = lastRecord;
						waitingForRecordSample = currTrajSample;
						waitingForRecordLastSample = lastTrajSample;
					}
				} 
			}

			lastRecord = &record;
			lastTrajSample = currTrajSample;
			currTrajSampleIndex = (currTrajSampleIndex + 1) % 2;
			currTrajSample = &(trajSamples[currTrajSampleIndex]);
		}
	}

	if (isWaitingForNextRecord) {

		return interpolateByTime(estimator, startHeightDiff, targetPos, t, waitingForRecordRecord, waitingForRecordSample, 
										waitingForRecordLastRecord, waitingForRecordLastSample, outSpeed);
	}

	return false;
}
*/

/*
v2
bool BallShotEstimator::Group::estimateNeededShotSpeedLocalByTime(BallShotEstimator& estimator, const float& startHeightDiff, const Vector3& targetPos, const float& posEpsilon, const float& t, const float& timeEpsilon, bool& outConstraintSatisfied, float& outSpeed) {

	if (t <= 0.0f)
		return false;

	BallSimulation::TrajSample trajSamples[2];
	
	bool hasLastValidRecordIndex = false;
	int currTrajSampleIndex = 0;
	
	outConstraintSatisfied = true;

	SoftPtr<Record> lastRecord;
	SoftPtr<BallSimulation::TrajSample> lastTrajSample;
	SoftPtr<BallSimulation::TrajSample> currTrajSample = &(trajSamples[currTrajSampleIndex]);
	
	bool isWaitingForNextRecord = false;
	float compDiff;

	SoftPtr<Record> waitingForRecordRecord;
	SoftPtr<Record> waitingForRecordLastRecord;
	SoftPtr<BallSimulation::TrajSample> waitingForRecordSample;
	SoftPtr<BallSimulation::TrajSample> waitingForRecordLastSample;

	for (Index i = 0; i < mRecords.count; ++i) {
		
		Record& record = dref(mRecords[i]);
		bool consider;
		bool isLastSampleFallback;

		if (record.maxT > 0.0f && record.maxT < t) {

			consider = false;
			currTrajSample.dref() = dref(record.simulation->getLastSample());

			compDiff = targetPos.el[Scene_Forward] - currTrajSample->pos.el[Scene_Forward];

			if (fabs(compDiff) <= posEpsilon) {

				outConstraintSatisfied = (fabs(t - currTrajSample->time) <= timeEpsilon);
				outSpeed = record.getSpeed();

				return outSpeed > 0.0f;

			} else {

				if (compDiff < 0.0f) {

					//we need a really slow shot 
					outConstraintSatisfied = false;

					return interpolateByTime(estimator, startHeightDiff, targetPos, t, record, currTrajSample, 
										lastRecord, lastTrajSample, outSpeed);
				}
			}


		} else {

			consider = record.simulation->estimateStateAtPosDim(Scene_Forward, targetPos.el[Scene_Forward], false, currTrajSample, &isLastSampleFallback, 0.0f);
		}

		if (consider && !isLastSampleFallback) {

			compDiff = currTrajSample->time - t;
			
			if (fabs(compDiff) <= timeEpsilon) {

				outSpeed = record.getSpeed();
				return outSpeed > 0.0f;

			} else {

				if (compDiff < 0.0f) {

					if (lastTrajSample.isValid()) {

						return interpolateByTime(estimator, startHeightDiff, targetPos, t, record, currTrajSample, 
										lastRecord, lastTrajSample, outSpeed);

					} else {

						isWaitingForNextRecord = true;
						waitingForRecordRecord = &record;
						waitingForRecordLastRecord = lastRecord;
						waitingForRecordSample = currTrajSample;
						waitingForRecordLastSample = lastTrajSample;
					}
				} 
			}

			lastRecord = &record;
			lastTrajSample = currTrajSample;
			currTrajSampleIndex = (currTrajSampleIndex + 1) % 2;
			currTrajSample = &(trajSamples[currTrajSampleIndex]);
		}
	}

	if (isWaitingForNextRecord) {

		return interpolateByTime(estimator, startHeightDiff, targetPos, t, waitingForRecordRecord, waitingForRecordSample, 
										waitingForRecordLastRecord, waitingForRecordLastSample, outSpeed);
	}

	return false;
}
*/

bool BallShotEstimator::Group::estimateNeededShotSpeedLocalByTime(BallShotEstimator& estimator, const float& startHeightDiff, const Vector3& targetPos, const float& posEpsilon, const float& t, const float& timeEpsilon, EstimResult& result) {

	if (t <= 0.0f)
		return false;

	BallSimulation::TrajSample trajSamples[2];
	
	bool hasLastValidRecordIndex = false;
	int currTrajSampleIndex = 0;
	
	result.isConstraintSatisfied = true;

	SoftPtr<Record> lastRecord;
	SoftPtr<BallSimulation::TrajSample> lastTrajSample;
	SoftPtr<BallSimulation::TrajSample> currTrajSample = &(trajSamples[currTrajSampleIndex]);
	bool lastTrajSampleIsLastFallback;
	
	float compDiff;

	for (Index i = 0; i < mRecords.count; ++i) {
		
		Record& record = dref(mRecords[i]);
		bool consider;
		bool isLastSampleFallback = true;

		if (record.maxT > 0.0f && record.maxT < t) {

			consider = false;
			currTrajSample.dref() = dref(record.simulation->getLastSample());

			compDiff = targetPos.el[Scene_Forward] - currTrajSample->pos.el[Scene_Forward];

			if (fabs(compDiff) <= posEpsilon) {

				result.minWaitTime = fabs(t - currTrajSample->time);
				result.isConstraintSatisfied = (result.minWaitTime <= timeEpsilon);
				result.speed = record.getSpeed();

				return result.speed > 0.0f;

			} else {

				if (compDiff < 0.0f) {

					//we need a really slow shot 
					result.isConstraintSatisfied = false;
					result.minWaitTime = fabs(t - currTrajSample->time);

					return interpolateByTime(estimator, startHeightDiff, targetPos, t, record, currTrajSample, 
										lastRecord, lastTrajSample, lastTrajSampleIsLastFallback, result.speed);
				}
			}
		} 
		
		consider = record.simulation->estimateStateAtPosDim(Scene_Forward, targetPos.el[Scene_Forward], false, currTrajSample, &isLastSampleFallback, 0.0f);

		if (consider) {

			if (!isLastSampleFallback) {
			
				compDiff = currTrajSample->time - t;
				
				if (fabs(compDiff) <= timeEpsilon) {

					result.isConstraintSatisfied = true;
					result.speed = record.getSpeed();
					return result.speed > 0.0f;

				} else {

					if (compDiff < 0.0f) {

						result.isConstraintSatisfied = lastTrajSample.isValid() && !lastTrajSampleIsLastFallback;

						if (lastTrajSample.isValid() && lastTrajSampleIsLastFallback) {

							result.minWaitTime = t - lastTrajSample->time;

						} else {

							result.minWaitTime = 0.0f;
						}

						return interpolateByTime(estimator, startHeightDiff, targetPos, t, record, currTrajSample, 
											lastRecord, lastTrajSample, lastTrajSampleIsLastFallback, result.speed);
					} 
				}
			}

			lastTrajSampleIsLastFallback = isLastSampleFallback;
			lastRecord = &record;
			lastTrajSample = currTrajSample;
			currTrajSampleIndex = (currTrajSampleIndex + 1) % 2;
			currTrajSample = &(trajSamples[currTrajSampleIndex]);
		}
	}

	return false;
}


bool BallShotEstimator::Group::estimateNeededShotSpeedLocalBySpeed(BallShotEstimator& estimator, const float& startHeightDiff, const Vector3& targetPos, const float& posEpsilon, const float& speed, const float& speedEpsilon, EstimResult& result) {

	float speedSq = speed * speed;
	float speedEpsilonSq = speedEpsilon * speedEpsilon;

	BallSimulation::TrajSample trajSamples[2];
	
	bool hasLastValidRecordIndex = false;
	int currTrajSampleIndex = 0;
	
	result.isConstraintSatisfied = true;

	SoftPtr<Record> lastRecord;
	SoftPtr<BallSimulation::TrajSample> lastTrajSample;
	SoftPtr<BallSimulation::TrajSample> currTrajSample = &(trajSamples[currTrajSampleIndex]);
	bool lastTrajSampleIsLastFallback;
	
	float compDiff;

	for (Index i = 0; i < mRecords.count; ++i) {
		
		Record& record = dref(mRecords[i]);
		bool consider;
		bool isLastSampleFallback;

		if (i == 0 && speedSq <= record.getSpeed() * record.getSpeed()) {

			consider = false;

			compDiff = targetPos.el[Scene_Forward] - 0.0f;

			if (fabs(compDiff) <= posEpsilon) {

				result.isConstraintSatisfied = (fabs(speedSq - 0.0f) <= speedEpsilonSq);
				result.minWaitTime = 0.0f;
				result.speed = record.getSpeed();

				return result.speed > 0.0f;
			}
		}

		consider = record.simulation->estimateStateAtPosDim(Scene_Forward, targetPos.el[Scene_Forward], false, currTrajSample, &isLastSampleFallback, 0.0f);

		if (consider) {

			if (!isLastSampleFallback) {

				compDiff = speedSq - currTrajSample->vel.magSquared();
				
				if (fabs(compDiff) <= speedEpsilonSq) {

					result.isConstraintSatisfied = true;
					result.speed = record.getSpeed();
					return result.speed > 0.0f;

				} else {

					if (compDiff < 0.0f) {

						result.isConstraintSatisfied = true;
						result.minWaitTime = 0.0f;

						return interpolateBySpeedSq(estimator, startHeightDiff, targetPos, speedSq, record, currTrajSample, 
											lastRecord, lastTrajSample, lastTrajSampleIsLastFallback, result.speed);
					} 
				}
			}

			lastRecord = &record;
			lastTrajSampleIsLastFallback = isLastSampleFallback;
			lastTrajSample = currTrajSample;
			currTrajSampleIndex = (currTrajSampleIndex + 1) % 2;
			currTrajSample = &(trajSamples[currTrajSampleIndex]);
		}
	}

	return false;
}


void BallShotEstimator::render(Renderer& renderer, 
							 const float* pBallRadiusStart, const float* pBallRadiusEnd, 
							 const RenderColor* pLineStart, const RenderColor* pLineEnd, 
							 const RenderColor* pSphereStart, const RenderColor* pSphereEnd,
							 const RenderColor* pSphereColl) {

	if (mMatch.isValid()) {

		float ballRadiusStart = pBallRadiusStart ? *pBallRadiusStart : mMatch->getBall().getRadius() * 0.15f;
		float ballRadiusEnd = pBallRadiusEnd ? *pBallRadiusEnd : mMatch->getBall().getRadius() * 0.95f;

		GroupMap::Iterator iter = mGroupMap.iterator();
		SoftPtr<Group> group;

		while (mGroupMap.next(iter, group.ptrRef())) {

			for (Index i = 0; i < group->getRecordCount(); ++i) {

				Group::Record& record = group->getRecord(i);

				record.simulation->render(renderer, false, &ballRadiusStart, &ballRadiusEnd, 
											pLineStart, pLineEnd, pSphereStart, pSphereEnd,
											pSphereColl);
			}
		}
	}
}

} }