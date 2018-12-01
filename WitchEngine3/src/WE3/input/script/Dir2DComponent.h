#ifndef _WEDir2DComponent_h_Input
#define _WEDir2DComponent_h_Input

#include "../Component_Dir2D.h"

#include "../../WEPtr.h"
#include "../../script/WEScriptEngine.h"

namespace WE { namespace Input {
	
	class ScriptTracker;
	class ScriptInputManager;

	class ScriptDir2D : public SoftPtr<Component_Dir2D> {
	public:

		ScriptDir2D(Component_Dir2D* pRef = NULL) : SoftPtr<Component_Dir2D>(pRef) {}

	public:

		bool script_isValid() { return isValid(); }
		
		inline float script_getValue1() { return dref().getValue1(); }
		inline float script_getValue2() { return dref().getValue2(); }

		inline float script_getRawValue1() { return dref().getRawValue1(); }
		inline float script_getRawValue2() { return dref().getRawValue2(); }

		inline void script_setRawValuesAsDeadZones(bool keepMax, bool unite) { dref().setRawValuesAsDeadZones(keepMax, unite); }

		inline ScriptResponseCurve script_createResponseCurve1() { return scriptArg(dref().createResponseCurve1()); }
		inline ScriptResponseCurve script_createResponseCurve2() { return scriptArg(dref().createResponseCurve2()); }

		inline void script_destroyResponseCurve1() { return dref().destroyResponseCurve1(); }
		inline void script_destroyResponseCurve2() { return dref().destroyResponseCurve2(); }

		inline ScriptResponseCurve script_getResponseCurve1() { return scriptArg(dref().getResponseCurve1()); }
		inline ScriptResponseCurve script_getResponseCurve2() { return scriptArg(dref().getResponseCurve2()); }
		
	public:

		ScriptDir2D(const ScriptDir2D& ref) : SoftPtr<Component_Dir2D>(ref.ptr()) {}
		inline ScriptDir2D& operator=(const ScriptDir2D& ref) { set(ref.ptr()); return *this; }

	public:

		static const TCHAR* ScriptClassName;

		static void declareInVM(SquirrelVM& target);
	};

	inline ScriptDir2D scriptArg(Component_Dir2D& val) { return ScriptDir2D(&val); }
	inline ScriptDir2D scriptArg(Component_Dir2D* val) { return ScriptDir2D(val); }


} }

#endif