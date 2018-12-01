
class TeamAIOppFootballerFlag {

	static Type_None = -1;
	static Type_ZoneDefend = 0;
	
	constructor(_type, _source) {
	
		type = _type;
		source = _source;
	}
	
	function update(t) {
	
		lastUpdateTime = t;
	}
	
	function end() {
	
		lastUpdateTime = -1.0;
	}
	
	function isActive(t, dt) {
	
		return lastUpdateTime + (dt * 1.5) >= t;
	}
	
	type = -1;
	source = null;
	lastUpdateTime = -1.0;
}

class TeamAIOppFootballerState {

	constructor() {
	
		mFlags = [];
	}

	function addFlag(flag) {
	
		mFlags.append(flag);	
	}
	
	function findFlagWithType(type) {
	
		if (mFlags) {
		
			foreach (flag in mFlags) {
			
				if (flag.type == type)
					return flag;
			}
		}
		
		return null;
	}
	
	function findActiveFlagWithType(type, t, dt) {
	
		if (mFlags) {
		
			for (local i = 0; i < mFlags.len(); i += 1) {
			
				local flag = mFlags[i];
			
				if (flag.type == type) {
				
					if (flag.isActive(t, dt)) {
					
						return flag;
						
					} else {
					
						mFlags.remove(i);
						i -= 1;
					}		
				}
			}
		}
		
		return null;
	}
	
	function removeFlag(type, source) {
	
		if (mFlags) {
		
			local index = 0;
			foreach (flag in mFlags) {
			
				if (flag.type == type && flag.source == source) {
				
					mFlags.remove(index);
					break;
				}
				
				index += 1;
			}
		}
	}

	mFlags = null;
}

class TeamAIRequest {

	static State_Waiting = -1;
	static State_Granted = 0;
	static State_Refused = 1;
	
	type = -1;
	state = -1;
	requestTime = -1.0;
	requestSource = null;
	
	constructor(type, time, source) {
	
		requestTime = time;
		requestSource = source;
	}
	
	function isWaiting() { return state == State_Waiting; }
	function isGranted() { return state == State_Granted; }
}

class TeamAIRequest_Footballer extends TeamAIRequest {

	function setTargetFootballer(footballer) {
	
		targetFootballer = footballer;
	}
	
	function getTargetFootballer() {
	
		return targetFootballer;
	}

	targetFootballer = null;
}

class TeamAIRegionTrack {

	static State_Tracking = -1;
	static State_Tracked = 0;
	static State_Failed = 1;

	constructor(t, region) {
	
		mRegion = region;
	}

	function doQuery(matchImpl, refTeam, teamFilter) {
	
		local footballer;
		local index = Int(0);
		
		while ((footballer = matchImpl.iterNextRegionFootballer(mRegion, teamFilter, index)) != null) {
		
			if (mFootballers == null)
				mFootballers = [];

			mFootballers.append(footballer.getScriptObject());	
		}
	
		mState = State_Tracked;
	}
	
	function isTracking() {
	
		return mState == State_Tracking;
	}
	
	function isValid() {
	
		return mState == State_Tracked;
	}
	
	function getFootballerArray() {
	
		return mFootballers;
	}	
	
	mRegion = null;
	mState = -1;
	mFootballers = null;
}

class TeamAI_v1 {

	constructor(match, thisTeam, oppTeam) {

		mMatch = match;
		mThisTeam = thisTeam;
		mOppTeam = oppTeam;
	
		mOppStates = {};
		mRegionTrackTable = {};	
		mRequestTables = {};
	}

	function getAIGoalPool() {
		
		return mMatch.getAIGoalPool();
	}
	
	function getOpponentState(oppNumber) {
	
		local oppState = oppNumber in mOppStates ? mOppStates[oppNumber] : null;
		
		if (oppState == null) {
		
			oppState = TeamAIOppFootballerState();
			mOppStates[oppNumber] <- oppState;
		}
		
		return oppState;
	}
	
	function addOpponentFlagRequest(t, dt, selfFootballer, oppFootballer, flagType, checkFlag) {
	
		local oppNumber = oppFootballer.impl.getNumber();
	
		if (checkFlag) {
		
			local oppState = getOpponentState(oppNumber);
			
			if (oppState.findActiveFlagWithType(flagType, t, dt) != null)
				return null;
		}
	
		local request = TeamAIRequest_Footballer(flagType, t, selfFootballer);
		request.setTargetFootballer(oppFootballer);
		
		local requestTable = oppNumber in mRequestTables ? mRequestTables[oppNumber] : null;
		
		if (requestTable == null) {
		
			requestTable = {}
			mRequestTables[oppNumber] <- requestTable;
		}
		
		local requestArray = flagType in requestTable ? requestTable[flagType] : null;
		
		if (requestArray == null) {
			
			requestArray = [];
			requestTable[flagType] <- requestArray;
		}
		
		requestArray.append(request);
			
		return request;
	}
	
	function addOpponentFlag(t, selfFootballer, oppFootballer, flagType) {
	
		local oppNumber = oppFootballer.impl.getNumber();
		local oppState = getOpponentState(oppNumber);
		
		local flag = TeamAIOppFootballerFlag(flagType, selfFootballer);
		oppState.addFlag(flag);
		
		flag.update(t);
		
		return flag;
	}
	
	function removeOpponentFlag(t, selfFootballer, oppFootballer, flagType) {
	
		local oppNumber = oppFootballer.impl.getNumber();
		local oppState = getOpponentState(oppNumber);
		
		oppState.removeFlag(flagType, selfFootballer);
	}
	
	function addRegionTrack(t, selfFootballer, region) {
	
		local key = region.getID();
		local regionTrack = (key in mRegionTrackTable) ? mRegionTrackTable[key] : null;
		
		if (regionTrack == null) {
		
			regionTrack = TeamAIRegionTrack(t, region);
			mRegionTrackTable[key] <- regionTrack;
		} 
		
		return regionTrack;
	}
	
	
	function selectRequest_ZoneDefendOpponent(bestRequest, request, oppFootballer) {
	
		if (bestRequest == null)
			return request;
		
		local oppPos = oppFootballer.impl.getPos();
		
		local distSqBest = distSq(bestRequest.requestSource.impl.getPos(), oppPos);
		local distSqNew = distSq(request.requestSource.impl.getPos(), oppPos);
		
		return distSqNew < distSqBest ? request : bestRequest;
	}
	
	function frameMove(match, t, dt) {
		
		{
		
			foreach(key, regionTrack in mRegionTrackTable) {
			
				regionTrack.doQuery(match.impl, mThisTeam, mOppTeam);
			}
			
			mRegionTrackTable = {};
		}
		
		{
			foreach(oppNumber, requestTable in mRequestTables) {

				local oppFootballer = match.impl.getFootballer(mOppTeam, oppNumber);
				
				foreach(reqType, requestArray in requestTable) {
				
					local alreadyTaken = false;
					
					if (oppFootballer != null) {
					
						local oppNumber = oppFootballer.impl.getNumber();
						local oppState = getOpponentState(oppNumber);
							
						if (oppState.findActiveFlagWithType(reqType, t, dt) != null) {
						
							alreadyTaken = true;
						}
					}
					
					if (oppFootballer == null || alreadyTaken) {
					
						foreach(request in requestArray)
							request.state = TeamAIRequest.State_Refused;
										
					} else {
						
						local selectFunc = AI_RequestManager.getOppRequestSelectFunction(reqType);	
						local bestRequest = null;
					
						foreach(request in requestArray) {
					
							local selectedRequest = selectFunc(bestRequest, request, oppFootballer);
						
							if (bestRequest != selectedRequest) {
							
								if (bestRequest != null)
									bestRequest.state = TeamAIRequest.State_Refused;
												
								selectedRequest.state = TeamAIRequest.State_Granted;
								
								bestRequest = selectedRequest;
								
							} else {
							
								request.state = TeamAIRequest.State_Refused;
							}
						}
					}
				}
				
				requestTable = null;
			}
			
			mRequestTables = {};
		}
	}
	
	mMatch = null;
	mThisTeam = -1;
	mOppTeam = -1;
	mOppStates = null;
	mRegionTrackTable = null;
	mRequestTables = null;
}