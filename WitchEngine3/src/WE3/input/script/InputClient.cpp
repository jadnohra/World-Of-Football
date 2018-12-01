#include "InputClient.h"

DECLARE_INSTANCE_TYPE_NAME( WE::Input::ScriptInputClient, CInputClient);

namespace WE { namespace Input {

const TCHAR* ScriptInputClient::ScriptClassName = L"CInputClient";

void ScriptInputClient::declareInVM(SquirrelVM& target) {

	SqPlus::SQClassDef<ScriptInputClient>(ScriptClassName).
		func(&ScriptInputClient::script_isValid, L"isValid").
		func(&ScriptInputClient::create, L"create").
		func(&ScriptInputClient::setName, L"setName").
		func(&ScriptInputClient::getName, L"getName").
		func(&ScriptInputClient::addSource, L"addSource").
		func(&ScriptInputClient::addMappedSource, L"addMappedSource").
		func(&ScriptInputClient::removeSource, L"removeSource").
		func(&ScriptInputClient::hasSourceID, L"hasSourceID").
		func(&ScriptInputClient::getSourceID, L"getSourceID").
		func(&ScriptInputClient::getSourceByID, L"getSourceByID").
		func(&ScriptInputClient::startIterateEvents, L"startIterateEvents").
		func(&ScriptInputClient::nextEvent, L"nextEvent").
		func(&ScriptInputClient::breakIterateEvents, L"breakIterateEvents").
		func(&ScriptInputClient::clearEvents, L"clearEvents").
		func(&ScriptInputClient::destroy, L"destroy");
}


} }