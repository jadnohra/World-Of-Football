#ifndef _WOFMatchFootballerBody_ConfigurableActionDynamics_Scripted_h
#define _WOFMatchFootballerBody_ConfigurableActionDynamics_Scripted_h

#include "WOFMatchFootballerBody_ConfigurableAction.h"
#include "../../../../script/WOFScriptBodyDynamics.h"

namespace WOF { namespace match {


	class FootballerBody_ConfigurableActionDynamics_Scripted : public FootballerBody_ConfigurableAction::Dynamics {
	public:

		virtual bool load(Footballer_Body& body, BufferString& tempStr, DataChunk& chunk, CoordSysConv* pConv);

		virtual bool isSimulable(Footballer_Body& body);
		virtual bool hasRestingState(Footballer_Body& body, const Vector3& startAcc, const Vector3& startVel);
		virtual bool simulate(Footballer_Body& body, const Time& tickLength, Vector3& inOutAcc, Vector3& inOutVel, Time& t);

		virtual void start(Footballer_Body& body, const Time& t, Vector3& startAcc, Vector3& startVel);
		virtual void stop(Footballer_Body& body, const Time& t);

		virtual bool frameMove(Footballer_Body& body, const Time& t, const Time& dt, Vector3& currAcc, Vector3& currVel);

	protected:

		ScriptBodyDynamics mImpl;
	};

} }

#endif