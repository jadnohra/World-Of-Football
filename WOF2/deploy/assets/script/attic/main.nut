
/*

Destructible c++ defined class which calls script side func 'destructor'

class SoftRef extends Destructible {

	destructor() {
		
		if (object != null)
			object->Release();
			
		object = null;
	}

	object = null;
}
*/

function createMatch(scriptEngine, matchImpl) {

	if (::match != null)
		return ::match;
		
	::match <- Match(matchImpl);

	return ::match;
}

function main(scriptEngine) {

	//we need a nicer way of scanning and including all nut files in some special 'active' directory
	//with the possibility of error checking, and passing the scriptEngine or making runFile global ...
	//until then just do it manually

	//if (!scriptEngine.runFile("footballer/Footballer_QuickTest")) 
	//	return false;

	::scriptEngine <- scriptEngine;
	
	if (!scriptEngine.runFile("Game")) 
		return false;
	
	::game <- Game();
	
	if (!::game.init(scriptEngine))
		return false;
	
	if (!scriptEngine.runFile("FuncRefArg")) 
		return false;
	
	if (!scriptEngine.runFile("ProcessResources")) 
		return false;
		
	if (!processResources(game))
		return false;
	
	
	if (!scriptEngine.runFile("PhaseManager")) 
		return false;
		
	/*	
	if (!scriptEngine.runFile("ResponseCurve")) 
		return false;
		*/
	
	if (!scriptEngine.runFile("footballer/InitInputControllers")) 
		return false;
	
	if (!scriptEngine.runFile("footballer/FootballerBallShot")) 
		return false;
	
	if (!scriptEngine.runFile("footballer/FootballerBodyJumpPhysics")) 
		return false;
	
	if (!scriptEngine.runFile("footballer/FootballerSettings")) 
		return false;	
	
	if (!scriptEngine.runFile("footballer/FootballerSettings_Idle")) 
		return false;	
		
	if (!scriptEngine.runFile("footballer/FootballerSettings_Run")) 
		return false;
		
	if (!scriptEngine.runFile("footballer/FootballerSettings_Tackle")) 
		return false;	
		
	if (!scriptEngine.runFile("footballer/FootballerSettings_Fall")) 
		return false;		
	
	if (!scriptEngine.runFile("footballer/FootballerSettings_Jump")) 
		return false;			

	if (!scriptEngine.runFile("footballer/FootballerSettings_Shoot")) 
		return false;					
	
	if (!scriptEngine.runFile("footballer/FootballerBase")) 
		return false;
	
	if (!scriptEngine.runFile("footballer/FootballerBallHandler")) 
		return false;
		
	if (!scriptEngine.runFile("footballer/FootballerBallTriggers")) 
		return false;	
		
	if (!scriptEngine.runFile("footballer/BodyActionBase")) 
		return false;
	
	if (!scriptEngine.runFile("footballer/BodyAction_Idle")) 
		return false;
		
	if (!scriptEngine.runFile("footballer/BodyAction_Running")) 
		return false;
		
	if (!scriptEngine.runFile("footballer/BodyAction_Tackling")) 
		return false;
		
	if (!scriptEngine.runFile("footballer/BodyAction_Falling")) 
		return false;
		
	if (!scriptEngine.runFile("footballer/BodyAction_Jumping")) 
		return false;
	
	if (!scriptEngine.runFile("footballer/BodyAction_Shooting")) 
		return false;	
		
	if (!scriptEngine.runFile("footballer/BodyAction_Turning")) 
		return false;	
	
	if (!scriptEngine.runFile("footballer/FootballerBrainBase")) 
		return false;
	
	if (!scriptEngine.runFile("footballer/FootballerBrain_AI_Idle")) 
		return false;
	
	if (!scriptEngine.runFile("footballer/FootballerBrain_AI_Random")) 
		return false;
	
	if (!scriptEngine.runFile("footballer/FootballerBrain_AI_v1")) 
		return false;
	
	if (!scriptEngine.runFile("footballer/FootballerBrain_AI")) 
		return false;
	
	if (!scriptEngine.runFile("footballer/FootballerBrain_Player")) 
		return false;

	if (!scriptEngine.runFile("footballer/Footballer")) 
		return false;
		
	if (!scriptEngine.runFile("footballer/BallCommandManager")) 
		return false;	
		
	if (!scriptEngine.runFile("util")) 
		return false;		
		
	//print(::game.getVersion());
		
	return true;
}

function loadMatchLights(match) {

	return ::game.loadMatchLights(match);
}
	
function setMatchLights(match) {
	
	return ::game.setMatchLights(match);
}

function onMatchLoaded(match) {

	return ::game.onMatchLoaded(match);
}
	