
class BallCommandManager {

	static SkillRandomAmount = 0.2;
	
	function load(pThis, chunk, conv) {
	
		return true;
	}

	function tryResolveFootballerConflict(cmd1, cmd2) {
	
		local footballer1 = cmd1.getSource().toFootballer();
		local footballer2 = cmd2.getSource().toFootballer();
		
		if (footballer1 == footballer2) {
			
			local diffID = cmd2.getID() - cmd1.getID();
		
			if (diffID > 0)
				cmd1.invalidateAndNotify()
			else if (diffID < 0)
				cmd2.invalidateAndNotify()
			else
				assrt("Double Command");			
				
			return;	
		}
		
		//local skillDiff = footballer1.AttributeBallControlFeet - footballer2.AttributeBallControlFeet;
		local skillDiff = (cmd1.getControlStrength() - cmd2.getControlStrength()) + rand2f(SkillRandomAmount);
		
		//if (skillDiff != 0) print("skillDiff: " + skillDiff);
						
		if (skillDiff > 0) {
		
			footballer1.onBallCommandConflictFeedback(cmd1, Footballer.BallConflictResult_Won);
			footballer2.onBallCommandConflictFeedback(cmd2, Footballer.BallConflictResult_Lost);
			cmd2.invalidateAndNotify();
		
		} else if (skillDiff < 0) {
		
			footballer1.onBallCommandConflictFeedback(cmd1, Footballer.BallConflictResult_Lost);
			footballer2.onBallCommandConflictFeedback(cmd2, Footballer.BallConflictResult_Won);
			cmd1.invalidateAndNotify();
			
		} else {
		
			footballer1.onBallCommandConflictFeedback(cmd1, Footballer.BallConflictResult_Tie);
			footballer2.onBallCommandConflictFeedback(cmd2, Footballer.BallConflictResult_Tie);
		}
	}
	
	function validateCommand(cmd, t) {
	
		if (cmd.isValid()) {
	
			if (cmd.getSource().isFootballer()) {
			
				switch (cmd.getType()) {

					case cmd.Type_Pos: {
				
						local footballer = cmd.getSource().toFootballer();
					
						if (!footballer.mImpl.isBallOwner()) {
					
							cmd.invalidateAndNotify();
						
							return false;
						}
					} break;
					
					
					case cmd.Type_Vel: {
				
						local footballer = cmd.getSource().toFootballer();
					
						if (!footballer.mBody.ballIsHeaderHeight() && !footballer.mImpl.isBallOwner()) {
					
							cmd.invalidateAndNotify();
						
							return false;
						}
						
					} break;
				}
			}
			
			return true;
		}
		
		return false;
	}
		
	function process(impl, ball, t) {
				
		local cmdCount = impl.getCommandCount();
		
		for (local i = 0; i < cmdCount; i += 1) {
		
			local cmd1 = impl.getCommand(i);
						
			if (validateCommand(cmd1, t) && cmd1.getSource().isFootballer()) {
		
				for (local j = 0; j < cmdCount; j += 1) {

					if (j != i) {	
								
						local cmd2 = impl.getCommand(j);
						
						if (validateCommand(cmd2, t) && cmd2.getSource().isFootballer()) {
					
							tryResolveFootballerConflict(cmd1, cmd2);
						}
					}
				}
			}
		}
		
		local finalCmd = impl.getFinalCommand();
		
		for (local i = 0; i < cmdCount; i += 1) {
		
			local cmd = impl.getCommand(i);
		
			finalCmd.mergeWith(ball, cmd, 0.5, 1.0);
		}
			
	}
}