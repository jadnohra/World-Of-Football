#ifndef _WEScriptArgByRef_h
#define _WEScriptArgByRef_h

#include "WEScriptEngine.h"

namespace WE {

	template<typename T>
	class ScriptArgByRef {
	public:

		ScriptArgByRef(T* pRef = NULL) : mRef(pRef) {}
		ScriptArgByRef(T& ref) : mRef(&ref) {}

		inline T& value() { return mRef.dref(); }

	public:

		static void declareInVM(SquirrelVM& target) {

			SqPlus::SQClassDef< ScriptArgByRef<T> >(getScriptArgByRefClassName<T>()).
				func(&ScriptArgByRef::script_set, L"set").
				func(&ScriptArgByRef::script_get, L"get");
		}

		inline void script_set(T val) { mRef.dref() = val; }
		inline T script_get() { return mRef.dref(); }
		
	public:

		//POD copy allowed

	protected:

		SoftPtr<T> mRef;
	};

	template<typename T>
	inline const TCHAR* getScriptArgByRefClassName();

	template<>
	inline const TCHAR* getScriptArgByRefClassName<float>() { return L"FloatByRef"; }

	template<>
	inline const TCHAR* getScriptArgByRefClassName<int>() { return L"IntByRef"; }

	typedef ScriptArgByRef<float> ScriptArgByRef_float;
	typedef ScriptArgByRef<int> ScriptArgByRef_int;

	inline ScriptArgByRef<float> scriptArg(float* val) { return ScriptArgByRef_float(val); }
	inline ScriptArgByRef<int> scriptArg(int* val) { return ScriptArgByRef_int(val); }



	template<typename T>
	class ScriptCArgByRef {
	public:

		ScriptCArgByRef(T val = (T) 0) : mValue(val) {}

		inline T& value() { return mValue; }

	public:

		static void declareInVM(SquirrelVM& target) {

			SqPlus::SQClassDef< ScriptCArgByRef<T> >(getScriptCArgByRefClassName<T>()).
				staticFunc(&ScriptCArgByRef::construct, L"constructor").
				func(&ScriptCArgByRef::script_set, L"set").
				func(&ScriptCArgByRef::script_get, L"get");
		}

		inline void script_set(T val) { mValue = val; }
		inline T script_get() { return mValue; }
		
		static int construct(HSQUIRRELVM v) {

			StackHandler sa(v);
			int paramCount = sa.GetParamCount();
			
			if (paramCount >= 2) {

				ScriptObject value;
				value.AttachToStackObject(2);

				if (value.IsNull())
					return sq_throwerror(v,_T("Invalid Constructor arguments"));

				SoftPtr<ScriptCArgByRef<T> > newObj;
				WE_MMemNew(newObj.ptrRef(), ScriptCArgByRef<T>(value.Get<T>()));

				return SqPlus::PostConstruct<ScriptCArgByRef<T> >(v, newObj, release);

			} else {

				SoftPtr<ScriptCArgByRef<T> > newObj;
				WE_MMemNew(newObj.ptrRef(), ScriptCArgByRef<T>());

				return SqPlus::PostConstruct<ScriptCArgByRef<T> >(v, newObj, release);
			}

			return sq_throwerror(v,_T("Invalid Constructor arguments"));
		} 

		static int release(SQUserPointer up, SQInteger size) { SQ_DELETE_CLASS(ScriptCArgByRef<T>); } 

	public:

		//POD copy allowed

	protected:

		T mValue;
	};

	template<typename T>
	inline const TCHAR* getScriptCArgByRefClassName();

	template<>
	inline const TCHAR* getScriptCArgByRefClassName<float>() { return L"Float"; }

	template<>
	inline const TCHAR* getScriptCArgByRefClassName<int>() { return L"Int"; }

	template<>
	inline const TCHAR* getScriptCArgByRefClassName<bool>() { return L"Bool"; }

	typedef ScriptCArgByRef<float> ScriptFloat;
	typedef ScriptCArgByRef<int> ScriptInt;
	typedef ScriptCArgByRef<bool> ScriptBool;
}

#endif