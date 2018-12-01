#include "ScriptRenderColor.h"

DECLARE_INSTANCE_TYPE_NAME(WE::RenderColor, CRenderColor);

namespace WE {

const TCHAR* ScriptRenderColor::ScriptClassName = L"CRenderColor";

void ScriptRenderColor::declareInVM(SquirrelVM& target) {

	SqPlus::SQClassDef<RenderColor>(ScriptClassName).
		func(&RenderColor::set2, L"set").
		func(&RenderColor::getR, L"getR").
		func(&RenderColor::getG, L"getG").
		func(&RenderColor::getB, L"getB").
		func(&RenderColor::getAlpha, L"getAlpha");
}

}