#include "ScriptRenderLight.h"

DECLARE_INSTANCE_TYPE_NAME(WE::RenderLight, CRenderLight);

namespace WE {

const TCHAR* ScriptRenderLight::ScriptClassName = L"CRenderLight";

void ScriptRenderLight::declareInVM(SquirrelVM& target) {

	SqPlus::SQClassDef<RenderLight>(ScriptClassName).
		enumInt(RL_Directional, L"Directional").
		enumInt(RL_Point, L"Point").
		func(&RenderLight::setType, L"setType").
		func(&RenderLight::script_getDiffuse, L"diffuse").
		func(&RenderLight::script_getSpecular, L"specular").
		func(&RenderLight::script_getAmbient, L"ambient").
		func(&RenderLight::script_getDirection, L"direction").
		func(&RenderLight::script_getPosition, L"position").
		var(&RenderLight::range, L"range").
		var(&RenderLight::attenuation, L"attenuation");
}

}