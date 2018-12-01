#include "WEScriptVector3.h"

DECLARE_INSTANCE_TYPE_NAME(WE::Vector3, Vector3);
DECLARE_INSTANCE_TYPE_NAME(WE::CtVector3, CtVector3);


namespace WE {

const TCHAR* ScriptVector3::ScriptClassName = L"Vector3";

void ScriptVector3::declareInVM(SquirrelVM& target) {

	SqPlus::SQClassDef<Vector3>(ScriptClassName).
		func(&Vector3::zero, L"zero").
		func(&Vector3::script_isZero, L"isZero").
		func(&Vector3::script_equals, L"equals").
		func(&Vector3::script_set3, L"set3").
		func(&Vector3::script_setEl, L"setEl").
		func(&Vector3::script_get, L"getEl").
		func(&Vector3::script_get, L"get").
		func(&Vector3::script_dot, L"dot").
		func(&Vector3::script_add, L"add").
		func(&Vector3::script_subtract, L"subtract").
		func(&Vector3::script_cross, L"cross").
		func(&Vector3::script_subtractFromSelf, L"subtractFromSelf").
		func(&Vector3::script_set, L"set").
		func(&Vector3::script_addToSelf, L"addToSelf").
		func(&Vector3::script_mul, L"mul").
		func(&Vector3::script_div, L"div").
		func(&Vector3::script_mulToSelf, L"mulToSelf").
		func(&Vector3::script_divToSelf, L"divToSelf").
		func(&Vector3::script_mulAndAdd, L"mulAndAdd").
		func(&Vector3::script_vmul, L"vmul").
		func(&Vector3::script_normalize, L"normalize").
		func(&Vector3::script_normalizeTo, L"normalizeTo").
		func(&Vector3::script_magSq, L"magSq").
		func(&Vector3::script_mag, L"mag").
		func(&Vector3::script_negateTo, L"negateTo").
		func(&Vector3::script_negate, L"negate").
		func(&Vector3::script_subtractNoReverse, L"subtractNoReverse").
		func(&Vector3::script_randomize, L"randomize").
		func(&Vector3::script_randomizeToSelf, L"randomizeToSelf").
		func(&Vector3::script_randomizeDir, L"randomizeDir").
		func(&Vector3::script_randomizeDirToSelf, L"randomizeDirToSelf").
		func(&Vector3::script_addMultiplication, L"addMultiplication").
		var(&Vector3::x, L"x").
		var(&Vector3::y, L"y").
		var(&Vector3::z, L"z");
}

const TCHAR* ScriptCtVector3::ScriptClassName = L"CtVector3";

void ScriptCtVector3::declareInVM(SquirrelVM& target) {

	SqPlus::SQClassDef<CtVector3>(ScriptClassName).
		staticVar(&CtVector3::kZero, L"kZero").
		func(&CtVector3::script_isZero, L"isZero").
		func(&CtVector3::script_equals, L"equals").
		func(&CtVector3::script_get, L"get").
		func(&CtVector3::script_dot, L"dot").
		func(&CtVector3::script_add, L"add").
		func(&CtVector3::script_mul, L"mul").
		func(&CtVector3::script_div, L"div").
		func(&CtVector3::script_mulAndAdd, L"mulAndAdd").
		func(&CtVector3::script_subtract, L"subtract").
		func(&CtVector3::script_cross, L"cross").
		func(&CtVector3::script_get, L"getEl").
		func(&CtVector3::script_magSq, L"magSq").
		func(&CtVector3::script_mag, L"mag").
		func(&CtVector3::script_negateTo, L"negateTo").
		func(&CtVector3::script_normalizeTo, L"normalizeTo").
		func(&CtVector3::script_randomize, L"randomize").
		func(&CtVector3::script_randomizeDir, L"randomizeDir").
		func(&CtVector3::script_addMultiplication, L"addMultiplication");

		SqPlus::BindVariable(&CtVector3::kZero, L"kZeroVector3");
}

}