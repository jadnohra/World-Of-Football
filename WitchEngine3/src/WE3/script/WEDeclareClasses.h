#ifndef _WEDeclareClasses_h
#define _WEDeclareClasses_h

#include "WEScriptEngine.h"

#include "WEScriptArgByRef.h"
#include "WEScriptDataChunk.h"
#include "../math/script/DeclareClasses.h"
#include "../scene/script/DeclareClasses.h"
#include "../coordSys/script/DeclareClasses.h"

namespace WE {

	class DeclareClasses {
	public:

		static void declareInVM(SquirrelVM& target) {

			ScriptArgByRef_float::declareInVM(target);
			ScriptArgByRef_int::declareInVM(target);

			ScriptFloat::declareInVM(target);
			ScriptInt::declareInVM(target);
			ScriptBool::declareInVM(target);

			math::DeclareClasses::declareInVM(target);
			scene::DeclareClasses::declareInVM(target);
			coordSys::DeclareClasses::declareInVM(target);

			ScriptDataChunk::declareInVM(target);
		}
	};

}

#endif