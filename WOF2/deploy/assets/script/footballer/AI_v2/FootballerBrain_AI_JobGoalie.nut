/*
	consolidate those FootballerBrain_AI_JobGoalie_ActionBase classes for use as 'StateStateInfo' in all goals/jobs
*/
class FootballerBrain_AI_JobGoalie_ActionBase {

	static Type_None = -1;
	static Type_Face = 0;
	static Type_FaceTeamDir = 1;
	static Type_PositionDepth = 2;
	static Type_Position = 3;
	static Type_Cover = 4;
	
	static Threat_None = -1;
	static Threat_Ball = 0;
	static Threat_BallOwner = 1;
	static Threat_Opponents = 2;
	
	weight = 0.0;
	
	function getType() { return Type_None; }
}

class FootballerBrain_AI_JobGoalie_FaceTeamDir extends FootballerBrain_AI_JobGoalie_ActionBase {

	function getType() { return Type_FaceTeamDir; }
};

class FootballerBrain_AI_JobGoalie_PositionDepth extends FootballerBrain_AI_JobGoalie_ActionBase {

	threatType = -1;
	depth = 0.0;
	
	function getType() { return Type_PositionDepth; }
};

class FootballerBrain_AI_JobGoalie_Position extends FootballerBrain_AI_JobGoalie_ActionBase  {

	threatType = -1;
	position = null;
	idealArrivalTime = null;
	
	function getType() { return Type_Position; }
	
	constructor() {
	
		position = Vector3();
	}
	
	function create(_threatType, _weight, _idealArrivalTime, _position) {
	
		local action = FootballerBrain_AI_JobGoalie_Position();
	
		action.threatType = _threatType;
		action.weight = _weight;
		action.idealArrivalTime = _idealArrivalTime;
			
		if (_position)
			action.position.set(_position);
		
		return action;
	}
};

class FootballerBrain_AI_JobGoalie_Cover extends FootballerBrain_AI_JobGoalie_ActionBase  {

	threatType = -1;
	coveredPos = null;
	attacker = null;
	region = null;
	
	function getType() { return Type_Cover; }
	
	constructor() {
	
		coveredPos = Vector3();
	}
	
	function create(_threatType, _weight, _coveredPos, _attacker, _region) {
	
		local action = FootballerBrain_AI_JobGoalie_Cover();
	
		action.threatType = _threatType;
		action.weight = _weight;
		action.coveredPos.set(_coveredPos);
		action.attacker = _attacker;
		action.region = _region;
		
		return action;
	}
};

class FootballerBrain_AI_TacticalPositioner {

	static State_None = -1;
	static State_Optimal = 0;
	static State_CanImprove = 1;
	static State_NeedsImprove = 2;
	
	function update(match, ball, brain, footballer, t, dt, threatPos, attacker) { return State_Optimal; }
	function getOptimalPos() { return null; }
};

class FootballerBrain_AI_TacticalPositioner_FixedGoalBoxArc extends FootballerBrain_AI_TacticalPositioner {

	constructor() {
	
		mOptimalPos = Vector3();
		mActionStartState = FootballerBrain_AI_JobGoalie_Cover();
	}

	function update(match, ball, brain, footballer, t, dt, threatPos, attacker) {
			
		if (threatPos == null)
			threatPos = ball.getPos();
			
		local impl = footballer.impl;	
		local goalInfo = match.getSpatialPitchModel().getTeamGoalInfo(impl.getTeamEnum());	
		local region = goalInfo.getGoalBoxRegion();
		
		region.clipByDestTo(match, goalInfo.getCenterPos(), threatPos, mOptimalPos, footballer.settings.PitchHeight);
		
		//printV3("mOptimalPos " + t, mOptimalPos);
		
		local distanceSq = distSq(impl.getPos(), mOptimalPos);
		local runSettings = footballer.settings.run;
		
		
		if (distanceSq <= runSettings.getMoveEpsilonSq(runSettings.Run_Normal, runSettings.RunSpeed_Normal)) {
		
			//print("optimal");
			return State_Optimal;
		}
		
		mActionStartState.coveredPos.set(goalInfo.getCenterPos());
		mActionStartState.attacker = attacker;
		mActionStartState.region = region;
		
		//print("needsImprove " + distanceSq + " / " + runSettings.getMoveEpsilonSq(runSettings.Run_Normal, runSettings.RunSpeed_Normal));
		return State_NeedsImprove;
	}
	
	function getOptimalPos() { return mOptimalPos; }
	
	function getActionStartState() {
	
		return mActionStartState.attacker == null ? null : mActionStartState;
	}
	
	mOptimalPos = null;
	mActionStartState = null;
};

class FootballerBrain_AI_JobGoalie extends FootballerBrain_AI_Goal {

	static State_Thinking = 0;
	static State_TacticalPosition = 1;
	
	constructor() {
	
		mActions = [];
		mManagedActiveGoal = FootballerBrain_AI_SingleUseGoal(null);
		mTrajSample = CBallTrajSample();
		mOppFootballerIterator = CTeamFootballerIterator();
		mTacticalPositioner = FootballerBrain_AI_TacticalPositioner_FixedGoalBoxArc();
		
		mTempVect3 = Vector3();
	}

	function start(brain, footballer, t) { 
	
		mState = State_Thinking;
		mOppFootballerIterator.setTeam(footballer.impl.getMatch(), footballer.impl.getOppTeamEnum());
		mOppFootballerIterator.setMaxConsecutiveNextCount(4);

		brain.body_idle(footballer, t);		
		
		return doReturn(t, Result_Processing); 
	}
	
	function addAction(action) {
	
		mActions.append(action);
	}
	
	function isLargerDepth(isTeamWithFwdDir, ref, comp) {
	
		return isTeamWithFwdDir ? comp > ref : comp < ref;
	}
	
	function isSamllerDepth(isTeamWithFwdDir, ref, comp) {
	
		return isTeamWithFwdDir ? comp < ref : comp > ref;
	}
	
	function think_self(match, ball, brain, footballer, t, dt) {
	
		if (!mManagedActiveGoal.hasGoal()) {
		
			if (!FootballerBrain_AI_GoalFace.isWithinTolerance(footballer, footballer.impl.getTeamFwdDir())) {
			
				addAction(FootballerBrain_AI_JobGoalie_FaceTeamDir());
			}
		}
	}
	
	function getFreeTimeBeforeRunDist(runSettings, runDist, refTime, interceptTime, runType, reactionTime) {
	
		local runSpeed = runSettings.getSpeedValue(runType, runSettings.RunSpeed_Normal);
		local timeToIntercept = runDist / runSpeed;
		local freeTimeBeforeIntercept = interceptTime - (refTime + timeToIntercept);
								
		freeTimeBeforeIntercept -= reactionTime;
					
		return freeTimeBeforeIntercept;			
	}
	
	function getFreeTimeBeforeInterceptDim(runSettings, refPos, refTime, interceptPos, interceptTime, runType, reactionTime) {
	
		return getFreeTimeBeforeRunDist(runSettings, fabs(refPos - interceptPos), refTime, interceptTime, runType, reactionTime);
	}
	
	function getFreeTimeBeforeIntercept(runSettings, refPos, refTime, interceptPos, interceptTime, runType, reactionTime) {
	
		return getFreeTimeBeforeRunDist(runSettings, dist(refPos, interceptPos), refTime, interceptTime, runType, reactionTime);	
	}
	
	function clipDim(match, refPos, region, depth, dim) {
	
		mTempVect3.set(refPos);
		mTempVect3.setEl(dim, depth);
		
		region.clip(match, mTempVect3);
		
		return mTempVect3.getEl(dim);
	}
	
	function thinkTacticalPosition_Ball(match, ball, brain, footballer, t, dt, threatPos, threatSource) {
		
		if (mTacticalPositioner.update(match, ball, brain, footballer, t, dt, threatPos, threatSource) != FootballerBrain_AI_TacticalPositioner.State_Optimal) {

			local actionStart = mTacticalPositioner.getActionStartState();
			
			if (actionStart != null) {
			
				addAction(actionStart);		
			
			} else {
		
				/*
		
				local action = FootballerBrain_AI_JobGoalie_Position.create(FootballerBrain_AI_JobGoalie_ActionBase.Threat_BallOwner, 0.5, t, mTacticalPositioner.getOptimalPos());
				//footballer.impl.getRegionPenalty().clip(match, action.position);
						
				addAction(action);		
				*/
			}
		}
	}
	
	function thinkTacticalPosition_BallOwners(match, ball, brain, footballer, t, dt) {
	
		local ownerCount = ball.getOwnerCount();
		local oppTeamEnum = footballer.impl.getOppTeamEnum();
		local oppBallOwnerCount = ball.getTeamOwnerCount(oppTeamEnum);
		local threatSourceOpp = null;
		
		if (oppBallOwnerCount > 0) {
		
			if (oppBallOwnerCount == 1) {
			
				for (local i = 0; i < ownerCount; i += 1) {
			
					local owner = ball.getOwner(i);
					
					if (owner.impl.getTeamEnum() == oppTeamEnum) {
					
						threatSourceOpp = owner;
						break;
					}
				}
			} 
		}
				
		
		if (oppBallOwnerCount > 0) {
		
			thinkTacticalPosition_Ball(match, ball, brain, footballer, t, dt, threatSourceOpp == null ? null : threatSourceOpp.impl.getPos(), threatSourceOpp);
		}
	}
	
	function think_ball(match, ball, brain, footballer, t, dt) {
	
		local impl = footballer.impl;
		
		if (impl.isBallOwner()) {
		
			//TODO
		
		} else {
		
			if (ball.isInPlay()) {
			
				local doTacticalPositioning = true;
			
				if (ball.getOwnerCount() <= 0) {
			
					local instGoalEstimate = match.getSpatialBallState().getInstantGoalEstimate(match, impl.getTeamEnum(), true);
					local threatLevel = 0.0;
				
					if (instGoalEstimate.isCrossingGoalPlane() && instGoalEstimate.isBetweenGoalBars()) {
					
						//print("Goal Threat");
						threatLevel = 2.0;
						
					} else {
					
						local dangerDist = match.conv(1.5); //adaptive?
					
						if (instGoalEstimate.getGoalBarDistLeft() <= dangerDist) {
						
							//print("Goal Threat Left");
							threatLevel = 1.0;
												
						} else if (instGoalEstimate.getGoalBarDistRight() <= dangerDist) {
						
							//print("Goal Threat Right");
							threatLevel = 1.0;
						}
					}
					
					local haveToActOnThreat = false;
					local threatTrajSample = mTrajSample;
					local threatActionDepth = null;
					
					if (threatLevel > 0.0) {
					
						local freeTimeBeforeHandlingThreat = 0.0;
						local selfPos = impl.getPos();
						local isFallbackHolder = Bool();
						
						local timeToReact = 0.3;
						
						//TODO!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
						//we should cache results here when simul valid!!!
										
						if (match.estimateBallStateAtPlane(selfPos, impl.getTeamFwdDir(), threatTrajSample, false, false, true, isFallbackHolder)) {
						
							mTempVect3.set(threatTrajSample.getPos());
							mTempVect3.setEl(::Scene_Up, footballer.settings.PitchHeight);
						
							freeTimeBeforeHandlingThreat = getFreeTimeBeforeIntercept(footballer.settings.run, selfPos, t, mTempVect3, threatTrajSample.getTime(), FootballerSettings_Run.Run_Normal, timeToReact);
						}
						
						haveToActOnThreat = (freeTimeBeforeHandlingThreat <= 0.0);
						
						{
							//check ball height / lobs
						
							local heightDiff = threatTrajSample.getPos().getEl(::Scene_Up) - footballer.settings.jump.JumpUp_HandPosAtApogee.getEl(::Scene_Up);
							heightDiff += match.conv(0.1);
							
							if (heightDiff > 0.0) {
							
								//if (haveToActOnThreat)
								//	print("sample " + match.invConv(threatTrajSample.getPos().getEl(::Scene_Up)) + " -> " + isFallbackHolder.get());
							
								//for now simply go to max back
								threatActionDepth = impl.isTeamWithFwdDir() ? -match.getPitchHalfLength() : match.getPitchHalfLength();
							}
						}
					}
					
					if (haveToActOnThreat) {
					
						//print("Handle Threat");
						local action = FootballerBrain_AI_JobGoalie_Position.create(FootballerBrain_AI_JobGoalie_ActionBase.Threat_Ball, 1.0, threatTrajSample.getTime() < 0.0 ? t : threatTrajSample.getTime(), threatTrajSample.getPos());
						
						if (threatActionDepth != null)
							action.position.setEl(::Scene_Fwd, threatActionDepth);
						
						impl.getRegionPenalty().clip(match, action.position);
						
						addAction(action);
						doTacticalPositioning = false;
					
					} else {
					
						//Tactical Depth Position
					
						/*
						local isTeamWithFwdDir = impl.isTeamWithFwdDir();
						local currDepth = impl.getPos().getEl(::Scene_Fwd);
						local minDepth = currDepth;
					
						local ballDepth = instGoalEstimate.getPos().getEl(::Scene_Fwd);
						
						if (isSamllerDepth(isTeamWithFwdDir, currDepth, ballDepth)) {
						
							local freeTimeBeforeIntercept = instGoalEstimate.getTime() - (t + timeToIntercept);
							
							local timeToReact = 0.3;						
							local freeTimeBeforeIntercept = getFreeTimeBeforeInterceptDim(footballer.settings.run, currDepth, t, ballDepth, instGoalEstimate.getTime(), FootballerSettings_Run.Run_StrafeBack, timeToReact);
							
							if (freeTimeBeforeIntercept < 0.0) {
							
								minDepth = ballDepth;
							}
						}
						
						if (minDepth != currDepth) {
						
							local action = FootballerBrain_AI_JobGoalie_PositionDepth();
							action.threatType = action.Threat_Ball;
							action.depth = minDepth;
							
							clipDim(match, refPos, region, depth, dim) {
							
							addAction(action);
						}
						*/
					}
					
				} else {
				}
				
				if (doTacticalPositioning)
					thinkTacticalPosition_BallOwners(match, ball, brain, footballer, t, dt);
							
			} else {
			
			}
		}
	}
	
	/*
	function gatherTacticalOpponentDepth(match, opponentImpl, footballerImpl) {
	
		local isTeamWithFwdDir = footballerImpl.isTeamWithFwdDir();
	
		local oppDepth = opponentImpl.getPos().getEl(::Scene_Fwd);
		local selfDepth = footballerImpl.getPos().getEl(::Scene_Fwd);
		
		if (!isTeamWithFwdDir) {
		
			oppDepth = -oppDepth;
			selfDepth = -selfDepth;
		}
		
		local depthDiff = oppDepth - selfDepth;

		if (depthDiff < 0.0) {
		
			if (mCycleTacticalOppDepth == null) 
				mCycleTacticalOppDepth = oppDepth;
			else
				mCycleTacticalOppDepth = minf(mCycleTacticalOppDepth, oppDepth);
		}		
	}
	*/
	
	/*
	function think_opponent(match, ball, brain, footballer, opponent, t, dt) {
	
		if (opponent.impl.isBallOwner()) {
		
		} else {
		
			gatherTacticalOpponentDepth(match, opponent.impl, footballer.impl);
		}
	}
	
	function think_opponents(match, ball, brain, footballer, t, dt) {
	
		local opponent;
		local isTeamWithFwdDir = impl.isTeamWithFwdDir();
		
		while ((opponent = mOppFootballerIterator.next()) != null) {
		
			think_opponent(match, ball, brain, footballer, opponent, t, dt);
		}
	}
	*/
	
	
			
	function think(brain, footballer, t, dt) { 
	
		local match = footballer.impl.getMatch();
		
		local ball = match.getBall();
		
		think_self(match, ball, brain, footballer, t, dt);
		think_ball(match, ball, brain, footballer, t, dt);
		//think_opponents(match, ball, brain, footballer, t, dt);
	}
			
	function combineActions(finalAction, action) {
	
		local retAction = finalAction;
	
		if (action.weight > finalAction.weight)
			retAction = action;
			
		return retAction;		
	}
	
	function setGoalByClass(brain, classType) {
	
		if (mManagedActiveGoal.hasGoal()) {
		
			if (!(mManagedActiveGoal.getGoal() instanceof classType)) {
			
				mManagedActiveGoal.stop(brain);
			}
		} 
		
		local goal = null;
		
		if (!mManagedActiveGoal.hasGoal()) {
		
			goal = classType();
			mManagedActiveGoal.setGoal(goal);
			
		} else {
		
			goal = mManagedActiveGoal.getGoal();
		}
	
		return goal;
	}	
	
	function setGoalFace(brain) {
	
		return setGoalByClass(brain, FootballerBrain_AI_GoalFace);
	}	
			
	function setGoalGoto(brain, strafe, toleranceMode) {
	
		local goal = setGoalByClass(brain, FootballerBrain_AI_GoalGoto);
		
		goal.setStrafe(strafe);
		goal.setToleranceMode(toleranceMode);
		
		return goal;
	}	
	
	function setGoalCover(brain, coveredPoint, attacker, region) {
	
		local goal = setGoalByClass(brain, FootballerBrain_AI_GoalCover);
		
		goal.setCovedredPoint(coveredPoint);
		goal.setOpponent(attacker);
		goal.setRegion(region);
		
		return goal;
	}	
	
	function applyAction_FaceTeamDir(action, brain, footballer, t, dt) {
	
		local goalFace = setGoalFace(brain);
		
		goalFace.setTargetByDir(footballer, footballer.impl.getTeamFwdDir(), false);
	}
	
	
			
	function applyAction_PositionDepth(action, brain, footballer, t, dt) {
	
		local goalGoto = setGoalGoto(brain, true, FootballerBrain_AI_GoalGoto.Tolerance_JustBefore);
		
		local impl = footballer.impl;
							
		local target = goalGoto.getTarget();
		target.set(impl.getPos());
		target.setEl(::Scene_Fwd, finalAction.depth);
	
		goalGoto.setTarget(footballer, null, true);
	}
	
	function applyAction_Position(action, brain, footballer, t, dt) {
	
		local impl = footballer.impl;
		local match = impl.getMatch();		
		
		local selfPos = impl.getPos();
		local targetPos = action.position;
		local allowStrafe = true;
		
		if (action.idealArrivalTime != null) {
						
			local freeTime = getFreeTimeBeforeIntercept(footballer.settings.run, selfPos, t, targetPos, action.idealArrivalTime, FootballerSettings_Run.Run_StrafeLeft, 0.0);
			
			allowStrafe = (freeTime >= 0.0);
		}
		
		if (allowStrafe) {
		
			targetPos.subtract(impl.getPos(), mTempVect3);
			
			if (mTempVect3.dot(impl.getFacing()) < 0.0)
				allowStrafe = false;
		}
	
		local goalGoto = setGoalGoto(brain, allowStrafe, FootballerBrain_AI_GoalGoto.Tolerance_JustBefore);
		
		goalGoto.setTarget(footballer, targetPos, true);
	}
	
	function applyAction_Cover(action, brain, footballer, t, dt) {
	
		local goalGoto = setGoalCover(brain, action.coveredPos, action.attacker, action.region);
	}
	
	function processActions(brain, footballer, t, dt) {
	
		local finalAction = null;
			
		foreach (action in mActions) {
		
			if (finalAction == null) {
				
				finalAction = action;
				
			} else {
			
				finalAction = combineActions(finalAction, action);
			}
		}
	
		mActions.resize(0);
		
		if (finalAction != null) {
		
			switch (finalAction.getType()) {
			
				case FootballerBrain_AI_JobGoalie_ActionBase.Type_FaceTeamDir: {
				
					applyAction_FaceTeamDir(finalAction, brain, footballer, t, dt);
				
				} break;
			
				case FootballerBrain_AI_JobGoalie_ActionBase.Type_PositionDepth: {
				
					applyAction_PositionDepth(finalAction, brain, footballer, t, dt);
					
				} break;
				
				case FootballerBrain_AI_JobGoalie_ActionBase.Type_Position: {
				
					applyAction_Position(finalAction, brain, footballer, t, dt);
					
				} break;
				
				case FootballerBrain_AI_JobGoalie_ActionBase.Type_Cover: {
				
					applyAction_Cover(finalAction, brain, footballer, t, dt);
					
				} break;
			}
					
		} else {
		
			mManagedActiveGoal.stop(brain);
		}
	}
			
	function frameMove(brain, footballer, t, dt) { 
	
		think(brain, footballer, t, dt);
		processActions(brain, footballer, t, dt);
		
		if (mManagedActiveGoal.update(brain, footballer, t, dt, null, null, null, null) != Result_Processing) {
		
			brain.body_idle(footballer, t);
		}
		
		return doReturn(t, Result_Processing); 
	}
	
	mState = -1;
	mTempVect3 = null;
	
	mActions = null;
	mTrajSample = null;
	mManagedActiveGoal = null;
	
	mOppFootballerIterator = null;
	mTacticalPositioner = null;
}