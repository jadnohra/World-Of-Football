#include "WOFMatchFootballerBody_ConfigurableActionDynamics_Scripted.h"

#include "../WOFMatchFootballer_Body.h"
#include "../../../WOFMatch.h"

namespace WOF { namespace match {


bool FootballerBody_ConfigurableActionDynamics_Scripted::load(Footballer_Body& body, BufferString& tempStr, DataChunk& chunk, CoordSysConv* pConv) {

	ScriptEngine& scriptEngine = body.mNodeMatch->getScriptEngine();

	if (chunk.getAttribute(L"class", tempStr)
		&& mImpl.create(scriptEngine, tempStr.c_tstr())
		&& mImpl.load(scriptEngine, &chunk, pConv)) {

		return true;
	}

	return false;
}

bool FootballerBody_ConfigurableActionDynamics_Scripted::isSimulable(Footballer_Body& body) {

	ScriptEngine& scriptEngine = body.mNodeMatch->getScriptEngine();

	return mImpl.isSimulable(scriptEngine);
}

bool FootballerBody_ConfigurableActionDynamics_Scripted::hasRestingState(Footballer_Body& body, const Vector3& startAcc, const Vector3& startVel) {

	ScriptEngine& scriptEngine = body.mNodeMatch->getScriptEngine();

	return mImpl.hasRestingState(scriptEngine, startAcc, startVel);
}

bool FootballerBody_ConfigurableActionDynamics_Scripted::simulate(Footballer_Body& body, const Time& tickLength, Vector3& inOutAcc, Vector3& inOutVel, Time& t) {

	ScriptEngine& scriptEngine = body.mNodeMatch->getScriptEngine();

	return mImpl.simulate(scriptEngine, tickLength, inOutAcc, inOutVel, t);
}

void FootballerBody_ConfigurableActionDynamics_Scripted::start(Footballer_Body& body, const Time& t, Vector3& startAcc, Vector3& startVel) {

	ScriptEngine& scriptEngine = body.mNodeMatch->getScriptEngine();

	mImpl.start(scriptEngine, t, startAcc, startVel);
}

void FootballerBody_ConfigurableActionDynamics_Scripted::stop(Footballer_Body& body, const Time& t) {

	ScriptEngine& scriptEngine = body.mNodeMatch->getScriptEngine();

	mImpl.stop(scriptEngine, t);
}

bool FootballerBody_ConfigurableActionDynamics_Scripted::frameMove(Footballer_Body& body, const Time& t, const Time& dt, Vector3& currAcc, Vector3& currVel) {

	ScriptEngine& scriptEngine = body.mNodeMatch->getScriptEngine();

	return mImpl.frameMove(scriptEngine, t, dt);
}

} }