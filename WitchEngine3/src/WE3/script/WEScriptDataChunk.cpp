#include "WEScriptDataChunk.h"

#include "../helper/load/WEMathDataLoadHelper.h"

using namespace WE;
DECLARE_INSTANCE_TYPE_NAME(ScriptDataChunk, DataChunk);

namespace WE {

const TCHAR* ScriptDataChunk::ScriptClassName = L"DataChunk";

void ScriptDataChunk::declareInVM(SquirrelVM& target) {

	SqPlus::SQClassDef<ScriptDataChunk>(ScriptClassName).
		func(&ScriptDataChunk::script_isValid, L"isValid").
		func(&ScriptDataChunk::script_hasChild, L"hasChild").
		func(&ScriptDataChunk::script_toChild, L"toChild").
		func(&ScriptDataChunk::script_getChild, L"getChild").
		func(&ScriptDataChunk::script_is, L"is").
		func(&ScriptDataChunk::script_has, L"has").
		func(&ScriptDataChunk::script_get, L"get").
		func(&ScriptDataChunk::script_getVector3, L"getVector3").
		func(&ScriptDataChunk::script_geti, L"geti").
		func(&ScriptDataChunk::script_getf, L"getf");
}

bool ScriptDataChunk::script_is(const TCHAR* tagName) {

	return isValid() && ptr()->equals(tagName);
}

bool ScriptDataChunk::script_has(const TCHAR* varName) {

	return isValid() && ptr()->hasAttribute(varName);
}

const TCHAR* ScriptDataChunk::script_get(const TCHAR* varName) {

	if (isValid() && ptr()->getAttribute(varName, mStringHolder)) {
	
		return mStringHolder.c_tstr();
	}

	return L"";
}

bool ScriptDataChunk::script_getVector3(const TCHAR* varName, Vector3* pValue, ScriptCoordSysConv* pConv) {

	if (isValid() && pValue) {
	
		return MathDataLoadHelper::extract(mStringHolder, dref(), WE::dref(pValue), true, true, false, varName, pConv ? *pConv : NULL, true, true);
	}

	return false;
}

int ScriptDataChunk::script_geti(const TCHAR* varName, int val) {

	if (isValid() && ptr()->scanAttribute(mStringHolder, varName, L"%d", &val)) {
	
		return val;
	}

	return val;
}

float ScriptDataChunk::script_getf(const TCHAR* varName, float val) {

	if (isValid() && ptr()->scanAttribute(mStringHolder, varName, L"%f", &val)) {
	
		return val;
	}

	return val;
}

ScriptDataChunk ScriptDataChunk::script_getChild(const TCHAR* name) {

	if (isValid()) {

		SoftRef<DataChunk> child = ptr()->getChild(name);

		return ScriptDataChunk(child);
	}

	return ScriptDataChunk();
}

bool ScriptDataChunk::script_hasChild(const TCHAR* name) {

	if (isValid()) {

		SoftRef<DataChunk> child = ptr()->getChild(name);

		return child.isValid();
	}

	return false;
}

bool ScriptDataChunk::script_toChild(const TCHAR* name) {

	if (isValid()) {

		SoftRef<DataChunk> child = ptr()->getChild(name);

		(*this) = child.ptr();
	}

	return isValid();
}

}