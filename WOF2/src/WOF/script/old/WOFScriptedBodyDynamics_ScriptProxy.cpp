#include "WOFScriptedBodyDynamics_ScriptProxy.h"

#include "WEVector3_ScriptClassDecl.h"
#include "WOFConfig_ScriptProxy.h"

namespace WOF {

bool ScriptedBodyDynamics_ScriptProxy::create(ScriptingEngine& engine, const TCHAR* className) {

	if (mScriptInstance.isNull()) {

		if (engine.createBodyDynamics(className, mScriptInstance)) {

			{
				SqPlus::SquirrelFunction<bool> fct(mScriptInstance.get(), L"load");
				
				Config_ScriptProxy config;
				
				bool result = (fct)(&config, NULL);

				result = result;
			}
		}
	}

	assrt(false);
	return false;
}

}