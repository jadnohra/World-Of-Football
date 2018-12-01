
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
	static ShotPowerBarCancelValue = 0.15;
	static ShotPowerBarStartInterval = 0.2;
	
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
	
	static InputClientName = "BrainPlayerInput";
	
	constructor() {
	
		constructInput();
	}	
	
	function load(footballer, chunk, conv) {

		mTackleInputID = InputID_Pass;
		mTackleShot = footballer.Shot_Pass;	
		
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
			mInputModifier = false;
			mInputModifier2 = false;
		}
		
		return true;
	}
	
	function stop(footballer, t) {
	
		footballer.impl.hideBar();
		mIsStarted = false;
	}
	
	function isAskingForManualSwitch(footballer) {
	
		return false; //(mInterpreter.getShotType() == footballer.Shot_Pass) && mInterpreter.getShotTypeChanged();
	}

	function frameMove(footballer, t, dt) {
	
		updateInput(footballer, t);
		
		local inputHasDir = !(mInputDir.isZero());
		
		if (footballer.shouldApplyBallAfterTouch()) {
		
			local isActiveAfterTouch = mWasApplyingAfterTouch || mInputDirChanged;
		
			if (isActiveAfterTouch) {
		
				footballer.getAction().switchIdle(footballer, t, null);
		
				mWasApplyingAfterTouch = true;
				
				applyAfterTouch(footballer, mInputDir);
				
			} else {
			
				if (footballer.settings.EnableLimitAfterTouchReactionTime) {
				
					if (mAfterTouchReactionStartTime == 0.0) {
					
						mAfterTouchReactionStartTime = t;
						
					} else {
					
						if (t - mAfterTouchReactionStartTime > footballer.settings.AfterTouchTimeReactionTime) {
						
							footballer.giveUpBallAfterTouch();
						}
					}
				}
			}
						
		} else {
		
			mAfterTouchReactionStartTime = 0.0;
		
			local resetWasApplyingAfterTouch = true;
			local actionProcessed = false;
			
			local isBallOwner = footballer.impl.isBallOwner();
			local isHeader = footballer.ballIsHeaderHeight();
			local actionType = footballer.getActionType();
			
			if (mIsAllowedShotTriggering && mTriggerShotUsePowerBar
				&& mTriggeringShotType != footballer.Shot_None 
				&& (actionType == footballer.Action_Running || actionType == footballer.Action_Idle || actionType == footballer.Action_Turning || actionType == footballer.Action_Shooting)) {
			
				footballer.impl.showBar(mTriggeringShotHoldCycle.value, ShotPowerBarCancelValue);
				
			} else {
			
				footballer.impl.hideBar();
			}
			
			if (!actionProcessed && 
				(mTriggeringShotType == mTackleShot || mTriggeredShotType == mTackleShot)) {
			
				local doTackle = !isHeader && !isBallOwner;
				
				if (doTackle) {
				
					updateActionTackling(footballer, t, readInputImmediateIsControlTackle(), inputHasDir ? mInputDir : null);
					actionProcessed = true;
				} 
			}
			
			if (!actionProcessed && mTriggeredShotType != footballer.Shot_None) {

				local shotType = mTriggeredShotType;
				local shotLevel = 1;
				local shotCoeff = mTriggerShotUsePowerBar ? 2.0 * mTriggeringShotHoldCycle.value : 1.0;
		
				local shot = isHeader ? footballer.settings.getHeadShot(shotLevel, shotType) : footballer.settings.getShot(shotLevel, shotType);
				
				local doShoot = (shot != null) && (isBallOwner || isHeader);
				
				if (doShoot) {

					switchActionShooting(footballer, t, isHeader, shotType, shot, false, shotCoeff);
					actionProcessed = true;			
				}
			} 
						
			if (!actionProcessed && actionType == footballer.Action_Running) {
			
				if (mTriggeringShotType != footballer.Shot_None) {
				
					footballer.getActionRunning().setIsStrafing(true);
					
				} else {
				
					footballer.getActionRunning().setIsStrafing(mInputModifier);
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
					
						updateActionShooting(footballer, t, inputHasDir ? mInputDir : null);
					
					} break;
						
					case footballer.Action_Tackling: {
					
						updateActionTackling(footballer, t, readInputImmediateIsControlTackle(), inputHasDir ? mInputDir : null);
					
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
							
								updateActionTurning(footballer, t, mInputDir);
							
							} else {
						
								updateActionRunning(footballer, t, mInputDir, mInputModifier, null);
							}
						
						} else {
						
							footballer.getAction().switchIdle(footballer, t, null);
						}
					
					}
				}
			}
		
			if (resetWasApplyingAfterTouch)
				mWasApplyingAfterTouch = false;
		}
	}
	
	function switchActionShooting(footballer, t, isHeader, shotType, shot, copyShot, shotCoeff) {
	
		local actionType = footballer.getActionType();
		
		if (actionType == footballer.Action_Running && !isHeader) {
		
			footballer.getActionRunning().setActiveBallShot(shot, shotCoeff, copyShot, false);
		
		} else if (actionType != footballer.Action_Shooting) {
				
			footballer.getAction().switchShooting(footballer, t, footballer.getActionShooting().createStartState(footballer, isHeader, shotType, shot, copyShot, shotCoeff));
		} 
	}
	
	function updateActionShooting(footballer, t, dir) {
	
		local actionType = footballer.getActionType();
	
		if (actionType == footballer.Action_Running) {

			//do nothing
		
		} else if (actionType == footballer.Action_Shooting) {
				
			footballer.getActionShooting().setBallShotDirection(footballer, dir);
		} 
	}
	
	function updateActionTackling(footballer, t, isBallControlTackle, tackleDir) {
	
		local action = footballer.getActionTackling();
	
		if (footballer.getActionType() != footballer.Action_Tackling) {
				
			footballer.getAction().switchTackling(footballer, t, action.createStartState(footballer, isBallControlTackle));
			
		} else {
			
			action.setBallTackleDirection(footballer, tackleDir);
			action.setIsBallControlTackle(isBallControlTackle);
		}
	}
	
	function updateActionRunning(footballer, t, runDir, isStrafing, speedType) {
	
		local action = footballer.getActionRunning();
	
		if (footballer.getActionType() != footballer.Action_Running) {
				
			footballer.getAction().switchRunning(footballer, t, action.createStartState(isStrafing, runDir, speedType));
			
		} else {
			
			action.setIsStrafing(isStrafing);
			if (!isStrafing)
				action.setBodyFacing(footballer, runDir);
			action.setRunDir(footballer, runDir);
			action.setRunSpeed(footballer, speedType);
		}
	}
	
	function updateActionTurning(footballer, t, faceDir) {
	
		if (footballer.getActionType() != footballer.Action_Turning) {
				
			footballer.getAction().switchTurning(footballer, t, null);
			
		} else {
			
			footballer.getActionTurning().setBodyFacing(footballer, faceDir);
		}
	}
	
	function applyAfterTouch(footballer, dir) {
	
		footballer.impl.requestBallAfterTouch(0, dir, 
													footballer.settings.BallAfterTouchSideMag, 
													footballer.settings.BallAfterTouchUpMag, 
													footballer.settings.BallAfterTouchDownMag, 
													footballer.settings.BallAfterTouchRotSyncRatio, true);
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
		mTriggeredShotType = footballer.Shot_None;
		
		local isHeader = footballer.ballIsHeaderHeight();
		
		local wasAllowedShotTriggering = mIsAllowedShotTriggering;
		mIsAllowedShotTriggering = footballer.impl.isBallOwner();
						
		local ignoreTriggers = false;
		local source = mInputClient.startIterateEvents();
		
		while (source.isValid()) {

			local ID = mInputClient.getSourceID(source);
			
			switch (ID) {
			
				case InputID_Dir: {
				
					local match = footballer.impl.getMatch();
				
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
				
					processInputShotEvent(source, t, footballer.Shot_Low, isHeader);
								
				} break;
				
				case InputID_High: {
				
					processInputShotEvent(source, t, footballer.Shot_High, isHeader);
								
				} break;
				
				case InputID_Extra: {
				
					processInputShotEvent(source, t, footballer.Shot_Extra, isHeader);
								
				} break;
				
				case InputID_Pass: {
				
					processInputShotEvent(source, t, footballer.Shot_Pass, isHeader);
								
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
			
				mTriggeredShotType = footballer.Shot_None;
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
		
		
		mTriggeringShotType = footballer.Shot_None;
		
		if (mTriggeredShotType == footballer.Shot_None) {
		
			local resetHoldTime = mIsAllowedShotTriggering && !wasAllowedShotTriggering;
		
			if (mTriggeringShotType == footballer.Shot_None) {
			
				readCurrInputShotState(t, InputID_Low, footballer.Shot_Low, isHeader, resetHoldTime);
			}
			
			if (mTriggeringShotType == footballer.Shot_None) {
			
				readCurrInputShotState(t, InputID_High, footballer.Shot_High, isHeader, resetHoldTime);
			}
			
			if (mTriggeringShotType == footballer.Shot_None) {
			
				readCurrInputShotState(t, InputID_Extra, footballer.Shot_Extra, isHeader, resetHoldTime);
			}
			
			if (mTriggeringShotType == footballer.Shot_None) {
			
				readCurrInputShotState(t, InputID_Pass, footballer.Shot_Pass, isHeader, resetHoldTime);
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
			footballer.impl.camRelativizeInPlace(inOutDir, true, false);
			
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
				buttonComp.setTapCountLimit(footballer.settings.ShotLevelCount);
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
				buttonComp.setTapCountLimit(footballer.settings.ShotLevelCount);
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
				buttonComp.setTapCountLimit(footballer.settings.ShotLevelCount);
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
				buttonComp.setTapCountLimit(footballer.settings.ShotLevelCount);
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
