#include "WOFScriptBodyDynamics.h"


namespace WOF {


bool ScriptBodyDynamics::create(ScriptEngine& engine, const TCHAR* className) {

	return engine.createInstance(className, mScriptObject);
}

bool ScriptBodyDynamics::load(ScriptEngine& engine, DataChunk* pChunk, CoordSysConv* pConv) {

	ScriptEngine::FunctionCall<bool> fct(engine, toScriptArg(), L"load");

	return (fct)(ScriptDataChunk(pChunk).toScriptArg(), ScriptCoordSysConv(pConv).toScriptArg());
}

bool ScriptBodyDynamics::isSimulable(ScriptEngine& engine) {

	ScriptEngine::FunctionCall<bool> fct(engine, toScriptArg(), L"isSimulable");

	return (fct)();
}

bool ScriptBodyDynamics::hasRestingState(ScriptEngine& engine, const Vector3& startAcc, const Vector3& startVel) {

	ScriptEngine::FunctionCall<bool> fct(engine, toScriptArg(), L"hasRestingState");

	return (fct)(ScriptVector3::toScriptArg(&Vector3(startAcc)), ScriptVector3::toScriptArg(&Vector3(startVel)));
}

bool ScriptBodyDynamics::simulate(ScriptEngine& engine, const Time& tickLength, Vector3& inOutAcc, Vector3& inOutVel, Time& t) {

	ScriptEngine::FunctionCall<bool> fct(engine, toScriptArg(), L"simulate");

	return (fct)(tickLength, ScriptVector3::toScriptArg(&inOutAcc), ScriptVector3::toScriptArg(&inOutVel), &ScriptArgByRef_float(t));
}


void ScriptBodyDynamics::start(ScriptEngine& engine, const Time& t, Vector3& startAcc, Vector3& startVel) {

	ScriptEngine::FunctionCall<void> fct(engine, toScriptArg(), L"start");

	(fct)(t, ScriptVector3::toScriptArg(&startAcc), ScriptVector3::toScriptArg(&startVel));
}

void ScriptBodyDynamics::stop(ScriptEngine& engine, const Time& t) {

	ScriptEngine::FunctionCall<void> fct(engine, toScriptArg(), L"stop");

	(fct)(t);
}

bool ScriptBodyDynamics::frameMove(ScriptEngine& engine, const Time& t, const Time& dt, Vector3& currAcc, Vector3& currVel) {

	ScriptEngine::FunctionCall<bool> fct(engine, toScriptArg(), L"frameMove");

	return (fct)(t, dt, ScriptVector3::toScriptArg(&currAcc), ScriptVector3::toScriptArg(&currVel));
}

}
