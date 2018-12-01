#ifndef _WEResponseCurve_h
#define _WEResponseCurve_h


#include "../script/WEScriptEngine.h"

namespace WE { 

	class ResponseCurve {
	public:

		ResponseCurve() {}
		ResponseCurve(float curvature, float min = 0.0f, float max = 1.0f, float deadZone = 0.0f, float saturation = 1.0f, bool isMirrored = true);

		void setCurvature(float value);
		void setDeadZone(float value);
		float getDeadZone();

		float map(float in);
		
	protected:

		bool mIsMirrored;

		float mMin;
		float mMax;
		float mDeadZone;
		float mSaturation;
		float mCurvature;

	public:

		static const TCHAR* ScriptClassName;

		static void declareInVM(SquirrelVM& target);
	
	public:

		static int script_construct(HSQUIRRELVM v);
		static int release(SQUserPointer up, SQInteger size) { SQ_DELETE_CLASS(ResponseCurve); } 
	};

	typedef ResponseCurve* ScriptResponseCurve;

	inline ResponseCurve* scriptArg(ResponseCurve& val) { return &val; }
	inline ResponseCurve* scriptArg(ResponseCurve* val) { return val; }
} 

#endif