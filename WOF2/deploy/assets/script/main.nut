
function createMatch(scriptEngine, matchImpl) {

	if (("match" in getroottable()) && ::match != null)
		return ::match;
		
	if (!scriptEngine.runFile("Match")) 
		return null;	
		
	local match = Match();
	
	if (!match.init(scriptEngine, matchImpl))
		match = null;
	
	::match <- match;

	return ::match;
}
