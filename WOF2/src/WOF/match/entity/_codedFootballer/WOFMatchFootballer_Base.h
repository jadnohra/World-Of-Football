#ifndef _WOFMatchFootballer_Base_h
#define _WOFMatchFootballer_Base_h

#include "../../WOFMatchDataTypes.h"
#include "../../WOFMatchTime.h"
#include "../../switchControl/WOFMatchControlSwitchStates.h"

namespace WOF { namespace match {


	class Footballer_Base {
	public:
		
		TeamEnum mTeam;
		FootballerNumber mNumber;

		SwitchControl::FootballerState mSwitchState;
	};

} }

#endif