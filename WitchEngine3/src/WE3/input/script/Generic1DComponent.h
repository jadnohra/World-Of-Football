#ifndef _WEGeneric1DComponent_h_Input
#define _WEGeneric1DComponent_h_Input

#include "../Component_Generic1D.h"

#include "../../WEPtr.h"
#include "../../script/WEScriptEngine.h"

namespace WE { namespace Input {
	
	class ScriptTracker;
	class ScriptInputManager;

	class ScriptGeneric1D : public SoftPtr<Component_Generic1D> {
	public:

		ScriptGeneric1D(Component_Generic1D* pRef = NULL) : SoftPtr<Component_Generic1D>(pRef) {}

	public:

		bool script_isValid() { return isValid(); }
		
		inline bool script_isDiscrete() { return dref().isDiscrete(); }
		inline bool script_isContinuous() { return dref().isContinuous(); }
		inline int script_getStepCount() { return dref().getStepCount(); }
		inline float script_getMin() { return dref().getMin(); }
		inline float script_getMax() { return dref().getMax(); }
		inline float script_getValue() { return dref().getValue(); }

	public:

		ScriptGeneric1D(const ScriptGeneric1D& ref) : SoftPtr<Component_Generic1D>(ref.ptr()) {}
		inline ScriptGeneric1D& operator=(const ScriptGeneric1D& ref) { set(ref.ptr()); return *this; }

	public:

		static const TCHAR* ScriptClassName;

		static void declareInVM(SquirrelVM& target);
	};

	inline ScriptGeneric1D scriptArg(Component_Generic1D& val) { return ScriptGeneric1D(&val); }
	inline ScriptGeneric1D scriptArg(Component_Generic1D* val) { return ScriptGeneric1D(val); }


} }

#endif