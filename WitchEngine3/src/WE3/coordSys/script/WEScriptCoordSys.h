#ifndef _WEScriptCoordSys_h
#define _WEScriptCoordSys_h

#include "../WECoordSys.h"
#include "../../script/includeScriptingAPI.h"
#include "../../WEPtr.h"
#include "../../WEMacros.h"

namespace WE {
	
	class ScriptCoordSys : public SoftPtr<FastUnitCoordSys> {
	public:

		ScriptCoordSys(FastUnitCoordSys* pRef = NULL);
	
	public:

		inline float script_convUnit(float val) { return dref().convUnit(val); }
		inline float script_invConvUnit(float val) { return dref().invConvUnit(val); }

		inline void script_convVector3Units(Vector3* pVec3) { dref().convVectorUnits(WE::dref(pVec3).el); }
		inline void script_invConvVector3Units(Vector3* pVec3) { dref().invConvVectorUnits(WE::dref(pVec3).el); }

		inline void script_setRUF(float r, float u, float f, Vector3* pTarget) { dref().setRUF(r, u, f, WE::dref(pTarget).el); }
		

	public:

		ScriptCoordSys(const ScriptCoordSys& ref) : SoftPtr(ref) {}
		inline ScriptCoordSys& operator=(const ScriptCoordSys& ref) { set(ref.ptr()); return *this; }

	public:

		static const TCHAR* ScriptClassName;

		static void declareInVM(SquirrelVM& target);
	};

	inline ScriptCoordSys scriptArg(FastUnitCoordSys& val) { return ScriptCoordSys(&val); }
	inline ScriptCoordSys scriptArg(FastUnitCoordSys* val) { return ScriptCoordSys(val); }
}

#endif