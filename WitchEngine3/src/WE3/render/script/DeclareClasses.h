#ifndef _WEDeclareClasses_h_Math
#define _WEDeclareClasses_h_Math

#include "../../script/WEScriptEngine.h"

#include "ScriptRenderColor.h"
#include "ScriptRenderLight.h"


namespace WE { namespace Render {

	class DeclareClasses {
	public:

		static void declareInVM(SquirrelVM& target) {

			ScriptRenderColor::declareInVM(target);
			ScriptRenderLight::declareInVM(target);
		}
	};

} }

#endif