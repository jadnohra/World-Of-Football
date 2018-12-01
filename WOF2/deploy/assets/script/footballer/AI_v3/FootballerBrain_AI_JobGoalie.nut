
class FootballerBrain_AI_JobGoalie_ReachabilityProfile {

	static Reachable_No = -1;
	static Reachable_Run = 0;
	static Reachable_Jump = 1;
	static Reachable_RunJump = 2;
	static Reachable_2D = 3;

	static State_None = -1;
	static State_Waiting = 0;
	static State_Executing = 1;
	static State_Executed = 2;
	
	constructor() {
	
		mTargetPos = Vector3();
	}
	
	function setup(runSafetyTime, runNoWaitTime, runHumanizeRadius) {
	
		mRunSafetyTime = runSafetyTime;
		mRunNoWaitTime = runNoWaitTime;
		mRunHumanizeRadius = runHumanizeRadius;
	}
	
	function setLookAt(lookAtPos) {
	
		if (lookAtPos == null) {
		
			mLookAtPos = null;
			
		} else {
		
			if (mLookAtPos == null)
				mLookAtPos = Vector3();
				
			mLookAtPos.set(lookAtPos);	
		}
	}
	
	function analyze(match, body, t, targetPos, arrivalTime, targetHeightOffset, checkRedundant) {
	
		if (checkRedundant) {
		
			if (mTargetPos.equals(targetPos) && mTargetArrivalTime == arrivalTime)
				return false;
		}
	
		mTargetPos.set(targetPos);
		mTargetArrivalTime = arrivalTime;
	
		local currPos = body.mImpl.getPos();
		local currTime = t;
		
		local dist2D = dist2Dim(currPos, targetPos, ::Scene_Right, ::Scene_Fwd);
		
		local runProfile = body.mProfile.run;
		local runSpeed = runProfile.getSpeedValue(runProfile.Run_Normal, runProfile.RunSpeed_Normal);
		
		local runArrivalTime2D = currTime + (dist2D / runSpeed);

		if (runArrivalTime2D <= arrivalTime) {
		
			m2DReachable = Reachable_Run;
			m2DRunStartTime = currTime + (arrivalTime - runArrivalTime2D);
			m2DUnreachableDistLeft = 0.0;
		
		} else {
		
			m2DRunStartTime = currTime;
		
			local jumpApogeeTime = body.mProfile.jump.jumpProfile.ApogeeTime;
			local maxJumpDist2D = body.mProfile.jump.JumpDistance_Default;
			local maxJumpHandApogeeDist2D = maxJumpDist2D + body.mProfile.jump.FlyPhase_HandPosAtApogee.getEl(::Scene_Right);
		
			//ignoring time to turn if any
		
			m2DJumpStartTime = arrivalTime - jumpApogeeTime;
			local timeToRun = maxf(0.0, m2DJumpStartTime - currTime);
			local runDistAtJumpStart = runSpeed * timeToRun;
			
			local distToCoverByJump2D = maxf(0.0, dist2D - runDistAtJumpStart);
						
			if (distToCoverByJump2D <= maxJumpHandApogeeDist2D) {
			
				m2DReachable = (timeToRun == 0.0 ? Reachable_Jump : Reachable_RunJump);
				m2DUnreachableDistLeft = 0.0;
			
			} else {
			
				m2DReachable = Reachable_No;
				m2DUnreachableDistLeft = distToCoverByJump2D - maxJumpHandApogeeDist2D;
			}
		}
		
		local heightDiff = (targetPos.getEl(::Scene_Up) + targetHeightOffset) - currPos.getEl(::Scene_Up);
				
		switch (m2DReachable) {
		
			case Reachable_Run: {
			
				if (heightDiff >= body.mProfile.BallHeaderHeight) {
				
					local jumpApogeeTime = body.mProfile.jump.jumpUpProfile.ApogeeTime;
					local maxJumpHeight = body.mProfile.jump.JumpDistance_MaxHeight;
					local maxJumpHandApogeeHeight = maxJumpHeight + body.mProfile.jump.JumpUp_HandPosAtApogee.getEl(::Scene_Up);
				
					if (heightDiff <= maxJumpHandApogeeHeight) {
					
						mReachable = Reachable_Jump;
						mJumpStartTime = arrivalTime - jumpApogeeTime;
					
					} else {
				
						mReachable = Reachable_No;
						mUnreachableHeightLeft = heightDiff - maxJumpHandApogeeHeight;
					}
				
				} else {
				
					mReachable = Reachable_2D;
				}
			
			} break;
			
			case Reachable_RunJump:
			case Reachable_Jump: {
			
				local jumpApogeeTime = body.mProfile.jump.jumpProfile.ApogeeTime;
				local maxJumpHeight = body.mProfile.jump.JumpDistance_MaxHeight;
				local maxJumpHandApogeeHeight = maxJumpHeight + body.mProfile.jump.FlyPhase_HandPosAtApogee.getEl(::Scene_Up);
				
				if (heightDiff <= maxJumpHandApogeeHeight) {
					
					mReachable = Reachable_2D;
					
				} else {
				
					mReachable = Reachable_No;
					mUnreachableHeightLeft = heightDiff - maxJumpHandApogeeHeight;
				}
							
			} break;
			
			//Reachable_No
			default: {
			
				mReachable = Reachable_2D;
				
			} break;
		}
		
		return true;
	}
	
	function isReachable(t) {
	
		switch (mReachable) {
		
			case Reachable_Jump: {
			
				return (mJumpStartTime - t >= 0.0);
			
			} break;
			
			case Reachable_2D: {
			
				switch (m2DReachable) {
		
					
					case Reachable_Run: {
					
						return ((m2DRunStartTime - mRunSafetyTime) - t >= 0.0);
					
					} break;
					
					case Reachable_Jump: {
					
						return (m2DJumpStartTime - t >= 0.0);
					
					} break;
					
					case Reachable_RunJump: {
					
						return ((m2DRunStartTime - mRunSafetyTime) - t >= 0.0);
								
					} break;
				}
			
			} break;
		}
		
		return false;
	}
	
	function process(match, brain, body, t, dt) {
	
		local actionType = null;
		local startTime;
		local doJumpUp;
		local hasNextAction = false;
	
		switch (mReachable) {
		
			case Reachable_Jump: {
			
				startTime = (mJumpStartTime - t);
				actionType = Footballer.Action_Jumping;
				doJumpUp = true;
						
			} break;
			
			case Reachable_2D: {
			
				switch (m2DReachable) {
							
					case Reachable_Run: {		
							
						startTime = ((m2DRunStartTime - mRunSafetyTime) - t) - dt;
						actionType = Footballer.Action_Running;		
					
					} break;
					
					case Reachable_Jump: {
					
						startTime = (m2DJumpStartTime - t) - dt;
						actionType = Footballer.Action_Jumping;		
						doJumpUp = true;
					
					} break;
					
					case Reachable_RunJump: {

						startTime = (m2DJumpStartTime - t) - dt;
						
						if (startTime <= 0.0) {
					
							actionType = Footballer.Action_Jumping;
							
						} else {
						
							startTime = ((m2DRunStartTime - mRunSafetyTime) - t) - dt;
							actionType = Footballer.Action_Running;		

							hasNextAction = true;						
						}
						
					} break;
				}
			
			} break;
		}
		
		if (actionType != null) {
		
			local adjustedStartTime = startTime;
			
			switch (actionType) {

				case Footballer.Action_Running: {
				
					{
						switch (brain.getDestinationIDState(mTargetDestinationID, true)) {
						
							case brain.Destination_Reaching: {
					
								//print("continue run");
								return State_Executing;
								
							} break;
							
							case brain.Destination_Reached: {
					
								//print("reached");
								return hasNextAction ? State_Executing : State_Executed;
								
							} break;
						}
					}
					
					adjustedStartTime -= mRunNoWaitTime;
				
					if (adjustedStartTime >= 0.0) {
												
						//print("waiting");
						return State_Waiting;
					}
				
					mTargetDestinationID = brain.setDestination(body, t, mTargetPos, true, mTargetArrivalTime, 1.0, mRunHumanizeRadius);
					
					if (brain.hasDestinationPos()) {
					
						//print("start run");
					
						brain.setBodyFacingLookAt(body, t, mLookAtPos == null ? mTargetPos : mLookAtPos, null, 0.5);
					
						return State_Executing;
					}
					
					//print("in place");
					
					return hasNextAction ? State_Executing : State_Executed;
					
				} break;
				
				case Footballer.Action_Jumping: {
				
					if (adjustedStartTime >= 0.0) {
		
						//print("waiting");
						return State_Waiting;
			
					} else {
				
						print("jump");
				
						brain.idle(body, t);
						return hasNextAction ? State_Executing : State_Executed;
					}
				
				} break;
			}
		}
		
		//print("none");
		
		return State_None;
	}
	
	function toString(t) {
	
		local ret = "";
	
		switch (mReachable) {
		
			case Reachable_No: {
			
				ret += "3D: Not Reachable, heightLeft " + mUnreachableHeightLeft;
				
			} break;
			
			case Reachable_Jump: {
			
				ret += "3D: Lob Reachable, timeToJump" + (mJumpStartTime - t);
			
			} break;
			
			case Reachable_2D: {
			
				ret += "3D: Like 2D";
			
			} break;
			
			default: {
			
				ret += "3D: ???";
			
			} break;
		}
		
		ret += ", ";
	
		switch (m2DReachable) {
		
			case Reachable_No: {
			
				ret += "2D: Not Reachable, distLeft " + m2DUnreachableDistLeft;
				
			} break;
			
			case Reachable_Run: {
			
				ret += "2D: Run Reachable, timeToRun " + (m2DRunStartTime - t);
			
			} break;
			
			case Reachable_Jump: {
			
				ret += "2D: Jump Reachable";
			
			} break;
			
			case Reachable_RunJump: {
			
				ret += "2D: RunJump Reachable, timeToJump " + (m2DJumpStartTime - t);
						
			} break;
		}
		
		return ret;
	}
	
	mRunSafetyTime = 0.0;
	mRunNoWaitTime = 0.0;
	mRunHumanizeRadius = null;
	
	mTargetPos = null;
	mTargetArrivalTime = null;
	mTargetDestinationID = null;
	
	mLookAtPos = null;
	
	m2DReachable = -1;
	m2DJumpStartTime = 0.0;
	m2DRunStartTime = 0.0;
	m2DUnreachableDistLeft = 0.0;
	
	mReachable = -1;
	mJumpStartTime = 0.0;
	mUnreachableHeightLeft = 0.0;
}

class FootballerBrain_AI_JobGoalie extends FootballerBrain_AI_Goal {

	constructor() {
	
		mTrajSample = CBallTrajSample();
		
		mTempVect3 = Vector3();
	}

	function start(brain, footballer, t) { 
	
		validateConfiguration(brain, footballer);
	
		brain.idle(footballer.mBody, t);		
		
		return doReturn(t, Result_Processing); 
	}
	
	function validateConfiguration(brain, footballer) {
	
		local match = footballer.mImpl.getMatch();
	
		if (mHeuristicPosition_Fwd == null) {
		
			mHeuristicPosition_Fwd = brain.helperDepthToFwd(footballer, match.conv(1.5));
			
			mHeuristicPosition_NoThreatDistSq = match.getPitchHalfLength();
			mHeuristicPosition_NoThreatDistSq *= mHeuristicPosition_NoThreatDistSq;
		}
	}
	
	
	
	
	function processBallOnGoalThreat(match, ball, brain, footballer, t, dt) {
	
		if ((footballer.mImpl.getSpatialStateBallDistIndex() == 0) && brain.willHaveValidDestinationOwnBall(footballer.mBody, 0.3)) {

			//keep going
			return true;	
		}
	
		local impl = footballer.mImpl;
			
		if (ball.getOwnerCount() <= 0) {
	
			local instGoalEstimate = match.getSpatialBallState().getInstantGoalEstimate(match, impl.getTeamEnum(), true);
			local threatLevel = 0.0;
		
			if (instGoalEstimate.isCrossingGoalPlane() && instGoalEstimate.isBetweenGoalBars()) {
			
				//print("Goal Threat");
				threatLevel = 2.0;
				
			} else {
			
				local dangerDist = match.conv(1.5); //adaptive? use ANN??
			
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
				local safetyTime = 0.75;
				
				{
					mTempVect3.set(ball.getVel());
					footballer.helperFixVel(mTempVect3);
					local estVel = mTempVect3;
					local uHolder = Float(0.0);
					
					local facing = impl.getTeamFwdDir();
					local intersectPos = Vector3();
					
					local chosenAction = -1;
					local ballArrivalTime = t;
					local humanizeRadius = null;
					
					if (intersectLinePlane(selfPos, facing, ball.getPos(), estVel, uHolder, intersectPos)) {
					
						footballer.helperFixFootballerGroundPos(intersectPos);
					
						local isFrontFacing = (uHolder.get() > 0.0);
					
						if (isFrontFacing) {
					
							local selfRadius = match.conv(0.4); //extract from bound box or hand pos at idle or ....
					
							ballArrivalTime = t + uHolder.get();
							freeTimeBeforeHandlingThreat = brain.getFreeTimeBeforeRunDestination(footballer.mBody.mProfile.run, selfPos, t, intersectPos, ballArrivalTime, ::FootballerBodyActionProfile_Run.Run_Normal, 0.0, selfRadius);
						
							local doWait = (freeTimeBeforeHandlingThreat - (safetyTime + timeToReact + dt)) > 0.0;
						
							if (doWait) {
							} else {
							
								if (freeTimeBeforeHandlingThreat >= 0.0) {
							
									chosenAction = 0;
									
								} else {
								
									chosenAction = 1;
								}
							}
							
						} else {
						
							//ball in the back?? or can't catch? or too late?
							
							if (threatLevel == 2.0) {
							
								chosenAction =  1;
							
							} else {
								
								chosenAction = 0;
							}
						}

					} else {
					
						//shouldn't happen!!
					}
					
					switch (chosenAction) {
					
						case 0: {
						
							//print("goto inters");
						
							brain.setDestination(footballer.mBody, t, intersectPos, true, ballArrivalTime, 1.0, humanizeRadius);
							brain.setBodyFacingLookAt(footballer.mBody, t, ball.getPos(), null, 0.5);
						
							return true;
						
						} break;
						
						case 1: {
						
							//print("goto inst estim");
						
							brain.setDestination(footballer.mBody, t, instGoalEstimate.getPos(), false, instGoalEstimate.getTime(), 1.0, humanizeRadius);
							brain.setBodyFacingLookAt(footballer.mBody, t, instGoalEstimate.getPos(), null, 0.5);
							
							return true;
							
						} break;
						
						case 2: {
						
							print("jump");
							
							processIdle(match, ball, brain, footballer, t, dt);
							
							return true;
							
						} break;
					}
				}
				
				//need ANN
				/*
				{
					//check ball height / lobs
				
					local heightDiff = threatTrajSample.getPos().getEl(::Scene_Up) - footballer.mBody.mProfile.jump.JumpUp_HandPosAtApogee.getEl(::Scene_Up);
					heightDiff += match.conv(0.1);
					
					if (heightDiff > 0.0) {
					
						//if (haveToActOnThreat)
						//	print("sample " + match.invConv(threatTrajSample.getPos().getEl(::Scene_Up)) + " -> " + isFallbackHolder.get());
					
						//for now simply go to max back
						threatActionDepth = impl.isTeamWithFwdDir() ? -match.getPitchHalfLength() : match.getPitchHalfLength();
					}
				}
				*/
			}
			
		} else {
		}
		
		return false;
	}
	
	function processPositioning(match, ball, brain, footballer, t, dt) {
	
		if (footballer.mImpl.getSpatialStateBallDistIndex() == 0) {

			if (!brain.hasDestinationOwnBall()) {
		
				brain.cancelBodyFacing(footballer.mBody, t);
				brain.setDestinationOwnBall(footballer.mBody, t, true);
				
				//print("ownball");
			}
		
			//go get the ball
			return true;	
		}
	
		local oppTeam = footballer.mImpl.getOppTeam();
		local goalInfo = match.getSpatialPitchModel().getTeamEnumGoalInfo(footballer.mImpl.getTeamEnum());
	
		local ballDistSq = distSq2Dim(ball.getPos(), goalInfo.getCenterPos(), ::Scene_Right, ::Scene_Fwd);
		
		local dest = null;
		local facing = null;
		local facingIsLookAt = false;
		
		local heuristicPosFwd = minf(mHeuristicPosition_Fwd, ball.getPos().getEl(::Scene_Fwd));
		
		if (ballDistSq < mHeuristicPosition_NoThreatDistSq) {
		
			local oppTacticalBallOwnership = oppTeam.getTacticalBallOwnership();
			
			//or any very close opp footballers to ball!!
			if (oppTacticalBallOwnership > 0.0) {
			
				local segPt1 = Vector3();
				local segPt2 = Vector3();
				
				segPt1.set(goalInfo.getBarLeftPos());
				segPt1.setEl(::Scene_Fwd, heuristicPosFwd);
				
				segPt2.set(goalInfo.getBarRightPos());
				segPt2.setEl(::Scene_Fwd, heuristicPosFwd);
				
				local u = 0.5;
				
				{
					local planeNormal = Vector3();
					ball.getPos().subtract(goalInfo.getCenterPos(), mTempVect3);
					mTempVect3.cross(::Scene_UpAxis, planeNormal);
					
					local uHolder = Float(0.5);
					
					if (!planeNormal.isZero()) {
					
						planeNormal.normalize();
					
						intersectSegPlane(goalInfo.getCenterPos(), planeNormal, segPt1, segPt2, uHolder, null);
						u = uHolder.get();
					}
				}
				
				if (u < 0.0)
					u = 0.0;
				else if (u > 1.0)
					u = 1.0;
					
				lerpv3(segPt1, segPt2, u, mTempVect3);	
			
				dest = mTempVect3;			
				
				facingIsLookAt = true;
				facing = ball.getPos();
				
				//print("goto posseg intersec");
			}
			
		} else {
		
			mTempVect3.set(goalInfo.getCenterPos());
			mTempVect3.setEl(::Scene_Fwd, heuristicPosFwd);
		
			dest = mTempVect3;
			
			facingIsLookAt = false;
			facing = footballer.mBody.mImpl.getTeamFwdDir();
			
			//print("goto posseg middle");
		}
		
		if (dest != null) {
		
			brain.setDestination(footballer.mBody, t, dest, true, t + 1.5, 0.2, brain.getDefaultHumanizeRadius());
			
			if (facingIsLookAt)
				brain.setBodyFacingLookAt(footballer.mBody, t, facing, null, 0.1);
			else
				brain.setBodyFacing(footballer.mBody, t, facing, false, null, 0.1);
			
			return true;
		}
		
		return false;
	}
	
	function processIdle(match, ball, brain, footballer, t, dt) {
	
		local body = footballer.mBody;
	
		brain.cancelGoalieRunJump(body, t);
		brain.cancelDestination(body, t);
		brain.setBodyFacing(body, t, body.mImpl.getTeamFwdDir(), false, null, 0.0);
		
		//print("stop and face");
	}
	
	function frameMove_v1(brain, footballer, t, dt) { 
	
		local match = footballer.mImpl.getMatch();
		local ball = match.getBall();
	
		if (ball.isInPlay()) {
		
			if (footballer.mImpl.isBallOwner()) {
			
				brain.idle(footballer.mBody, t);
				//print("idle");
				//and wait until ball stabilizes before doing anything
			
			} else {
			
				if (processBallOnGoalThreat(match, ball, brain, footballer, t, dt))
					return doReturn(t, Result_Processing); 	
					
				if (processPositioning(match, ball, brain, footballer, t, dt))
					return doReturn(t, Result_Processing); 		
					
				processIdle(match, ball, brain, footballer, t, dt);
			}
		
		} else {
		
			processIdle(match, ball, brain, footballer, t, dt);
		}
			
		return doReturn(t, Result_Processing); 
	}
	
	function frameMove_v2(brain, footballer, t, dt) { 
	
		local match = footballer.mImpl.getMatch();
		local ball = match.getBall();
		
		if (ball.isInPlay() && (ball.getOwnerCount() == 0) && (ball.isResting() != true)) {
		
			if (mReachabilityProfile == null) {
			
				mReachabilityProfile = FootballerBrain_AI_JobGoalie_ReachabilityProfile();
				
				mReachabilityProfile.setup(match.getFrameTickLength() * 2.0, 1.5, null);
			}
					
					
			mTempVect3.set(ball.getVel());
			footballer.helperFixVel(mTempVect3);
			local estVel = mTempVect3;
			local uHolder = Float(0.0);
					
			local intersectPos = Vector3();
					
			local chosenAction = -1;
			local ballArrivalTime = t;
					
			if (intersectLinePlane(footballer.mImpl.getPos(), footballer.mBody.mImpl.getTeamFwdDir(), ball.getPos(), estVel, uHolder, intersectPos)
					&& (uHolder.get() > 0.0)) {		
			
				ballArrivalTime = t + dist(ball.getPos(), intersectPos) / estVel.mag();
				
				brain.setGoalieRunJump(footballer.mBody, t, intersectPos, ballArrivalTime, ball.getRadius());
				brain.setGoalieRunJumpLookAt(ball.getPos());		
				
				return doReturn(t, Result_Processing); 
				
				//mReachabilityProfile.analyze(match, ball, brain, footballer, t, intersectPos, ballArrivalTime, ball.getRadius(), true);
				//print(mReachabilityProfile.toString(t));
				
				/*
				mReachabilityProfile.setLookAtPos(ball.getPos());
				
				switch (mReachabilityProfile.process(match, ball, brain, footballer, t, dt)) {
				
					case mReachabilityProfile.State_Executing: {
				
						return doReturn(t, Result_Processing); 
						
					} break;
					
					case mReachabilityProfile.State_Executed: {
				
						local body = footballer.mBody;
	
						brain.cancelDestination(body, t);
						brain.setBodyFacingLookAt(body, t, ball.getPos(), null, null);
						
						return doReturn(t, Result_Processing); 
						
					} break;
				}
				*/
			} else {
			
				
			}
		}
	
		processIdle(match, ball, brain, footballer, t, dt);
					
		return doReturn(t, Result_Processing); 
	}
	
	function frameMove(brain, footballer, t, dt) { 
	
		return frameMove_v2(brain, footballer, t, dt);
	}
	
	mTempVect3 = null;
	mTrajSample = null;
	
	mHeuristicPosition_Fwd = null;
	mHeuristicPosition_NoThreatDistSq = null;
	
	mReachabilityProfile = null; 
}