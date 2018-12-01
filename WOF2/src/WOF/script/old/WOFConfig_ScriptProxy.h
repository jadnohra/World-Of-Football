#ifndef _WOFConfig_ScriptProxy_h
#define _WOFConfig_ScriptProxy_h

#include "WE3/data/WEDataSource.h"
using namespace WE;

#include "includeScriptingAPI.h"

namespace WOF {
	
	class Config_ScriptProxy {
	public:

		static const TCHAR* ScriptClassName;

		static void declareScriptClass(SquirrelVM& target);

	public:


		Config_ScriptProxy(DataChunk* pChunk = NULL);

		bool has(const SQChar *varName);
		const SQChar* get(const SQChar *varName);

	protected:

		HardRef<DataChunk> mChunk;
		BufferString mStringHolder;
	};
}

#endif