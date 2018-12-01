#include "WEResponseCurve.h"

#include "../WEMacros.h"
#include "../WEMem.h"

DECLARE_INSTANCE_TYPE_NAME(WE::ResponseCurve, ResponseCurve);

namespace WE {

const TCHAR* ResponseCurve::ScriptClassName = L"ResponseCurve";

void ResponseCurve::declareInVM(SquirrelVM& target) {

	SqPlus::SQClassDef<ResponseCurve>(ScriptClassName).
		staticFunc(&ResponseCurve::script_construct, L"constructor").
		func(&ResponseCurve::setCurvature, L"setCurvature").
		func(&ResponseCurve::setDeadZone, L"setDeadZone").
		func(&ResponseCurve::getDeadZone, L"getDeadZone").
		func(&ResponseCurve::map, L"map");
}

int ResponseCurve::script_construct(HSQUIRRELVM v) {

	StackHandler sa(v);
	int paramCount = sa.GetParamCount();
	
	float curvature = 1.0f;
	float min = 0.0f;
	float max = 1.0f;
	float deadZone = min;
	float saturation = max; 
	bool isMirrored = true;

	int paramIndex = 2;
	ScriptObject arg;

	if (paramIndex <= paramCount) {

		arg.AttachToStackObject(paramIndex);
		curvature = arg.Get<float>();

		++paramIndex;
	}

	if (paramIndex <= paramCount) {

		arg.AttachToStackObject(paramIndex);
		min = arg.Get<float>();

		++paramIndex;
	}

	if (paramIndex <= paramCount) {

		arg.AttachToStackObject(paramIndex);
		max = arg.Get<float>();

		++paramIndex;
	}

	if (paramIndex <= paramCount) {

		arg.AttachToStackObject(paramIndex);
		deadZone = arg.Get<float>();

		++paramIndex;
	}

	if (paramIndex <= paramCount) {

		arg.AttachToStackObject(paramIndex);
		saturation = arg.Get<float>();

		++paramIndex;
	}

	if (paramIndex <= paramCount) {

		arg.AttachToStackObject(paramIndex);
		isMirrored = arg.Get<bool>();

		++paramIndex;
	}

	SoftPtr<ResponseCurve> newObj;
	WE_MMemNew(newObj.ptrRef(), ResponseCurve(curvature, min, max, deadZone, saturation, isMirrored));

	return SqPlus::PostConstruct<ResponseCurve>(v, newObj, release);
} 


ResponseCurve::ResponseCurve(float curvature, float min, float max, float deadZone, float saturation, bool isMirrored) {

	mCurvature = curvature;
	mMin = min;
	mMax = max;
	mDeadZone = deadZone;
	mSaturation = saturation;
	mIsMirrored = isMirrored;
}

void ResponseCurve::setCurvature(float value) {

	mCurvature = value;
}

void ResponseCurve::setDeadZone(float value) {

	mDeadZone = fabs(value);
}

float ResponseCurve::getDeadZone() {

	return mDeadZone;
}

float ResponseCurve::map(float in) {

	bool mirrored;

	if (mIsMirrored && !sameSgnZeroTolerantT(in, mMax)) {

		in = -in;
		mirrored = true;

	} else {

		mirrored = false;
	}

	if (in <= mDeadZone) {

		return mirrored ? -mMin : mMin;
	}

	if (in <= mMin) {

		return mirrored ? -mMin : mMin;
	}

	if (in >= mMax) {

		return mirrored ? -mMax : mMax;
	}
	
	if (mMax - mMin == 1.0f) {

		in = powf(in, mCurvature);

	} else {

		float range = mMax - mMin;

		in = (in - mMin) / range;

		in = powf(in, mCurvature);

		in = mMin + in * range;
	}

	if (in <= mMin || in <= mDeadZone) {

		return mirrored ? -mMin : mMin;
	}

	if (in >= mSaturation || in >= mMax) {

		return mirrored ? -mMax : mMax;
	}

	return mirrored ? -in : in;
}

}