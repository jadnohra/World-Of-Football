#include "WOFMatchFootballerBody_ConfigurableActionInstance.h"
#include "../WOFMatchFootballer_Body.h"


namespace WOF { namespace match {

/*
void FootballerBody_ConfigurableActionInstance::start(Footballer_Body& body, const Vector3& bodyAcc, const Vector3& bodyVel, const Time& currTime) {

	mBodyDynamics.acc = bodyAcc;
	mBodyDynamics.vel = bodyVel;
}

void FootballerBody_ConfigurableAction::executeCommands(const Time& currTime, TimedCommandIter& iter, CommandProcessor* pCommandProcessor) {

	for (Commands::Index i = iter.nextCommandIndex; i < mCommands.count; ++i) {

		Command& command = mCommands[i];

		if (command.mTime <= currTime) {

			switch (command.getType()) {

				case Command_PlaySound: {

					dref(pCommandProcessor).playSound(currTime);

				} break;

				case Command_StartDynamics: {

					StockCommand_Dynamics& com = (StockCommand_Dynamics&) command;

					com.getDynamics().start(body, const Vector3& startAcc, const Vector3& startVel, currTime);

				} break;

				default: {

					dref(pCommandProcessor).executeCommand(currTime, command);

				} break;
			}

			++iter.nextCommandIndex;

		} else {

			break;
		}
	}
}
*/

} }