#ifndef _WEScriptDataChunk_h
#define _WEScriptDataChunk_h

#include "../data/WEDataSource.h"
using namespace WE;

#include "includeScriptingAPI.h"
#include "../math/script/WEScriptVector3.h"
#include "../coordSys/script/WEScriptCoordSysConv.h"

namespace WE {
	
	class ScriptDataChunk : public HardRef<DataChunk> {
	public:

		ScriptDataChunk::ScriptDataChunk(DataChunk* pRef = NULL) : HardRef(pRef) {}

	public:

		inline bool script_isValid() { return isValid(); }

		bool script_hasChild(const TCHAR* name);
		bool script_toChild(const TCHAR* name);

		ScriptDataChunk script_getChild(const TCHAR* name);
		
		bool script_is(const TCHAR* tagName);
		bool script_has(const TCHAR* varName);
		const TCHAR* script_get(const TCHAR* varName);

		bool script_getVector3(const TCHAR* varName, Vector3* pValue, ScriptCoordSysConv* pConv);
		int script_geti(const TCHAR* varName, int defaultVal);
		float script_getf(const TCHAR* varName, float defaultVal);

	public:

		ScriptDataChunk(const ScriptDataChunk& ref) : HardRef(ref.ptr()) {}
		inline ScriptDataChunk& operator=(const ScriptDataChunk& ref) { set(ref.ptr()); return *this; }

	public:

		static const TCHAR* ScriptClassName;

		static void declareInVM(SquirrelVM& target);

	protected:

		BufferString mStringHolder;
	};

	inline ScriptDataChunk scriptArg(DataChunk& val) { return ScriptDataChunk(&val); }
	inline ScriptDataChunk scriptArg(DataChunk* val) { return ScriptDataChunk(val); }
}

#endif