#ifndef _WEScriptSource_h_Input
#define _WEScriptSource_h_Input

#include "../SourceHolder.h"

#include "../../script/WEScriptEngine.h"

namespace WE { namespace Input {
	
	class ScriptTracker;
	class ScriptController;
	class ScriptInputManager;
	class ScriptButton;
	class ScriptGeneric1D;
	class ScriptDir2D;

	class ScriptSource : public SourceHolder {
	public:

		ScriptSource(Source* pRef = NULL) : SourceHolder(pRef) {}

		inline operator Source*() { return ptr(); }

	public:

		inline bool script_isValid() { return isValid(); } 

		inline Time script_getTime() { return dref().getTime(); }
		inline const TCHAR* script_getName() { return dref().getName(); }
		bool script_addTracker(ScriptInputManager* pManager, ScriptTracker* pTracker);
		bool script_removeTracker(ScriptInputManager* pManager, ScriptTracker* pTracker);

		ScriptSource script_getBaseSource();

		ScriptSource script_createNegative(ScriptInputManager* pManager, const TCHAR* name);
		static ScriptSource script_createAxisFrom2Sources(ScriptInputManager* pManager, ScriptSource* pNeg, ScriptSource* pPos, const TCHAR* name);
		static ScriptSource script_createDir2DFrom2Axis(ScriptInputManager* pManager, ScriptSource* pAxis1, ScriptSource* pAxis2, const TCHAR* name);
		static ScriptSource script_createConstant(ScriptInputManager* pManager, const TCHAR* name, float value);
		ScriptSource script_createButton(ScriptInputManager* pManager, const TCHAR* name);
		static ScriptSource script_createAnd(ScriptInputManager* pManager, ScriptSource* pArg1, ScriptSource* pArg2, const TCHAR* name);

		ScriptButton script_getButton();
		ScriptGeneric1D script_getGeneric1D();
		ScriptDir2D script_getDir2D();

		bool script_hasComponent(int compID);
		bool script_isButton();
		bool script_isGeneric1D();
		bool script_isDir2D();

	public:

		ScriptSource(const ScriptSource& ref) : SourceHolder(ref.ptr()) {}
		inline ScriptSource& operator=(const ScriptSource& ref) { set(ref.ptr()); return *this; }

	public:

		static const TCHAR* ScriptClassName;

		static void declareInVM(SquirrelVM& target);
	};

	inline ScriptSource scriptArg(Source& val) { return ScriptSource(&val); }
	inline ScriptSource scriptArg(Source* val) { return ScriptSource(val); }


} }

#endif