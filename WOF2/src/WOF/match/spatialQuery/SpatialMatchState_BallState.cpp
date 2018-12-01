#include "SpatialMatchState.h"

#include "../entity/footballer/Footballer.h"
#include "../entity/ball/Ball.h"
#include "../Match.h"
#include "../Triggers.h"
#include "SpatialPitchModel.h"

DECLARE_INSTANCE_TYPE_NAME(WOF::match::SpatialMatchState::BallState, CSpatialMatch_BallState);
DECLARE_INSTANCE_TYPE_NAME(WOF::match::SpatialMatchState::BallState::BallGoalEstimate, CSpatialMatch_BallStateGoalEstimate);

namespace WOF { namespace match {

const TCHAR* SpatialMatchState::BallState::ScriptClassName = L"CSpatialMatch_BallState";
const TCHAR* SpatialMatchState::BallState::BallGoalEstimate::ScriptClassName = L"CSpatialMatch_BallStateGoalEstimate";

void SpatialMatchState::BallState::declareInVM(SquirrelVM& target) {

	SqPlus::SQClassDef<BallState>(ScriptClassName).
		enumInt(FinalBall_None, L"FinalBall_None").
		enumInt(FinalBall_InPlay, L"FinalBall_InPlay").
		enumInt(FinalBall_ThrowIn, L"FinalBall_ThrowIn").
		enumInt(FinalBall_Corner, L"FinalBall_Corner").
		enumInt(FinalBall_Goalie, L"FinalBall_Goalie").
		enumInt(FinalBall_Goal, L"FinalBall_Goal").
		func(&BallState::script_finalIsValid, L"finalIsValid").
		func(&BallState::script_finalGetID, L"finalGetID").
		func(&BallState::script_finalGetState, L"finalGetState").
		func(&BallState::script_finalGetPitchInTeam, L"finalGetPitchInTeam").
		func(&BallState::script_getInstantGoalEstimate, L"getInstantGoalEstimate");

	BallGoalEstimate::declareInVM(target);
}

void SpatialMatchState::BallState::BallGoalEstimate::declareInVM(SquirrelVM& target) {

	SqPlus::SQClassDef<BallGoalEstimate>(ScriptClassName).
		func(&BallGoalEstimate::script_isValid, L"isValid").
		func(&BallGoalEstimate::script_isInstantaneous, L"isInstantaneous").
		func(&BallGoalEstimate::script_isCrossingGoalPlane, L"isCrossingGoalPlane").
		func(&BallGoalEstimate::script_isInGoalDir, L"isInGoalDir").
		func(&BallGoalEstimate::script_isBetweenGoalBars, L"isBetweenGoalBars").
		func(&BallGoalEstimate::script_getGoalBarDistLeft, L"getGoalBarDistLeft").
		func(&BallGoalEstimate::script_getGoalBarDistRight, L"getGoalBarDistRight").
		func(&BallGoalEstimate::script_getTime, L"getTime").
		func(&BallGoalEstimate::script_getPos, L"getPos").
		func(&BallGoalEstimate::script_getVel, L"getVel");
}

SpatialMatchState::BallState::BallGoalEstimate* SpatialMatchState::BallState::script_getInstantGoalEstimate(Match& match, int team, bool validOnly) {

	BallGoalEstimate& est = match.getTeam((TeamEnum) team).isTeamWithFwdDir() ? mInstantaneousGoalEstimateBack : mInstantaneousGoalEstimateFwd;

	if (validOnly && !est.isValid)
		return NULL;

	return &est;
}

SpatialMatchState::BallState::BallState() 
	: mFinalState(FinalBall_None), mFinalIsSimulProcessing(false), mFinalStateID(-1), mFinalSimulID(-1) {
}

void SpatialMatchState::BallState::setFinalState(FinalBallStateEnum state) {

	if (state != mFinalState) {

		mFinalState = state;
		++mFinalStateID;
	}
}

void SpatialMatchState::BallState::analyzeFinalStateExitPoint(Match& match, BallSimulation& simul, Vector2& exitPoint, CoordSysDirection exitDir[2]) {

	TeamEnum outTeamEnum = simul.getCommand().extractTeam();

	if (outTeamEnum == Team_Invalid)
		outTeamEnum = (TeamEnum) (trand<int>(10) % 2);

	bool isSideExit = exitDir[0] != CSD_Unused;
	bool isFwdExit = exitDir[1] != CSD_Unused;

	if (isFwdExit) {

		bool isGoal = false;

		{
			BallSimulation::Index count = simul.getTriggerSampleCount();

			for (BallSimulation::Index i = mFinalNextProcessTriggerIndex; i < count; ++i) {

				const BallSimulation::TriggerSample& sample = simul.getTriggerSample(i);

				if (sample.trigger->getID() == TriggerID_Goal) {

					isGoal = true;
					mFinalPitchInOwner = ((const MatchTrigger_Goal*) (sample.trigger.pObject))->getTeam();
					setFinalState(FinalBall_Goal);

					break;
				}
			}
		}


		if (!isGoal) {

			mFinalPitchInOwner = opponentTeam(outTeamEnum);

			if (match.getTeamWithWorldFwdDir(exitDir[1] == CSD_Forward) == outTeamEnum) {

				setFinalState(FinalBall_Corner);

			} else {
				
				setFinalState(FinalBall_Goalie);
			}
		}

	} else {

		mFinalPitchInOwner = opponentTeam(outTeamEnum);

		setFinalState(FinalBall_ThrowIn);
	}
}

void SpatialMatchState::BallState::updateFinalState(Match& match, const Clock& clock) {

	bool doUpdate = false;
	BallSimulation& simul = (match.getFootballerBallInterceptManager().getSimul());

	if (mFinalState == FinalBall_None) {

		doUpdate = true;

	} else {

		if (simul.getSimulState() != BallSimulation::Simul_None) {

			if (mFinalSimulID == simul.getID()) {

				if (mFinalIsSimulProcessing)
					doUpdate = true;

			} else {

				doUpdate = true;
			}

		} else {

			doUpdate = true;
		}
	}

	if (doUpdate) {

		if (simul.getSimulState() != BallSimulation::Simul_None) {

			if (mFinalSimulID != simul.getID()) {

				setFinalState(FinalBall_None);
				mFinalSimulID = simul.getID();
				mFinalIsSimulProcessing = true;
				mFinalNextProcessIndex = 0;
				mFinalNextProcessTriggerIndex = 0;
			}

			if (mFinalIsSimulProcessing) {

				PitchInfo& pitchInfo = match.getPitchInfoRef();

				Vector2 tacticalPos[2];
				Vector2 exitPoint;
				CoordSysDirection exitDir[2];
				int posIndex = 0;

				BallSimulation::Index count = simul.getSampleCount();
				
				if (count > 0) 
					--count;
				
				if (count) {

					BallSimulation::Index startIndex = mFinalNextProcessIndex;

					if (startIndex > 0)
						--startIndex;

					if (count) {

						const BallSimulation::TrajSample& sample = simul.getSample(0);
						match.toScene2D(sample.pos, tacticalPos[posIndex]);
						posIndex = (posIndex + 1) % 2;
					}

					for (BallSimulation::Index i = startIndex; i < count; ++i) {

						const BallSimulation::TrajSample& nextSample = simul.getSample(i + 1);
						
						match.toScene2D(nextSample.pos, tacticalPos[posIndex]);
						posIndex = (posIndex + 1) % 2;

						if (!pitchInfo.contains(tacticalPos[posIndex], exitPoint, &tacticalPos[(posIndex + 1) % 2], &(exitDir[0]), &(exitDir[1]))) {

							analyzeFinalStateExitPoint(match, simul, exitPoint, exitDir);
							break;
						}
					}

					mFinalNextProcessIndex = simul.getSampleCount();
					mFinalNextProcessTriggerIndex = simul.getTriggerSampleCount();
				}
				
				if (mFinalIsSimulProcessing && simul.isValidSimul()) {

					if (mFinalNextProcessIndex == 0 && simul.getSampleCount() == 1) {

						//simul contains only 1 sample that was never processed

						const BallSimulation::TrajSample& sample = simul.getSample(0);
						match.toScene2D(sample.pos, tacticalPos[posIndex]);
						
						if (!pitchInfo.contains(tacticalPos[posIndex], exitPoint, NULL, &(exitDir[0]), &(exitDir[1]))) {

							analyzeFinalStateExitPoint(match, simul, exitPoint, exitDir);
						}
					}

					if (mFinalState == FinalBall_None)
						setFinalState(FinalBall_InPlay);

					mFinalIsSimulProcessing = false;
				} 
			}
		
		} else {

			setFinalState(FinalBall_None);
		}
	}
}

float SpatialMatchState::BallState::BallGoalEstimate::calcBarDist(const SpatialPitchModel::GoalInfo& goalInfo, const Vector3& ballPos, const Vector3& ballVel, const Vector3& barPos) {

	if (!this->vel.isZero()) {

		float u;
		Vector3 closestPt;

		u = distanceLineU(ballPos, ballVel, barPos);

		if (u >= 0.0f) {

			ballPos.addMultiplication(ballVel, u, closestPt);
			closestPt.el[Scene_Up] = barPos.el[Scene_Up];

			return distance(closestPt, barPos);
		} 
	}

	return distance(this->pos, barPos);
}

void SpatialMatchState::BallState::updateInstantanousGoalEstimate(Match& match, const Clock& clock, Ball& ball, BallGoalEstimate& estimate, const SpatialPitchModel::GoalInfo& goalInfo) {
		
	estimate.isInstantaneous = true;

	if (ball.isInPlay()) {

		const Vector3& ballPos = ball.getPos();
		Vector3 ballVel = (ball.getVel() != NULL) ? dref(ball.getVel()) : Vector3::kZero;

		if (ball.isOnPitch())
			ballVel.el[Scene_Up] = 0.0f;

		switch (goalInfo.pPlaneFace->classify(ballPos)) {

			case PP_None:
			case PP_Back: {

				estimate.isValid = false;

			} break;

			default: {

				estimate.isValid = true;

			} break;
		}

		if (estimate.isValid) {

			bool computeBarDistances = true;

			estimate.time = -1.0f;
			estimate.isCrossingGoalPlane = false;
			estimate.isInGoalDir = false;

			if (ball.isResting()) {
				
				estimate.pos = ballPos;
				estimate.vel.zero();

			} else {
			
				float dirDot = ballVel.dot(goalInfo.pPlaneFace->normal);
				
				if (dirDot < 0.0f) {

					estimate.isInGoalDir = true;
					float u;

					if (intersectPlane(goalInfo.pPlaneFace->position, goalInfo.pPlaneFace->normal, ballPos, ballVel, u, &estimate.pos)) {

						estimate.isCrossingGoalPlane = true;
						estimate.vel = ballVel;

						if (ballVel.isZero()) {

							assrt(false);
							estimate.isInGoalDir = false;

						} else {
						
							estimate.time = sqrtf(distanceSq(ballPos, estimate.pos) / ballVel.magSquared());
							estimate.time += clock.getTime();
						}

						//computeBarDistances = false;

						//estimate.goalBarDistLeft = distance(estimate.pos, dref(goalInfo.pPosBarLeft));
						//estimate.goalBarDistRight = distance(estimate.pos, dref(goalInfo.pPosBarRight));

					} else {

						assrt(false);

						estimate.isInGoalDir = false;

						estimate.pos = ballPos;
						estimate.vel = ballVel;
					}

				} else {

					estimate.isInGoalDir = dirDot <= 0.0f;

					estimate.pos = ballPos;
					estimate.vel = ballVel;
				}
			}

			if (computeBarDistances) {

				estimate.goalBarDistLeft = estimate.calcBarDist(goalInfo, ballPos, ballVel, dref(goalInfo.pPosBarLeft));
				estimate.goalBarDistRight = estimate.calcBarDist(goalInfo, ballPos, ballVel, dref(goalInfo.pPosBarRight));
			}

			estimate.isBetweenGoalBars = (goalInfo.pPlaneLeft->classify(estimate.pos) == PP_Front) && (goalInfo.pPlaneRight->classify(estimate.pos) == PP_Front);
		}

	} else {

		estimate.isValid = false;
	}
}

void SpatialMatchState::BallState::updateInstantanousGoalEstimates(Match& match, const Clock& clock) {

	Ball& ball = match.getBall();
	SpatialPitchModel& pitchModel = match.getSpatialPitchModel();

	updateInstantanousGoalEstimate(match, clock, ball, mInstantaneousGoalEstimateBack, pitchModel.getGoalInfoBack());
	updateInstantanousGoalEstimate(match, clock, ball, mInstantaneousGoalEstimateFwd, pitchModel.getGoalInfoFwd());
}

void SpatialMatchState::BallState::update(Match& match, const Clock& clock) {

	{
		updateFinalState(match, clock);
	}

	{
		Ball& ball = match.getBall();
		SpatialPitchModel& pitchModel = match.getSpatialPitchModel();
		PitchRegionShape& pitchRegion = pitchModel.getRegionPitch();

		if (pitchRegion.contains(match, ball.getPos())) {

			ball.setIsInPlay(true);

		} else {

			ball.setIsInPlay(false);
		}
	}

	{
		updateInstantanousGoalEstimates(match, clock);
	}
}

} }