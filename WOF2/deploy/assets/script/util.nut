
function printV3(name, v) {

	print(name + ": " + v.getEl(0) + ", " + v.getEl(1) + ", " + v.getEl(2));
}

function logV3(name, v) {

	log(name + ": " + v.getEl(0) + ", " + v.getEl(1) + ", " + v.getEl(2));
}

function help() {

	print("");
	print(" setAutoSwitch(bool)");
	print(" showScanner(bool)");
	print(" freeCam(bool)");
	print(" manipulateCam(bool)");
	print("");
}

function freeCam(enable) {

	getMatch().freeCam(enable);
}

function manipulateCam(enable) {

	getMatch().manipulateCam(enable);
}

function showScanner(show) {

	getMatch().showScanner(show);
}

function switchTeamFormationMode(team, formation, mode) {

	getMatch().getTeam(team).switchFormation(team, formation, mode);
}

function setAutoSwitch(val) {

	::game.getMatch().setAutoSwitch(val);
}

function setSwitchVeryDynamic(val) {

	::game.getMatch().setSwitchVeryDynamic(val);
}

function setupSwitching(autoSwitch, veryDynamic) {

	setAutoSwitch(autoSwitch);
	setSwitchVeryDynamic(veryDynamic);
}

function printFootballerPos(team, number) { 

	local match = ::game.getMatch();
	local footballer = match.getFootballer(team, number);
	
	if (footballer != null) {
	
		printV3(footballer.impl.getNodeName(), footballer.impl.getPos());
	}
}

function printFootballerFacing(team, number) { 

	local match = ::game.getMatch();
	local footballer = match.getFootballer(team, number);
	
	if (footballer != null) {
	
		printV3(footballer.impl.getNodeName(), footballer.impl.getFacing());
	}
}

function setAISwitchLock(team, number, locked) { 

	local match = ::game.getMatch();
	local footballer = match.getFootballer(team, number);
	
	if (footballer != null) {
	
		footballer.mBrainAI.setSwitchLocked(locked);
	}
}

function reloadScriptFile(file) {

	getScriptMatch().mScriptEngine.runFile(file);
}

function currTest9() { 

	local match = ::game.getMatch();
	local defender = match.getFootballer(0, 20);
	local attacker = match.getFootballer(0, 2);
	
	if (defender != null && defender.isInAIMode() && attacker != null) {

		local currTime = match.getTime();
		
		local targetPos = Vector3();
		local targetPosTactical = Vector2();
		
		sceneSetRU(targetPosTactical, 0.0, -match.getPitchHalfLength());
		
		defender.impl.tacticalToWorldPos(targetPosTactical, targetPos);
	
		local region = defender.impl.getRegion("PenaltyBox");
		defender.getBrainAI().setGoalCoverStaticPosFromFootballer(defender, currTime, attacker, targetPos, region, 0.0);
	}
}

function currTest10() { 

	local match = ::game.getMatch();
	local goalie = match.getFootballer(0, 20);
	
	if (goalie != null && goalie.isInAIMode()) {

		local currTime = match.getTime();
		
		goalie.getBrainAI().setGoalGoalieJumpAtBall(goalie, currTime);
	}
}

function currTest11() { 

	local match = ::game.getMatch();
	local goalie = match.getFootballer(0, 20);
	
	if (goalie != null && goalie.isInAIMode()) {

		local currTime = match.getTime();
		
		goalie.getBrainAI().setGoalStaticGoalie1(goalie, currTime, null);
	}
}

function testMiniGame(defenceTeam, goalieNumber, defenderNumber, defenderReflexTime, attackTeam, attackerNumber1, attackerNumber2) { 

	local match = ::game.getMatch();
	local goalie = match.getFootballer(defenceTeam, goalieNumber);
	
	local attacker1 = match.getFootballer(attackTeam, attackerNumber1);
	
	if (goalie != null && goalie.isInAIMode()) {

		local currTime = match.getTime();
		local idleActionGoal = null;
		
		if (attacker1 != null) {
		
			local targetPos = Vector3();
			local targetPosTactical = Vector2();
		
			sceneSetRU(targetPosTactical, 0.0, -match.getPitchHalfLength());
		
			goalie.impl.tacticalToWorldPos(targetPosTactical, targetPos);
	
			local region = goalie.impl.getRegion("GoalBox");
			idleActionGoal = goalie.getBrainAI().createGoalCoverStaticPosFromFootballer(goalie, currTime, attacker1, targetPos, region, 0.0, null);
		}
		
		//idleActionGoal = null;
		goalie.getBrainAI().setGoalStaticGoalie1(goalie, currTime, idleActionGoal);
	}
	
	local defender = match.getFootballer(defenceTeam, defenderNumber);
	local attacker2 = match.getFootballer(attackTeam, attackerNumber2);
	
	if (defender != null && defender.isInAIMode() && attacker2 != null) {

		local currTime = match.getTime();
		
		local targetPos = Vector3();
		local targetPosTactical = Vector2();
		
		sceneSetRU(targetPosTactical, 0.0, -match.getPitchHalfLength());
		
		defender.impl.tacticalToWorldPos(targetPosTactical, targetPos);
	
		local ballOwnerActionGoal = null;
		
		{
			ballOwnerActionGoal = defender.getBrainAI().createGoalGoToSafeSpace(defender, currTime);
		}
			
		local region = defender.impl.getRegion("PenaltyBox");	
		defender.getBrainAI().setGoalCoverStaticPosFromFootballer(defender, currTime, attacker2, targetPos, region, defenderReflexTime, ballOwnerActionGoal);
	}
}

function currTest12(team, number, x, y) {

	local match = ::game.getMatch();
	local footballer = match.getFootballer(team, number);
		
	if (footballer != null && footballer.isInAIMode()) {

		local currTime = match.getTime();
		local teamAI = getScriptMatch().getTeamAI(footballer.impl.getTeamEnum());
		local goal = null;
		
		{
			goal = FootballerBrain_AI_GoalGoto();
		
			local pos = Vector3();
			sceneSetRUF(pos, x.tofloat(), 0.0, y.tofloat());
			goal.setTarget(footballer, pos, false);
			goal.setToleranceMode(FootballerBrain_AI_GoalGoto.Tolerance_JustBefore);
		}
		
		footballer.getBrainAI().setGoal(footballer, currTime, goal);
	}
}

function currTest13(team, number) {

	local match = ::game.getMatch();
	local footballer = match.getFootballer(team, number);
		
	if (footballer != null && footballer.isInAIMode()) {

		local currTime = match.getTime();
		local teamAI = getScriptMatch().getTeamAI(footballer.impl.getTeamEnum());
		local goal = null;
		
		{
			goal = FootballerBrain_AI_GoalGainBall();
		}
		
		footballer.getBrainAI().setGoal(footballer, currTime, goal);
	}
}

function currTest6(dt) { 

	shootLowToPos(0, 2, 0.0, 0.0, true, dt, -1.0);
}

function currTest7(speed) { 

	shootLowToPos(0, 2, 0.0, 0.0, false, -1.0, ::match.impl.conv(speed));
}

function currTest8(passerTeam, passerNumber, targetTeam, targetNumber, speed) { 

	local match = ::game.getMatch();
	local passer = match.getFootballer(passerTeam, passerNumber);
	local target = match.getFootballer(targetTeam, targetNumber);
	
	if (passer != null && passer.isInAIMode() && target != null) {

		local targetPos = target.impl.getPos();
		
		shootLowToPos(passerTeam, passerNumber, targetPos.get(match.Scene_Right), targetPos.get(match.Scene_Fwd), false, -1.0, ::match.impl.conv(speed));
	}
}

function shootHighToPos(team, number, x, y, isByTime, dt, speed) {

	shootToPos(team, number, ::match.getFootballerSettings().Shot_High, x, y, isByTime, dt, speed);
}

function shootLowToPos(team, number, x, y, isByTime, dt, speed) {

	shootToPos(team, number, ::match.getFootballerSettings().Shot_Low, x, y, isByTime, dt, speed);
}

function shootPassToPos(team, number, x, y, isByTime, dt, speed) {

	shootToPos(team, number, ::match.getFootballerSettings().Shot_Pass, x, y, isByTime, dt, speed);
}

function shootExtraToPos(team, number, x, y, isByTime, dt, speed) {

	shootToPos(team, number, ::match.getFootballerSettings().Shot_Extra, x, y, isByTime, dt, speed);
}

function shootToPos(team, number, shotType, x, y, isByTime, dt, speed) {

	shootToPos3D(team, number, shotType, x, y, getMatch().getBall().getRadius(), isByTime, dt, speed);
}

function shootToPos3D(team, number, shotType, x, y, z, isByTime, dt, speed) {

	if (shotType < 0 || shotType >= ::match.getFootballerSettings().ShotTypeCount)
		return;

	local match = ::game.getMatch();
	local footballer = match.getFootballer(team, number);
	
	if (footballer != null && footballer.isInAIMode()) {
	
		local pos = Vector3();
	
		sceneSetRUF(pos, x.tofloat(), z.tofloat(), y.tofloat());
		
		local currTime = match.getTime();
	
		footballer.getBrainAI().setGoalShootTo(footballer, currTime, pos, shotType, true, true, isByTime, currTime + dt, speed);
	}
}

function fireShot(team, number, shotType, shotSpeedScale) {

	local match = ::game.getMatch();
	local footballer = match.getFootballer(team, number);
	
	if (footballer != null && footballer.isInAIMode()) {
	
		footballer.getBrainAI().setGoalFireShot(footballer, match.getTime(), shotType, shotSpeedScale);
	}
}

function runToPos(team, number, x, y) {

	local match = ::game.getMatch();
	local footballer = match.getFootballer(team, number);
	
	if (footballer != null && footballer.isInAIMode()) {
	
		local pos = Vector3();
	
		sceneSetRUF(pos, x.tofloat(), 0.0, y.tofloat());
	
		footballer.getBrainAI().setGoalReachPos(footballer, match.getTime(), pos, match.conv(0.35));
	}
}

function runToBallPos(team, number) {

	local match = ::game.getMatch();
	local footballer = match.getFootballer(team, number);
	
	if (footballer != null && footballer.isInAIMode()) {
	
		footballer.getBrainAI().setGoalReachPos(footballer, match.getTime(), match.getBall().getPos(), match.conv(0.35));
	}
}

function printFormationPos(team, number) {

	local match = ::game.getMatch();
	local footballer = match.getFootballer(team, number);
	
	if (footballer != null) {
	
		local formationPos = Vector3();
		footballer.impl.pitchToFormationPos(footballer.impl.getPos(), formationPos);
		
		print("" + formationPos.getEl(match.Scene_Right) + ", " + formationPos.getEl(match.Scene_Fwd));
	}
}

function getFootballer(team, number) {

	return ::game.getMatch().getFootballer(team, number);
}

function interceptBall(team, number) {

	local match = ::game.getMatch();
	local footballer = match.getFootballer(team, number);
	
	if (footballer != null && footballer.isInAIMode()) {
	
		footballer.getBrainAI().setGoalInterceptBall(footballer, match.getTime(), 0.7, match.conv(0.35));
	}
}

function ownBall(team, number) {

	local match = ::game.getMatch();
	local footballer = match.getFootballer(team, number);
	
	if (footballer != null && footballer.isInAIMode()) {
	
		footballer.getBrainAI().setGoalOwnBall(footballer, match.getTime(), 0.7, match.conv(0.35));
	}
}

function test() { ownBall(1,11); cancelGoal(0,11); }
function test2() { ownBall(1,11); ownBall(0,11); }

function test10() { 
	
	ownBall(1,11); 
	ownBall(1,10); 
	ownBall(1,9); 
	ownBall(1,8); 
	ownBall(1,7); 
	
	ownBall(0,11); 
	ownBall(0,10); 
	ownBall(0,9); 
	ownBall(0,8); 
	ownBall(0,7); 
}

function currTest(acc) { getFootballer(0,20).AttributeSkillShootAccuracy = acc; }

function currTest2(acc, skill) { 

	local footballer = getFootballer(0,20);
	footballer.AttributeSkillShootAccuracy = acc; 
	footballer.AttributeSkillBallControlFeet = skill; 
}

function currTest3(feet, body) { 

	local footballer = getFootballer(0,20);
	footballer.AttributeSkillBallControlFeet = feet; 
	footballer.AttributeSkillBallControlBody = body; 
}

function currTest3_2(acc, feet, body) { 

	local footballer = getFootballer(0,20);
	footballer.AttributeSkillShootAccuracy = acc; 
	footballer.AttributeSkillBallControlFeet = feet; 
	footballer.AttributeSkillBallControlBody = body; 
}

function currTest4(tackler, collider) { 

	local footballer = getFootballer(0,20);
	footballer.AttributePhysicalPowerTackle = tackler; 
	
	footballer = getFootballer(0,2);
	footballer.AttributePhysicalAgilityBalance = collider; 
}

function currTest5(tackling, protection) { 

	local footballer = getFootballer(0,20);
	footballer.AttributeSkillBallTackle = tackling; 
	
	footballer = getFootballer(0,2);
	footballer.AttributeSkillBallProtection = protection; 
}

function cancelGoal(team, number) {

	local match = ::game.getMatch();
	local footballer = match.getFootballer(team, number);
	
	if (footballer != null && footballer.isInAIMode()) {
	
		footballer.getBrainAI().cancelGoal(footballer, match.getTime());
	}
}