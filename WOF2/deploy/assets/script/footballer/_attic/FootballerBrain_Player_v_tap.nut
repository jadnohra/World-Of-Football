
class FootballerBrain_Player {
	
	static TapInterval = 0.18;
	static ShotPowerBarCycleInterval = 0.5;
	static ShotPowerBarCancelValue = 0.1;
	
	static InputID_None = -1;
	static InputID_Dir = 0;
	static InputID_Modifier = 1;
	static InputID_Low = 2;
	static InputID_High = 3;
	static InputID_Extra = 4;
	static InputID_SetDeadZone = 5;
	
	static InputClientName = "BrainPlayerInput";
	
	constructor() {
		
		constructInput();
	}	

	function load(footballer, chunk, conv) {
			
		return true;
	}
	
	function start(footballer, t, controller) {
	
		if (!mIsStarted) {
		
			if (!initInput(footballer, controller, t)) {
			
				assrt("Failed To Create InputClient");
			}
		
			mIsStarted = true;
			mWasApplyingAfterTouch = false;
		}
		
		return true;
	}
	
	function stop(footballer, t) {
	
		mIsStarted = false;
	}
	
	function isAskingForManualSwitch(footballer) {
	
		return false; //(mInterpreter.getShotType() == footballer.Shot_Pass) && mInterpreter.getShotTypeChanged();
	}

	function frameMove(footballer, t, dt) {
	
		updateInput(footballer, t);
		
		local inputHasDir = !(mInputDir.isZero());
		
		if (footballer.shouldApplyBallAfterTouch()) {
		
			footballer.getAction().switchIdle(footballer, t, null);
		
			local isActiveAfterTouch = mWasApplyingAfterTouch || mInputDirChanged;
		
			if (isActiveAfterTouch) {
		
				mWasApplyingAfterTouch = true;
				
				applyAfterTouch(footballer, mInputDir);
			} 
						
		} else {
		
			local resetWasApplyingAfterTouch = true;
			local actionProcessed = false;
			
			local isBallOwner = footballer.impl.isBallOwner();
			local isHeader = footballer.ballIsHeaderHeight();
			local actionType = footballer.getActionType();
			
			if (mHasTriggeredShot) {

				local shotType;
				local tapCount = 0;
				local shotLevel;
				local shotCoeff = 1.0;
		
				{
					local arg = FuncRefArg1();
					arg.arg1 = 1;
					
					shotType = readInputShotState(footballer, arg);
					tapCount = arg.arg1;
					shotLevel = footballer.settings.tapCountToShotLevel(tapCount, shotType, isHeader);
				}
				
				local shot = isHeader ? footballer.settings.getHeadShot(shotLevel, shotType) : footballer.settings.getShot(shotLevel, shotType);
				
				local doShoot = (shot != null) && (isBallOwner || isHeader);
				
				if (doShoot) {

					switchActionShooting(footballer, t, isHeader, shotType, shot, false, shotCoeff);
					actionProcessed = true;			
					
				}
			} 
			
			if (!actionProcessed && mHasTriggeredTackle) {
			
				local doTackle = !isHeader && !isBallOwner;
				
				if (doTackle) {
				
					updateActionTackling(footballer, t, mInputImmediateTackle, inputHasDir ? mInputDir : null);
					actionProcessed = true;
				} 
			}
			
			if (!actionProcessed && actionType == footballer.Action_Running) {
			
				if (mHasTappingShots) {
				
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
					
						updateActionTackling(footballer, t, mInputImmediateTackle, inputHasDir ? mInputDir : null);
					
					} break;
					
					default: {
					
						if (applyDir) {
						
							updateActionRunning(footballer, t, mInputDir, mInputModifier, null);
						
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
	
	function applyAfterTouch(footballer, dir) {
	
		footballer.impl.requestBallAfterTouch(0, dir, 
													footballer.settings.BallAfterTouchSideMag, 
													footballer.settings.BallAfterTouchUpMag, 
													footballer.settings.BallAfterTouchDownMag, 
													footballer.settings.BallAfterTouchRotSyncRatio, true);
	}
	
	function readInputShotState(footballer, argShotLevel) {
	
		local shotType = footballer.Shot_None;
		
		if (mHasTriggeredShot) {
		
			if (mLowTaps > 0) {
			
				shotType = footballer.Shot_Low;
				argShotLevel.arg1 = mLowTaps;
				
			} else if (mHighTaps > 0) {
			
				shotType = footballer.Shot_High;
				argShotLevel.arg1 = mHighTaps;
				
			} else if (mExtraTaps > 0) {
			
				shotType = footballer.Shot_Extra;
				argShotLevel.arg1 = mExtraTaps;
			}
		}
		
		return shotType;
	}
	
	function updateInput(footballer, t) {
	
		mInputDirChanged = false;
		mHasTriggeredShot = false;
		mHasTriggeredTackle = false;
	
		mLowTaps = 0;
		mHighTaps = 0;
		mExtraTaps = 0;
	
		mInputImmediateTackle = readInputImmediateIsControlTackle();
		mHasTappingShots = readInputHasTappingShots();
	
	
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
				
				case InputID_Low: {
				
					local tapCount = readInputEndTapCount(source, t);
					
					if (tapCount > 0) {
					
						mHasTriggeredShot = true;
						mLowTaps = tapCount;
						
						if (mLowTaps == 1)
							mHasTriggeredTackle = true;
					} 
								
				} break;
				
				case InputID_High: {
				
					local tapCount = readInputEndTapCount(source, t);
					
					if (tapCount > 0) {
					
						mHasTriggeredShot = true;
						mHighTaps = tapCount;
					} 
								
				} break;
				
				case InputID_Extra: {
				
					local tapCount = readInputEndTapCount(source, t);
					
					if (tapCount > 0) {
					
						mHasTriggeredShot = true;
						mExtraTaps = tapCount;
					} 
								
				} break;
				
				case InputID_SetDeadZone: {
				
					if (source.getGeneric1D().getValue() != 0.0) {
					
						ignoreTriggers = true;
						mInputClient.getSourceByID(InputID_Dir).getDir2D().setRawValuesAsDeadZones(true, true);
					}
				
				} break;
			}
		
			source = mInputClient.nextEvent();
		}
		
		if (ignoreTriggers) {
		
			mHasTriggeredShot = false;
			mHasTriggeredTackle = false;
		}
	}
	
	function readInputImmediateIsControlTackle() {
	
		local source = mInputClient.getSourceByID(InputID_Low).getBaseSource();
		
		return source.getGeneric1D().getValue() != 0.0;
	}
	
	function readInputHasTappingShots() {
	
		return mInputClient.getSourceByID(InputID_Low).getButton().getIsTapping()
							|| mInputClient.getSourceByID(InputID_High).getButton().getIsTapping()
							|| mInputClient.getSourceByID(InputID_Extra).getButton().getIsTapping();
	
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
			
			local dir2DComp = dir2D.getDir2D();
						
			{
				local curve = dir2DComp.createResponseCurve1();
				curve.setDeadZone(0.1);
			}
			
			{
				local curve = dir2DComp.createResponseCurve2();
				curve.setDeadZone(0.1);
			}
			
			if (!inputClient.addMappedSource(dir2D, InputID_Dir, true)) { assrt("Failed to add InputID_Dir"); return null; }
		}
		
		{
			local source = controller.getSource("Modifier", t);
			if (!source.isValid()) { assrt("Modifier not in Controller"); return null; }
		
			if (!inputClient.addMappedSource(source, InputID_Modifier, false)) { assrt("Failed to add InputID_Modifier"); return null; }
		}
		
		{
			local source = controller.getSource("ShotLow", t);
			if (!source.isValid()) { assrt("ShotLow not in Controller"); return null; }
		
			local button = source.createButton(manager, "ShotLowButton");
			if (!button.isValid()) { assrt("ShotLowButton couldn't be created"); return null; }
			
			local buttonComp = button.getButton();
			
			{
				buttonComp.setMode(false, false, true);
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
				buttonComp.setMode(false, false, true);
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
				buttonComp.setMode(false, false, true);
				buttonComp.setTapInterval(TapInterval);
				buttonComp.setTapCountLimit(footballer.settings.ShotLevelCount);
			}
		
			if (!inputClient.addMappedSource(button, InputID_Extra, true)) { assrt("Failed to add InputID_Extra"); return null; }
		}
		
		{
			local key1 = controller.getSource("ShotLow", t);
			local key2 = controller.getSource("ShotExtra", t);
			
			local setDeadZone = CInputSource.createAnd(manager, key1, key2, "setDeadZone");
			if (!setDeadZone.isValid()) { assrt("Failed to create InputID_SetDeadZone"); return null; }
			
			if (!inputClient.addMappedSource(setDeadZone, InputID_SetDeadZone, false)) { assrt("Failed to add InputID_SetDeadZone"); return null; }
		}
	
		return inputClient;
	}
	
	function constructInput() {
	
		mInputDir = Vector3();
		mInputDir.zero();
		mInputDirChanged = false;
	}
			
	mIsStarted = false;
	mInputClient = null;
	mWasApplyingAfterTouch = false;
	
	mInputDirChanged = false;
	mInputDir = null;
	
	mInputModifier = false;
	
	mInputImmediateTackle = false;
	mHasTriggeredShot = false;
	mHasTriggeredTackle = false;
	mHasTappingShots = false;
	mLowTaps = 0;
	mHighTaps = 0;
	mExtraTaps = 0;
}
