#include "WOFConfig_ScriptProxy.h"

DECLARE_INSTANCE_TYPE_NAME(WOF::Config_ScriptProxy, Config);

namespace WOF {

const TCHAR* Config_ScriptProxy::ScriptClassName = L"Config";

void Config_ScriptProxy::declareScriptClass(SquirrelVM& target) {

	SqPlus::SQClassDef<Config_ScriptProxy>(ScriptClassName).
		func(&Config_ScriptProxy::has, L"has").
		func(&Config_ScriptProxy::get, L"get");
}

Config_ScriptProxy::Config_ScriptProxy(DataChunk* pChunk) {

	mChunk = pChunk;
}

bool Config_ScriptProxy::has(const SQChar *varName) {

	return mChunk.isValid() && mChunk->hasAttribute(varName);
}

const SQChar* Config_ScriptProxy::get(const SQChar *varName) {

	if (mChunk.isValid() && mChunk->getAttribute(varName, mStringHolder)) {
	
		return mStringHolder.c_tstr();
	}

	mStringHolder.assign(L"worx");

	return mStringHolder.c_tstr();
}

}