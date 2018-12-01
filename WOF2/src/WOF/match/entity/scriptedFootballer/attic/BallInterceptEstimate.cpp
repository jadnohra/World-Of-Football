#include "BallInterceptEstimate.h"

DECLARE_INSTANCE_TYPE_NAME(WOF::match::BallInterceptEstimate, CBallInterceptEstimate);

namespace WOF { namespace match {

const TCHAR* BallInterceptEstimate::ScriptClassName = L"CBallInterceptEstimate";


void BallInterceptEstimate::declareInVM(SquirrelVM& target) {

	SqPlus::SQClassDef<BallInterceptEstimate>(ScriptClassName).
		func(&BallInterceptEstimate::script_isValid, L"isValid").
		func(&BallInterceptEstimate::script_invalidate, L"invalidate").
		func(&BallInterceptEstimate::script_isSimulated, L"isSimulated").
		func(&BallInterceptEstimate::script_isResting, L"isResting").
		func(&BallInterceptEstimate::script_isInstantaneous, L"isInstantaneous").
		func(&BallInterceptEstimate::script_getVel, L"getVel").
		func(&BallInterceptEstimate::script_getPos, L"getPos");
}

} }