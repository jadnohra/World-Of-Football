#include "WEScriptCoordSys.h"

#include "../../WEAssert.h"
#include "../../WEMacros.h"

DECLARE_INSTANCE_TYPE_NAME(WE::ScriptCoordSys, CoordSys);

namespace WE {

const TCHAR* ScriptCoordSys::ScriptClassName = L"CoordSys";

void ScriptCoordSys::declareInVM(SquirrelVM& target) {

	SqPlus::SQClassDef<ScriptCoordSys>(ScriptClassName).
		func(&ScriptCoordSys::script_convUnit, L"convUnit").
		func(&ScriptCoordSys::script_invConvUnit, L"invConvUnit").
		func(&ScriptCoordSys::script_convVector3Units, L"convVector3Units").
		func(&ScriptCoordSys::script_invConvVector3Units, L"invConvVector3Units").
		func(&ScriptCoordSys::script_setRUF, L"setRUF");
}

ScriptCoordSys::ScriptCoordSys(FastUnitCoordSys* pRef) 
	: SoftPtr(pRef) {
}


}