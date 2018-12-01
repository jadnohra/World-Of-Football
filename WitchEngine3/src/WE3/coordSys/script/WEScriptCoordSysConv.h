#ifndef _WEScriptCoordSysConv_h
#define _WEScriptCoordSysConv_h

#include "../WECoordSysConv.h"
#include "../../script/includeScriptingAPI.h"
#include "../../WERef.h"

namespace WE {
	
	class ScriptCoordSysConv : public HardRef<CoordSysConv> {
	public:

		ScriptCoordSysConv(CoordSysConv* pRef = NULL);
	
	public:

		inline void script_convVector3(Vector3& val) { ptr()->toTargetVector(val.el); }
		inline void script_convPoint3(Vector3& val) { ptr()->toTargetPoint(val.el); }
		inline float script_convUnit(float val) { ptr()->toTargetUnits(val); return val; }
		inline float script_invConvUnit(float val) { ptr()->fromTargetUnits(val); return val; }

		void script_convVector3Units(Vector3& val) { 
			
			ptr()->toTargetUnits(val.el[0]); 
			ptr()->toTargetUnits(val.el[1]); 
			ptr()->toTargetUnits(val.el[2]); 
		}

	public:

		ScriptCoordSysConv(const ScriptCoordSysConv& ref) : HardRef(ref) {}
		inline ScriptCoordSysConv& operator=(const ScriptCoordSysConv& ref) { set(ref.ptr()); return *this; }

	public:

		static const TCHAR* ScriptClassName;

		static void declareInVM(SquirrelVM& target);
	};

	inline ScriptCoordSysConv scriptArg(CoordSysConv& val) { return ScriptCoordSysConv(&val); }
	inline ScriptCoordSysConv scriptArg(CoordSysConv* val) { return ScriptCoordSysConv(val); }
}

#endif