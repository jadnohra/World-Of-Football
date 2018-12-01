#include "WEScriptVector2.h"

DECLARE_INSTANCE_TYPE_NAME(WE::Vector2, Vector2);
DECLARE_INSTANCE_TYPE_NAME(WE::CtVector2, CtVector2);


namespace WE {

const TCHAR* ScriptVector2::ScriptClassName = L"Vector2";

void ScriptVector2::declareInVM(SquirrelVM& target) {

	SqPlus::SQClassDef<Vector2>(ScriptClassName).
		func(&Vector2::zero, L"zero").
		func(&Vector2::script_isZero, L"isZero").
		func(&Vector2::script_equals, L"equals").
		func(&Vector2::script_set2, L"set2").
		func(&Vector2::script_setEl, L"setEl").
		func(&Vector2::script_get, L"getEl").
		func(&Vector2::script_get, L"get").
		func(&Vector2::script_dot, L"dot").
		func(&Vector2::script_add, L"add").
		func(&Vector2::script_subtract, L"subtract").
		func(&Vector2::script_subtractFromSelf, L"subtractFromSelf").
		func(&Vector2::script_set, L"set").
		func(&Vector2::script_addToSelf, L"addToSelf").
		func(&Vector2::script_mul, L"mul").
		func(&Vector2::script_div, L"div").
		func(&Vector2::script_mulToSelf, L"mulToSelf").
		func(&Vector2::script_divToSelf, L"divToSelf").
		func(&Vector2::script_mulAndAdd, L"mulAndAdd").
		func(&Vector2::script_vmul, L"vmul").
		func(&Vector2::script_normalize, L"normalize").
		func(&Vector2::script_normalizeTo, L"normalizeTo").
		func(&Vector2::script_magSq, L"magSq").
		func(&Vector2::script_mag, L"mag").
		func(&Vector2::script_negateTo, L"negateTo").
		func(&Vector2::script_negate, L"negate").
		func(&Vector2::script_subtractNoReverse, L"subtractNoReverse").
		func(&Vector2::script_randomize, L"randomize").
		func(&Vector2::script_randomizeToSelf, L"randomizeToSelf").
		func(&Vector2::script_addMultiplication, L"addMultiplication").
		var(&Vector2::x, L"x").
		var(&Vector2::y, L"y");
}

const TCHAR* ScriptCtVector2::ScriptClassName = L"CtVector2";

void ScriptCtVector2::declareInVM(SquirrelVM& target) {

	SqPlus::SQClassDef<CtVector2>(ScriptClassName).
		staticVar(&CtVector2::kZero, L"kZero").
		func(&CtVector2::script_isZero, L"isZero").
		func(&CtVector2::script_equals, L"equals").
		func(&CtVector2::script_get, L"get").
		func(&CtVector2::script_dot, L"dot").
		func(&CtVector2::script_add, L"add").
		func(&CtVector2::script_mul, L"mul").
		func(&CtVector2::script_div, L"div").
		func(&CtVector2::script_mulAndAdd, L"mulAndAdd").
		func(&CtVector2::script_subtract, L"subtract").
		func(&CtVector2::script_get, L"getEl").
		func(&CtVector2::script_magSq, L"magSq").
		func(&CtVector2::script_mag, L"mag").
		func(&CtVector2::script_negateTo, L"negateTo").
		func(&CtVector2::script_normalizeTo, L"normalizeTo").
		func(&CtVector2::script_randomize, L"randomize").
		func(&CtVector2::script_addMultiplication, L"addMultiplication");

		SqPlus::BindVariable(&CtVector2::kZero, L"kZeroVector2");
}

}