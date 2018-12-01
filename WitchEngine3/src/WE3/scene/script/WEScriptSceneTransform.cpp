#include "WEScriptSceneTransform.h"

DECLARE_INSTANCE_TYPE_NAME(WE::SceneTransform, CSceneTransform);


namespace WE {

const TCHAR* ScriptSceneTransform::ScriptClassName = L"CSceneTransform";

void ScriptSceneTransform::declareInVM(SquirrelVM& target) {

	SqPlus::SQClassDef<SceneTransform>(ScriptClassName).
		func(&SceneTransform::script_getPosition, L"getPos").
		func(&SceneTransform::script_getDirection, L"getDir").
		func(&SceneTransform::script_transformVector, L"transformVector").
		func(&SceneTransform::script_transformPoint, L"transformPoint").
		func(&SceneTransform::script_simpleInvTransformVector, L"simpleInvTransformVector").
		//func(&SceneTransform::script_simpleInvTransformPoint, L"simpleInvTransformPoint").
		func(&SceneTransform::script_invTransformVector, L"invTransformVector").
		func(&SceneTransform::script_invTransformPoint, L"invTransformPoint").
		func(&SceneTransform::script_isEqualDir, L"isEqualDir");
}

}