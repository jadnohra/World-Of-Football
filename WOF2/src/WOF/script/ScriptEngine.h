#ifndef h_WOF_ScriptEngine
#define h_WOF_ScriptEngine

#include "WE3/script/WEScriptEngine.h"
using namespace WE;

namespace WOF {

	class ScriptEngine : public WE::ScriptEngine {
	public:

		bool init(Log* pScriptOutLog, Console* pScriptOutConsole, const TCHAR* pScriptPath);
	};
}

#endif