#include "WEVector3_ScriptClassDecl.h"

DECLARE_INSTANCE_TYPE(Vector3);

namespace WOF {

const TCHAR* Vector3_ScriptClassDecl::ScriptClassName = L"Vector3";

void Vector3_ScriptClassDecl::declareScriptClass(SquirrelVM& target) {

	SqPlus::SQClassDef<Vector3>(ScriptClassName).
		func(&Vector3::zero, L"zero").
		func(&Vector3::_set, L"set").
		func(&Vector3::_dot, L"dot").
		func(&Vector3::_add, L"add").
		var(&Vector3::x, L"el[0]").
		var(&Vector3::y, L"el[1]").
		var(&Vector3::z, L"el[2]");
}

}