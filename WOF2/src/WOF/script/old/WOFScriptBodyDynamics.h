#ifndef _WOFScriptBodyDynamics_h
#define _WOFScriptBodyDynamics_h

#include "WE3/WETime.h"
using namespace WE;

#include "WOFScriptEngine.h"
#include "WEScriptDataChunk.h"
#include "WEScriptCoordSysConv.h"
#include "WEScriptVector3.h"
#include "WEScriptArgByRef.h"

namespace WOF {


	class ScriptBodyDynamics {
	public:

		bool create(ScriptEngine& engine, const TCHAR* className);

		inline const SquirrelObject& toScriptArg() { return mScriptObject; }

	public:

		bool load(ScriptEngine& engine, DataChunk* pChunk, CoordSysConv* pConv);
		bool isSimulable(ScriptEngine& engine);
		bool hasRestingState(ScriptEngine& engine, const Vector3& startAcc, const Vector3& startVel);
		bool simulate(ScriptEngine& engine, const Time& tickLength, Vector3& inOutAcc, Vector3& inOutVel, Time& t);

		void start(ScriptEngine& engine, const Time& t, Vector3& startAcc, Vector3& startVel);
		void stop(ScriptEngine& engine, const Time& t);

		bool frameMove(ScriptEngine& engine, const Time& t, const Time& dt, Vector3& currAcc, Vector3& currVel);

	public:

	protected:

		SquirrelObject mScriptObject;
	};

}

#endif