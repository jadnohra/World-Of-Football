#ifndef _BallInterceptEstimate_h
#define _BallInterceptEstimate_h

#include "WE3/math/WEIntersection.h"
#include "WE3/script/WEScriptEngine.h"
using namespace WE;

#include "../ball/ballSimul/BallSimulation2.h"

namespace WOF { namespace match {

	struct BallInterceptEstimate {

		bool isValid;

		Vector3 position;
		Vector3 velocity;

		bool isSimulated;
		float simulExpiryTime;
		
		bool isResting;
		
		union {

			struct {

				unsigned int simulID; //isSimulated
			};
		
			struct {
				
				unsigned int restingID; //isResting
			};
		};

		BallInterceptEstimate() : isValid(false) {}

		inline bool script_isValid() { return isValid; }
		inline void script_invalidate() { isValid = false; }
		inline bool script_isSimulated() { return isSimulated; }
		inline bool script_isResting() { return isResting; }
		inline bool script_isInstantaneous() { return !isSimulated && !isResting; }
		inline CtVector3* script_getPos() { return &position; }
		inline CtVector3* script_getVel() { return &velocity; }

	public:

		static const TCHAR* ScriptClassName;

		static void declareInVM(SquirrelVM& target);
	};

} }

#endif