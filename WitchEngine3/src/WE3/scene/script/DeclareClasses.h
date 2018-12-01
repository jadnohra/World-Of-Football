#ifndef _WEDeclareClasses_h_Scene
#define _WEDeclareClasses_h_Scene

#include "../../script/WEScriptEngine.h"
#include "WEScriptSceneTransform.h"

namespace WE { namespace scene {

	class DeclareClasses {
	public:

		static void declareInVM(SquirrelVM& target) {

			ScriptSceneTransform::declareInVM(target);

			SqPlus::RegisterGlobal(&DeclareClasses::script_sceneSetRUF, L"sceneSetRUF");
			SqPlus::RegisterGlobal(&DeclareClasses::script_sceneSetRU, L"sceneSetRU");
		}

	public:
	
		inline static void script_sceneSetRUF(Vector3& vec, float r, float u, float f) { vec.el[Scene_Right] = r; vec.el[Scene_Up] = u; vec.el[Scene_Forward] = f; }
		inline static void script_sceneSetRU(Vector2& vec, float r, float f) { vec.el[Scene2D_Right] = r; vec.el[Scene2D_Forward] = f; }
	};

} }

#endif