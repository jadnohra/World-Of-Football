class FootballerBrain_AI_GoalFace extends FootballerBrain_AI_Goal {

	constructor() {
	
		mFaceDir = Vector3();
	}
	
	function setTargetByDir(footballer, dir, needsFixing) {
	
		mFaceDir.set(dir);
		
		if (needsFixing) 
			footballer.helperFixDir(mFaceDir);
		 
	}
	
	function setTargetByPos(footballer, pos) {
	
		mFaceDir.set(pos);
		mFaceDir.subtractFromSelf(footballer.impl.getPos());
		footballer.helperFixDir(mFaceDir);
	}
	
	function start(brain, footballer, t) { 
	
		return doReturn(t, mFaceDir.isZero() ? Result_Success : Result_Processing);	
	}
	
	function isWithinTolerance(footballer, dir) {
	
		return (footballer.impl.getFacing().dot(dir) >= 0.995);
	}
		
	function frameMove(brain, footballer, t, dt) { 
		
		local impl = footballer.impl;
		
		if (mFaceDir.isZero() || isWithinTolerance(footballer, mFaceDir)) {
		
			brain.body_idle(footballer, t);
			return doReturn(t, Result_Success);
		} 
		
		if (footballer.getActionType() == footballer.Action_Running) {

			if (impl.getCollState() == impl.Coll_Blocked) {
				
				brain.body_idle(footballer, t);
				mState = Result_Processing;
				
			} else {
			
				footballer.getActionRunning().setBodyFacing(footballer, mFaceDir);
				footballer.getActionRunning().setRunDir(footballer, mFaceDir);
			}
		
		} else {
		
			footballer.getAction().switchTurning(footballer, t, null);
			footballer.getActionTurning().setBodyFacing(footballer, mFaceDir);
		}
		
		return doReturn(t, Result_Processing);	
	}
	
	mFaceDir = null;
}