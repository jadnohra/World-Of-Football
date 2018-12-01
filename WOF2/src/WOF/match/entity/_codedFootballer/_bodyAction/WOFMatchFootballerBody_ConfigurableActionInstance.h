#ifndef _WOFMatchFootballerBody_ConfigurableActionInstance_h
#define _WOFMatchFootballerBody_ConfigurableAction_h

#include "WOFMatchFootballerBody_ConfigurableAction.h"

namespace WOF { namespace match {

	class FootballerBody_ConfigurableActionInstance {
	public:

		void start(Footballer_Body& body, const Vector3& bodyAcc, const Vector3& bodyVel, const Time& currTime);
		void executeCommands(Footballer_Body& body, const Time& currTime, TimedCommandIter& iter, CommandProcessor* pCommandProcessor);

	public:

		BodyDynamics mBodyDynamics;
		TimedCommandIter mCommandIter;
	};

} }