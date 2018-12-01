#include "ScriptEngine.h"

#include "DeclareClasses.h"

namespace WOF {

bool ScriptEngine::init(Log* pScriptOutLog, Console* pScriptOutConsole, const TCHAR* pScriptPath) {

	if (WE::ScriptEngine::init(pScriptOutLog, pScriptOutConsole, pScriptPath)) {

		DeclareClasses::declareInVM(getVM());

		return true;
	}

	return false;
}

}