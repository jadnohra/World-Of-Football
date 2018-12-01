#ifndef _WEDeclareClasses_h_coordSys
#define _WEDeclareClasses_h_coordSys

#include "WEScriptCoordSys.h"
#include "WEScriptCoordSysConv.h"

namespace WE { namespace coordSys {

	class DeclareClasses {
	public:

		static void declareInVM(SquirrelVM& target) {

			ScriptCoordSys::declareInVM(target);
			ScriptCoordSysConv::declareInVM(target);
		}
	};

} }

#endif