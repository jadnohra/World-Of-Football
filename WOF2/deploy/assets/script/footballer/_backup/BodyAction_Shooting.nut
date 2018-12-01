
class BodyActionShooting_StartState {

	isHeadShot = false;
	copyBallShot = true;
	ballShotType = -1;
	ballShot = null;
	shotSpeedScale = null;
}

class BodyAction_Shooting extends BodyActionBase {

	function getType(footballer) {
	
		return footballer.Action_Shooting;
	}

	constructor() {
	
		mBallShotDir = Vector3();
				
		ballControlOnConstructor();
	}

	function load(footballer, chunk, conv) {
	
		return true;
	}
	
	function isFinished() { return mIsFinished; }	
	
	function setBallShotDirection(footballer, dir) {
	
		if (dir != null)
			mBallShotDir.set(dir);
		else
			mBallShotDir.zero();
	}
	
	function hasBallShot() { return (mBallShot != null && mBallShot.isValid); }
	
	function createStartState(footballer, isHeader, shotType, shot, copyShot, shotCoeff) {
	
		local startState = BodyActionShooting_StartState();
		
		startState.isHeadShot = isHeader;
		startState.copyBallShot = copyShot;
		startState.ballShotType = shotType;
		startState.ballShot	= shot;
		startState.shotSpeedScale = shotCoeff;
		
		return startState;
	}
	
	function applyStartState(footballer, startState) {
	
		if (startState != null) {
			
			if (startState.copyBallShot && startState.ballShot != null) {
			
				mBallShot = FootballerBallShot(false, ::FootballerBallShot_None, 0.0, 0.0, 0.0, false);
				mBallShot.set(startState.ballShot);
				
			} else {
			
				mBallShot = startState.ballShot;
			}
			
			if (startState.shotSpeedScale != null)
				mBallShotSpeedScale = startState.shotSpeedScale;
			else 	
				mBallShotSpeedScale = 1.0;
			
			mBallShotType = startState.ballShotType;
			mIsHeadShot = startState.isHeadShot;
			
		} else {
		
			mIsHeadShot = false;
			mBallShotType = footballer.Shot_High;
			mBallShot = footballer.settings.Shots[footballer.settings.DefaultShotLevel][mBallShotType];
			mBallShotSpeedScale = 1.0;
		}
		
		//print("scale " + mBallShotSpeedScale);
		
		mBallShotDir.zero();
	}
	
	function start(footballer, t, startState, prevAction) {
		
		applyStartState(footballer, startState);
		
		{
			mJumpPhysics = null;
			local momentum = footballer.getBodyNonZeroMomentum();
					
			if (momentum != null) {
			
				local magSq = momentum.magSq();
				
				if (magSq > 0.0) {
				
					mJumpPhysics = FootballerBodyJumpPhysics();
				}
			} 	
		}
		
		mIsFinished = false;
		mDidLand = false;
		
		local impl = footballer.impl;
		local settings = footballer.shoot;
		
		impl.setAnim(mIsHeadShot ? settings.HeaderAnimName : settings.AnimName, true);
		impl.setAnimSpeed(mIsHeadShot ? settings.HeaderAnimSpeed : settings.AnimSpeed);
		impl.setAnimLoop(false);
		
		
		if (mJumpPhysics != null) {
		
			local settings = footballer.shoot;
		
			mJumpPhysics.init(footballer, footballer.impl.getFacing(), settings.HeaderJumpDistance_Default, 
								settings.FlyPhase_StartTime, settings.FlyPhase_EndTime);
		}
		
		if (mIsHeadShot) {
		
			if (settings.HeaderSoundName != null)
				impl.playSound(settings.HeaderSoundName, impl.getPos(), settings.HeaderSoundMag);
			
		} else {
		
			if (settings.KickSoundName != null)
				impl.playSound(settings.KickSoundName, impl.getPos(), settings.KickSoundMag);
		}
		
		mActionStartTime = t;
		
		ballControlOnStart(footballer, t, startState);
		
		footballer.setBodyMomentum(null);
		return true;
	}
	
	function stop(footballer, t) {
	
		mJumpPhysics = null;
		ballControlOnStop(footballer, t);
	
		mIsFinished = true;
	}
	
	function handleActionEnd(footballer) {
	
		if (footballer.impl.isAnimFinished()) {
		
			mIsFinished = true;
		}
	}
		
	
	function frameMove(footballer, t, dt) {
			
		if (mIsFinished) {
			
			switchIdle(footballer, t, null);
			return;
		}	
			
		local impl = footballer.impl;	
				
		impl.nodeMarkDirty();
			
		{		
			impl.addAnimTime(dt);		
		}
		
		local actionTime = t - mActionStartTime;
		
		if (mJumpPhysics != null) {
		
			if (actionTime > mJumpPhysics.startTime && actionTime < mJumpPhysics.endTime) {
			
				local posDiff = Vector3();
			
				mJumpPhysics.update(dt, posDiff);
				
				impl.shiftPos(posDiff);
				
			} 
		}
		
		if (mIsHeadShot && !mDidLand) {
		
			local settings = footballer.shoot;
		
			if (actionTime > settings.Land_TriggerTime) {
				
				mDidLand = true;
			}
			
			if (mDidLand && settings.LandSoundName != null) {			
			
				impl.playSound(settings.LandSoundName, impl.getPos(), settings.LandSoundMag);
			}
		}
		
		{
			if (impl.nodeIsDirty()) {

				impl.collUpdate();
				footballer.resolveCollisions(footballer, t, false);
			}
		}
		
		handleBallControlState(footballer, t, actionTime);
		
		handleActionEnd(footballer);
	}
	
	
	mBallShotType = null;
	mBallShot = null;
	mBallShotDir = null;
	mBallShotSpeedScale = 1.0;
	mIsHeadShot = false;
	mDidLand = false;
	
	mIsFinished = true;
	mActionStartTime = 0.0;
	mJumpPhysics = null;
	mDidLand = false;
	
	/*******************
		Ball Handling
	********************/
	
	function ballControlOnConstructor() {
	}
	
	function ballControlOnStart(footballer, t, startState) {
	
		mBallControlState = BallControlState_None;
	}
	
	function ballControlOnStop(footballer, t) {
	
		footballer.clearTriggerVolumes();
	
		if (!footballer.impl.isShootingBall())
			footballer.impl.cancelBallControl();
	}
	
	function getBallShotDir(footballer) {
	
		local shotDir;
		local facingDir = footballer.impl.getFacing();

		if (mBallShotDir.isZero()) {
		
			shotDir = facingDir;
		
		} else {
		
			if (mBallShotDir.dot(facingDir) == 0.0) {
			
				shotDir = Vector3();
			
				lerpv3(facingDir, mBallShotDir, 0.5, shotDir);
				
				if (shotDir.dot(facingDir) > 0.0) {
				
					return shotDir;
				}
			} 
			
			if (mBallShotDir.dot(facingDir) > 0.0) {
			
				shotDir = mBallShotDir;
			
			} else {

				shotDir = facingDir;
			}
		}
		
		return shotDir;
	}
	
	function onBallSelfCollision(footballer, ball, boneID) { 
	
		if (mIsHeadShot && boneID == footballer.settings.BoneID_Head && mBallControlState != BallControlState_PostShot) 
			doHeader(footballer, ball); 
	}	
	
	function onBallPassiveSelfCollision(footballer, ball, boneID, isInFront) { 
	
		if (mIsHeadShot && isInFront && boneID == footballer.settings.BoneID_Head && mBallControlState != BallControlState_PostShot) 
			doHeader(footballer, ball); 
	}	
	
	function doHeader(footballer, ball) {
	
		//print("shootHeader");
	
		footballer.doHeaderShot(ball, mBallShot, mBallShot.speed * mBallShotSpeedScale, getBallShotDir(footballer), mJumpPhysics == null ? null : mJumpPhysics.velocity);
	
		mBallControlState = BallControlState_PostShot;	
	}
	
	
	function onBallTriggerVolume(footballer, ball, volumeID) {
	
		doHeader(footballer, ball);
	}
		
	function handleBallControlState(footballer, t, actionTime) {
	
		local isBallOwner = footballer.impl.isBallOwner();
		local repeat = true;
		
		while (repeat) {
	
			repeat = false;
	
			switch (mBallControlState) {
			
				case BallControlState_None: {
				
					repeat = true;
				
					if (mIsHeadShot) {
						
						footballer.addTriggerVolumeHead(0);
						mBallControlState = BallControlState_PreShot;
						
					} else {
				
						if (isBallOwner) {
						
							mBallControlState = BallControlState_PreShot;
							
						} else {
						
							mBallControlState = BallControlState_NotOwner;
						}
					}
				
				} break;
				
				case BallControlState_NotOwner: {
				
					if (isBallOwner) {
						
						repeat = true;
						
						mBallControlState = BallControlState_PreShot;
					}
				
				} break;
				
				case BallControlState_PreShot: {
				
					if (mIsHeadShot) {
						
						footballer.executeTriggerVolumesOnBall();
					
					} else {
				
						if (actionTime >= footballer.shoot.ShootPhase_StartTime) {

							if (isBallOwner) {
							
								local match = footballer.impl.getMatch();
								local ball = match.getBall();
								
								if (footballer.ballIsFootControlHeight()) {
								
									if (hasBallShot()) {
								
										footballer.shootBall(match.genBallCommandID(), ball, mBallShot, mBallShot.speed * mBallShotSpeedScale, getBallShotDir(footballer), null, 
																true, true, true, footballer.getSkillBallProtection(), footballer.getSkillBallControlBodyOrFeet(), false, footballer.settings.BallShotSwitchInfluence, footballer.settings.BallShotSoundName);

										mBallControlState = BallControlState_PostShot;
									}
								} 
							} 
						} 
					}
				
				} break;
				
				case BallControlState_PostShot: {
				} break;
			}
		
			
		}
	}
	
	mBallControlState = -1;
	
	static BallControlState_None = -1;
	static BallControlState_NotOwner = 0;
	static BallControlState_PreShot = 1;
	static BallControlState_PostShot = 2;
}