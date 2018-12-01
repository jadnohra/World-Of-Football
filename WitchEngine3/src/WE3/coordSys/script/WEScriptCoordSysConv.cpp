#include "WEScriptCoordSysConv.h"

#include "../../WEMem.h"

DECLARE_INSTANCE_TYPE_NAME(WE::ScriptCoordSysConv, CoordSysConv);

namespace WE {

const TCHAR* ScriptCoordSysConv::ScriptClassName = L"CoordSysConv";

void ScriptCoordSysConv::declareInVM(SquirrelVM& target) {

	SqPlus::SQClassDef<ScriptCoordSysConv>(ScriptClassName).
		func(&ScriptCoordSysConv::script_convVector3, L"convVector3").
		func(&ScriptCoordSysConv::script_convPoint3, L"convPoint3").
		func(&ScriptCoordSysConv::script_convUnit, L"convUnit").
		func(&ScriptCoordSysConv::script_invConvUnit, L"invConvUnit").
		func(&ScriptCoordSysConv::script_convVector3Units, L"convVector3Units");
}

ScriptCoordSysConv::ScriptCoordSysConv(CoordSysConv* pRef) 
	: HardRef(pRef) {

	if (!isValid()) {

		WE_MMemNew(ptrRef(), CoordSysConv());
	}
}

}