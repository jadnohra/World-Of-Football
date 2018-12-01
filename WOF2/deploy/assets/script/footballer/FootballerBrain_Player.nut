
class FootballerBrain_Player_DirTestItem {

	constructor(interv, x, y, z) {
	
		dir = Vector3();
		
		interval = interv.tofloat();
		dir.set3(x.tofloat(), y.tofloat(), z.tofloat());
	}
	
	interval = 0.0;
	dir = null;
}

class FootballerBrain_Player extends FootballerBrainBase {
	
	static DeadZone = 0.05;
	static TapInterval = 0.18;
	static ShotPowerBarCycleInterval = 0.5;
	static ShotPowerBarStartInterval = 0.2;
	ShotPowerBarCancelValue = 0.15;
	ShotPowerBarCoeff = 2.0;
	
	static InputID_None = -1;
	static InputID_Dir = 0;
	static InputID_Modifier = 1;
	static InputID_Low = 2;
	static InputID_High = 3;
	static InputID_Extra = 4;
	static InputID_Pass = 5;
	static InputID_SetDeadZone = 6;
	static InputID_Modifier2 = 7;
	static InputID_Test = 8;
	static InputID_Switch = 9;
	static InputID_Magic1 = 10;
	
	static InputClientName = "FootballerBrainPlayerInput";
	
	constructor() {
	
		constructInput();
	}	
	
	function load(footballer, chunk, conv) {

		local profile = footballer.mBody.mProfile;
	
		mTackleInputID = InputID_Pass;
		mTackleShot = profile.Shot_Pass;	
		
		ShotPowerBarCancelValue = profile.ShotPowerCoeffMin;
		ShotPowerBarCoeff = profile.ShotPowerCoeffMax;
		
		return true;
	}
	
	function start(footballer, t, controller) {
	
		if (!mIsStarted) {
		
			if (!initInput(footballer, controller, t)) {
			
				assrt("Failed To Create InputClient");
			}
		
			mIsStarted = true;
			mWasApplyingAfterTouch = false;
			mIsAllowedShotTriggering = false;
		}
		
		return true;
	}
	
	function stop(footballer, t) {
	
		footballer.mImpl.hideBar();
		mIsStarted = false;
	}
	
	function frameMove(footballer, t, dt) {

		updateInput(footballer, t);
		
		local inputHasDir = !(mInputDir.isZero());
		
		if (footballer.mBody.shouldApplyBallAfterTouch()) {
		
			local isActiveAfterTouch = (mWasApplyingAfterTouch || mInputDirChanged);
		
			if (mWasApplyingAfterTouch && !inputHasDir && footballer.mBody.mProfile.AfterTouchGiveUpIfIdled) {
			
				isActiveAfterTouch = false;
				footballer.mBody.giveUpBallAfterTouch();
			}
		
			if (isActiveAfterTouch) {
		
				footballer.mBody.requestIdle(null, t, dt);
		
				mWasApplyingAfterTouch = true;
				
				applyAfterTouch(footballer, mInputDir);
				
			} else {
			
				if (footballer.mBody.mProfile.EnableLimitAfterTouchReactionTime) {
				
					if (mAfterTouchReactionStartTime == 0.0) {
					
						mAfterTouchReactionStartTime = t;
						
					} else {
					
						if (t - mAfterTouchReactionStartTime > footballer.mBody.mProfile.AfterTouchTimeReactionTime) {
						
							//print("Giveup aftertouch");
							footballer.mBody.giveUpBallAfterTouch();
						}
					}
				}
			}
						
		} else {
		
			mAfterTouchReactionStartTime = 0.0;
		
			local resetWasApplyingAfterTouch = true;
			local actionProcessed = false;
			
			local isBallOwner = footballer.mImpl.isBallOwner();
			local isHeader = footballer.mBody.ballIsHeaderHeight();
			local actionType = footballer.mBody.getActionType();
			
			if (mIsAllowedShotTriggering && mTriggerShotUsePowerBar
				&& mTriggeringShotType != footballer.mBody.mProfile.Shot_None 
				&& (actionType == footballer.Action_Running || actionType == footballer.Action_Idle || actionType == footballer.Action_Turning || actionType == footballer.Action_Shooting)) {
			
				footballer.mImpl.showBar(mTriggeringShotHoldCycle.value, ShotPowerBarCancelValue);
				
			} else {
			
				footballer.mImpl.hideBar();
			}
			
			if (!actionProcessed && 
				(mTriggeringShotType == mTackleShot || mTriggeredShotType == mTackleShot)) {
			
				local doTackle = !isHeader && !isBallOwner;
				
				if (doTackle) {
				
					footballer.mBody.requestActionTackle(null, t, dt);
					actionProcessed = true;
				} 
			}
			
			if (!actionProcessed && mTriggeredShotType != footballer.mBody.mProfile.Shot_None) {

				local shotType = mTriggeredShotType;
				local shotLevel = 1;
				local shotCoeff = mTriggerShotUsePowerBar ? ShotPowerBarCoeff * mTriggeringShotHoldCycle.value : 1.0;
		
				local shot = isHeader ? footballer.mBody.mProfile.getHeadShot(shotLevel, shotType) : footballer.mBody.mProfile.getShot(shotLevel, shotType);
				
				local doShoot = (shot != null) && (isBallOwner || isHeader);
				
				if (doShoot) {

					footballer.mBody.requestShot(null, t, dt, isHeader, shotType, shot, false, shotCoeff);
					footballer.mBody.setShotDir(t, dt, null);
					actionProcessed = true;			
				}
			} 
						
			if (!actionProcessed && actionType == footballer.Action_Running) {
			
				if (mTriggeringShotType != footballer.mBody.mProfile.Shot_None) {
				
					footballer.mBody.getActionRunning().requestStrafe(footballer.mBody, null, true);
					
				} else {
				
					footballer.mBody.getActionRunning().requestStrafe(footballer.mBody, null, mInputModifier);
				}
			}
			
			if (!actionProcessed) {
			
				local applyDir = inputHasDir;
					
				if (applyDir) {	
					
					if (mWasApplyingAfterTouch && !mInputDirChanged) {
					
						applyDir = false;
						resetWasApplyingAfterTouch = false;
					}
				}
			
				switch (actionType) {
						
					case footballer.Action_Shooting: {
					
						footballer.mBody.setShotDir(t, dt, inputHasDir ? mInputDir : null);
					
					} break;
						
					case footballer.Action_Tackling: {
					
						footballer.mBody.getActionTackling().setIsBallControlTackle(readInputImmediateIsControlTackle());
						footballer.mBody.setShotDir(t, dt, inputHasDir ? mInputDir : null);
				
					} break;
					
					default: {
					
						if (applyDir) {
						
							local turnInPlace = false;
						
							if (mInputModifier2) {
							
								turnInPlace = true;
							
								/*
								if (actionType != footballer.Action_Running) {
								
									turnInPlace = true;
								}
								*/
							}
						
							if (turnInPlace) {
							
								footballer.mBody.requestZeroVelocity(null, t, dt);
								footballer.mBody.requestFacing(null, t, dt, mInputDir, false);
							
							} else {
						
								footballer.mBody.requestVelocity(null, t, dt, mInputDir, false, null, mInputModifier);
							}
						
						} else {
						
							footballer.mBody.requestIdle(null, t, dt);
						}
					
					}
				}
			}
		
			if (resetWasApplyingAfterTouch)
				mWasApplyingAfterTouch = false;
		}
	}
	
		
		
	function applyAfterTouch(footballer, dir) {
	
		footballer.mImpl.requestBallAfterTouch(0, dir, 
													footballer.mBody.mProfile.BallAfterTouchSideMag, 
													footballer.mBody.mProfile.BallAfterTouchUpMag, 
													footballer.mBody.mProfile.BallAfterTouchDownMag, 
													footballer.mBody.mProfile.BallAfterTouchRotSyncRatio, true);
	}
	
		
	function processInputShotEvent(source, t, shotType, isHeader) {
	
		local holdTime = readInputEndHoldTime(source, t);

		if (isHeader)
			return;
				
		if (mTriggeringShotType == shotType) {
	
			if (holdTime > 0.0) {
			
				mTriggeringShotHoldCycle.update(ShotPowerBarCycleInterval, holdTime);
			
				if (mTriggeringShotHoldCycle.value > ShotPowerBarCancelValue) {
				
					mTriggeredShotType = shotType;
				
				} else {
				
					if (mTriggeringShotHoldCycle.cycleIndex == 0) {
					
						mTriggeredShotType = shotType;
					} 
				}
			} 
		}
	}
	
	function readCurrInputShotState(t, inputID, shotType, isHeader, resetHoldTime) {
	
		local buttonComp = mInputClient.getSourceByID(inputID).getButton();
			
		if (buttonComp.getIsHolding()) {
		
			if (isHeader) {
		
				mTriggeringShotType = -1;
				mTriggeredShotType = shotType;
				mTriggerShotUsePowerBar = false;
		
			} else {
			
				local holdTime = buttonComp.getHoldTime(t, resetHoldTime);
				
				mTriggeringShotType = shotType;
				mTriggeringShotHoldCycle.update(ShotPowerBarCycleInterval, holdTime);
				
				if (mTriggeringShotHoldCycle.cycleIndex == 0 && holdTime <= ShotPowerBarStartInterval) {
				
					mTriggerShotUsePowerBar = false;
					
				} else {
				
					mTriggerShotUsePowerBar = true;
				}
			}
		}
	}
	
	function updateInput(footballer, t) {
	
		mInputDirChanged = false;
		mTriggeredShotType = footballer.mBody.mProfile.Shot_None;
		
		local isHeader = footballer.mBody.ballIsHeaderHeight();
		
		local wasAllowedShotTriggering = mIsAllowedShotTriggering;
		mIsAllowedShotTriggering = footballer.mImpl.isBallOwner();
						
		local ignoreTriggers = false;
		local source = mInputClient.startIterateEvents();
		
		while (source.isValid()) {

			local ID = mInputClient.getSourceID(source);
			
			switch (ID) {
			
				case InputID_Dir: {
				
					local match = footballer.mImpl.getMatch();
				
					mInputDirChanged = true;
					readInputDir(footballer, source, match.SceneFwdAxis(), match.SceneRightAxis(), true, mInputDir);
				
				} break;
			
				case InputID_Modifier: {
				
					mInputModifier = (source.getGeneric1D().getValue() != 0.0);
				
				} break;
				
				case InputID_Modifier2: {
				
					mInputModifier2 = (source.getGeneric1D().getValue() != 0.0);
				
				} break;
				
				case InputID_Low: {
				
					processInputShotEvent(source, t, footballer.mBody.mProfile.Shot_Low, isHeader);
								
				} break;
				
				case InputID_High: {
				
					processInputShotEvent(source, t, footballer.mBody.mProfile.Shot_High, isHeader);
								
				} break;
				
				case InputID_Extra: {
				
					processInputShotEvent(source, t, footballer.mBody.mProfile.Shot_Extra, isHeader);
								
				} break;
				
				case InputID_Pass: {
				
					processInputShotEvent(source, t, footballer.mBody.mProfile.Shot_Pass, isHeader);
								
				} break;
				
				case InputID_SetDeadZone: {
				
					if (source.getGeneric1D().getValue() != 0.0) {
					
						ignoreTriggers = true;
						mInputClient.getSourceByID(InputID_Dir).getDir2D().setRawValuesAsDeadZones(true, true);
						
						{
							local dir2D = mInputClient.getSourceByID(InputID_Dir);
							local dir2DComp = dir2D.getDir2D();
							local curve = dir2DComp.getResponseCurve1();
							log("SetDeadZone: " + curve.getDeadZone());
						}
					}
				
				} break;
				
				case InputID_Switch: {
				
					if (source.getGeneric1D().getValue() != 0.0) {
					
						footballer.mImpl.requestManualSwitch();
					}
				
				} break;
				
				case InputID_Magic1: {
				
					if (source.getGeneric1D().getValue() != 0.0) {
					
						footballer.mImpl.getMatch().setMagicRequestingBallFootballer(footballer.mImpl);
					}
				
				} break;

				/*
				case InputID_Test: {
				
					if (source.getGeneric1D().getValue() != 0.0) {
				
						if (mDirTestItems != null && mDirTestItems.len() > 0) {
						
							if (mDirTestIndex < 0) {
							
								mDirTestIndex = 0;	
								mDirTestTime = t;
								
							} else {
							
								mDirTestIndex = -1;	
							}
						}
					}
				
				} break;
				*/
				
			}
			
			/*
			if (ID == InputID_Test) {
			
				mTriggeredShotType = footballer.mBody.mProfile.Shot_None;
				mInputClient.breakIterateEvents();
				break;
			}
			*/
			
					
			source = mInputClient.nextEvent();
		}
				
		
		/*
		if (mDirTestIndex >= 0) {
		
			local testItem = mDirTestItems[mDirTestIndex];
		
			if (t - mDirTestTime >= testItem.interval) {
			
				mDirTestTime = t;
			
				mInputDirChanged = true;
				mInputDir.set(testItem.dir);
				
				mDirTestIndex += 1;
			
				if (mDirTestIndex >= mDirTestItems.len()) {
				
					if (mDirTestLoop)
						mDirTestIndex = 0;
					else	
						mDirTestIndex = -1;
				}
			}
		}
		*/
		
		
		mTriggeringShotType = footballer.mBody.mProfile.Shot_None;
		
		if (mTriggeredShotType == footballer.mBody.mProfile.Shot_None) {
		
			local resetHoldTime = mIsAllowedShotTriggering && !wasAllowedShotTriggering;
		
			if (mTriggeringShotType == footballer.mBody.mProfile.Shot_None) {
			
				readCurrInputShotState(t, InputID_Low, footballer.mBody.mProfile.Shot_Low, isHeader, resetHoldTime);
			}
			
			if (mTriggeringShotType == footballer.mBody.mProfile.Shot_None) {
			
				readCurrInputShotState(t, InputID_High, footballer.mBody.mProfile.Shot_High, isHeader, resetHoldTime);
			}
			
			if (mTriggeringShotType == footballer.mBody.mProfile.Shot_None) {
			
				readCurrInputShotState(t, InputID_Extra, footballer.mBody.mProfile.Shot_Extra, isHeader, resetHoldTime);
			}
			
			if (mTriggeringShotType == footballer.mBody.mProfile.Shot_None) {
			
				readCurrInputShotState(t, InputID_Pass, footballer.mBody.mProfile.Shot_Pass, isHeader, resetHoldTime);
			}
		}
	}
	
	function readInputImmediateIsControlTackle() {
	
		local source = mInputClient.getSourceByID(mTackleInputID).getBaseSource();
		
		return source.getGeneric1D().getValue() != 0.0;
	}
	
	function readInputHasTappingShots() {
	
		return mInputClient.getSourceByID(InputID_Low).getButton().getIsTapping()
							|| mInputClient.getSourceByID(InputID_High).getButton().getIsTapping()
							|| mInputClient.getSourceByID(InputID_Extra).getButton().getIsTapping()
							|| mInputClient.getSourceByID(InputID_Pass).getButton().getIsTapping();
	
	}
	
		
	function readInputDir(footballer, source, axis1, axis2, camRel, inOutDir) {
	
		local dirComp = source.getDir2D();
		
		axis1.mul(dirComp.getValue1(), inOutDir);
		axis2.mulAndAdd(dirComp.getValue2(), inOutDir);
		
		if (camRel)
			footballer.mImpl.camRelativizeInPlace(inOutDir, true, false);
			
		return !(inOutDir.isZero());
	}
	
	function readInputEndTapCount(source, t) {
	
		local buttonComp = source.getButton();
		
		if (!buttonComp.getIsTapping()) {
		
			return buttonComp.getTapCount(true);
		}
		
		return -1;	
	}
	
	function readInputEndHoldTime(source, t) {
	
		local buttonComp = source.getButton();
		
		if (!buttonComp.getIsHolding()) {
		
			return buttonComp.getHoldTime(t, true);
		}
		
		return -1.0;	
	}
	
	
	function initInput(footballer, controller, t) {
	
		mInputClient = controller.getInputClient();
		
		if (mInputClient == null || !mInputClient.isValid() || !(mInputClient.getName() == InputClientName)) {
		
			mInputClient = createInputClient(footballer, controller, t);
			
			if (mInputClient == null) {
			
				//create dummy client
				mInputClient = CInputClient();
				mInputClient.create();
			
				return false;
			}
		
			controller.setInputClient(mInputClient);
		}
		
		mInputDirChanged = false;
		
		return true;
	}
	
	function createInputClient(footballer, controller, t) {
	
		local manager = controller.getManager();
		
		if (manager == null || !manager.isValid())
			return null;
	
		local inputClient = CInputClient();
		inputClient.create(manager);
		inputClient.setName(InputClientName);
			
		{
			local fwd = controller.getSource("FwdAxis", t);
			if (!fwd.isValid()) { assrt("FwdAxis not in Controller"); return null; }
		
			local right = controller.getSource("RightAxis", t);
			if (!fwd.isValid()) { assrt("RightAxis not in Controller"); return null; }			
			
			local dir2D = CInputSource.createDir2DFrom2Axis(manager, fwd, right, "dir");
			if (!dir2D.isValid()) { assrt("Failed to create InputID_Dir"); return null; }
			
			local deadZone = controller.getDeadZone();

			if (deadZone < 0.0) {
			
				deadZone = DeadZone;
			}
			
			local dir2DComp = dir2D.getDir2D();
						
			{
				local curve = dir2DComp.createResponseCurve1();
				curve.setDeadZone(deadZone);
			}
			
			{
				local curve = dir2DComp.createResponseCurve2();
				curve.setDeadZone(deadZone);
			}
			
			if (!inputClient.addMappedSource(dir2D, InputID_Dir, true)) { assrt("Failed to add InputID_Dir"); return null; }
		}
		
		{
			local source = controller.getSource("Modifier", t);
			if (!source.isValid()) { assrt("Modifier not in Controller"); return null; }
		
			if (!inputClient.addMappedSource(source, InputID_Modifier, false)) { assrt("Failed to add InputID_Modifier"); return null; }
		}
		
		{
			local source = controller.getSource("Modifier2", t);
			
			if (source.isValid()) {
		
				if (!inputClient.addMappedSource(source, InputID_Modifier2, false)) { assrt("Failed to add InputID_Modifier2"); return null; }
			}
		}
		
		{
			local source = controller.getSource("ShotLow", t);
			if (!source.isValid()) { assrt("ShotLow not in Controller"); return null; }
		
			local button = source.createButton(manager, "ShotLowButton");
			if (!button.isValid()) { assrt("ShotLowButton couldn't be created"); return null; }
			
			local buttonComp = button.getButton();
			
			{
				buttonComp.setMode(false, true, false);
				buttonComp.setTapInterval(TapInterval);
				buttonComp.setTapCountLimit(footballer.mBody.mProfile.ShotLevelCount);
			}
		
			if (!inputClient.addMappedSource(button, InputID_Low, true)) { assrt("Failed to add InputID_Low"); return null; }
		}
		
		{
			local source = controller.getSource("ShotHigh", t);
			if (!source.isValid()) { assrt("ShotHigh not in Controller"); return null; }
		
			local button = source.createButton(manager, "ShotHighButton");
			if (!button.isValid()) { assrt("ShotHighButton couldn't be created"); return null; }
			
			local buttonComp = button.getButton();
			
			{
				buttonComp.setMode(false, true, false);
				buttonComp.setTapInterval(TapInterval);
				buttonComp.setTapCountLimit(footballer.mBody.mProfile.ShotLevelCount);
			}
		
			if (!inputClient.addMappedSource(button, InputID_High, true)) { assrt("Failed to add InputID_High"); return null; }
		}
		
		{
			local source = controller.getSource("ShotExtra", t);
			if (!source.isValid()) { assrt("ShotExtra not in Controller"); return null; }
		
			local button = source.createButton(manager, "ShotExtraButton");
			if (!button.isValid()) { assrt("ShotExtraButton couldn't be created"); return null; }
			
			local buttonComp = button.getButton();
			
			{
				buttonComp.setMode(false, true, false);
				buttonComp.setTapInterval(TapInterval);
				buttonComp.setTapCountLimit(footballer.mBody.mProfile.ShotLevelCount);
			}
		
			if (!inputClient.addMappedSource(button, InputID_Extra, true)) { assrt("Failed to add InputID_Extra"); return null; }
		}
		
		{
			local source = controller.getSource("Pass", t);
			if (!source.isValid()) { assrt("Pass not in Controller"); return null; }
		
			local button = source.createButton(manager, "PassButton");
			if (!button.isValid()) { assrt("PassButton couldn't be created"); return null; }
			
			local buttonComp = button.getButton();
			
			{
				buttonComp.setMode(false, true, false);
				buttonComp.setTapInterval(TapInterval);
				buttonComp.setTapCountLimit(footballer.mBody.mProfile.ShotLevelCount);
			}
		
			if (!inputClient.addMappedSource(button, InputID_Pass, true)) { assrt("Failed to add InputID_Pass"); return null; }
		}
		
		{
			local source = controller.getSource("SetDeadZone", t);
			
			if (source.isValid()) {
		
				if (!inputClient.addMappedSource(source, InputID_SetDeadZone, false)) { assrt("Failed to add InputID_SetDeadZone"); return null; }
			}
		}
		
		{
			local source = controller.getSource("Switch", t);
			
			if (source.isValid()) {
		
				if (!inputClient.addMappedSource(source, InputID_Switch, false)) { assrt("Failed to add InputID_Switch"); return null; }
			}
		}
		
		{
			local source = controller.getSource("Magic1", t);
			
			if (source.isValid()) {
		
				inputClient.addMappedSource(source, InputID_Magic1, false);
			}
		}
		
		{
			local key1 = controller.getSource("ShotHigh", t);
			local key2 = controller.getSource("ShotExtra", t);
			
			local keyTest = CInputSource.createAnd(manager, key1, key2, "Test");
			if (!keyTest.isValid()) { assrt("Failed to create InputID_Test"); return null; }
			
			if (!inputClient.addMappedSource(keyTest, InputID_Test, false)) { assrt("Failed to add InputID_Test"); return null; }
		}
	
		return inputClient;
	}
	
	function constructInput() {
	
		mInputDir = Vector3();
		mInputDir.zero();
		mInputDirChanged = false;
		mTriggeringShotHoldCycle = CInputHoldCycle();
	}
			
	mIsStarted = false;
	mInputClient = null;
	mWasApplyingAfterTouch = false;
	mAfterTouchReactionStartTime = 0.0;
	
	mInputDirChanged = false;
	mInputDir = null;
	
	mInputModifier = false;
	mInputModifier2 = false;
	
	mTriggeredShotType = -1;
	mTriggeringShotType = -1;
	mTriggerShotUsePowerBar = false;
	mIsAllowedShotTriggering = false;
	mTriggeringShotHoldCycle = null;
	
	mTackleInputID = -1;
	mTackleShot = -1;
	
	/*
	mDirTestIndex = -1;
	mDirTestTime = 0.0;
	mDirTestLoop = false;
	mDirTestItems = null;
	*/
					
	/*
		Jitter
		
		FootballerBrain_Player_DirTestItem(0.0999756,-0.745331,0.0,-0.666695),
		FootballerBrain_Player_DirTestItem(0.0999756,-0.737027,0.0,-0.675863),
		FootballerBrain_Player_DirTestItem(0.0999756,-0.745331,0.0,-0.666695),
		FootballerBrain_Player_DirTestItem(0.0999756,-0.737027,0.0,-0.675863),
		FootballerBrain_Player_DirTestItem(0.0666504,-0.745331,0.0,-0.666695),
		FootballerBrain_Player_DirTestItem(0.0999756,-0.737027,0.0,-0.675863),
		FootballerBrain_Player_DirTestItem(0.0666504,-0.745331,0.0,-0.666695),
		FootballerBrain_Player_DirTestItem(0.0999756,-0.737027,0.0,-0.675863),
		FootballerBrain_Player_DirTestItem(0.0999756,-0.745331,0.0,-0.666695),
		FootballerBrain_Player_DirTestItem(0.0666504,-0.737027,0.0,-0.675863),
		FootballerBrain_Player_DirTestItem(0.0666504,-0.745331,0.0,-0.666695),
		FootballerBrain_Player_DirTestItem(0.0999756,-0.737027,0.0,-0.675863),
		FootballerBrain_Player_DirTestItem(0.0999756,-0.745331,0.0,-0.666695),
		FootballerBrain_Player_DirTestItem(0.233276,-0.737027,0.0,-0.675863),
		FootballerBrain_Player_DirTestItem(0.0999756,-0.745331,0.0,-0.666695),
		FootballerBrain_Player_DirTestItem(0.0666504,-0.737027,0.0,-0.675863)
	*/	
}
