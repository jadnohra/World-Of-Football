#ifndef _WOFMatchControlSwitchFootballerState_h
#define _WOFMatchControlSwitchFootballerState_h

#include "float.h"

#include "../WOFMatchTime.h"

namespace WOF { namespace match { 

	class Footballer;
	class Footballer_Brain;

	struct SwitchControl {

		enum ValueID {

			VID_PlayerControlled = -5, VID_BallOwnership = -4, VID_CurrBallDist = -3, 
			VID_CurrBallRelHeight = -2, VID_CurrBallRelAngle = -1, 
			VID_BallPathInterceptTime = 0, VID_BallPathInterceptDist, VID_RestBallDist, 
			VID_Count, VID_None
		};

		struct ValueValidityState {

			bool isValid[VID_Count];
		};

		struct Value {

			bool isValid;
			float val;

			Time time;
			Time endTime;

			Value() : isValid(false), endTime(FLT_MAX) {}
		};

		struct Value_PathIntercept {

			SimulationID simulID;

			bool isValidIntercept;
			float interceptTime;
			float interceptDist;

			bool isValidRestBall;
			float restBallDist;
		};

		//OK SOME VALUEID's dont need to be stored here 
		//since we already have them realtime like playercontrolled
		//and ballownership so make the values in form of function calls
		//whith footballer as attribute which will use real time values if possible

		struct FootballerState {

			Value computed[VID_Count];

			bool getValue(Footballer_Brain& footballer, const ValueID& ID, float& value) const; //returns isValid
			bool getValue(Footballer_Brain& footballer, const ValueID& ID, const Time& currTime, float& value, Time& time) const; //returns isValid

			void getValue_PlayerControlled(Footballer_Brain& footballer, float& value) const; 
			void getValue_PlayerControlled(Footballer_Brain& footballer, const Time& currTime, float& value, Time& time) const; 

			void getValue_BallOwnership(Footballer_Brain& footballer, float& value) const; 
			void getValue_BallOwnership(Footballer_Brain& footballer, const Time& currTime, float& value, Time& time) const; 

			void getValue_CurrBallDist(Footballer_Brain& footballer, float& value) const; 
			void getValue_CurrBallDist(Footballer_Brain& footballer, const Time& currTime, float& value, Time& time) const; 

			void getValue_CurrBallRelHeight(Footballer_Brain& footballer, float& value) const; 
			void getValue_CurrBallRelHeight(Footballer_Brain& footballer, const Time& currTime, float& value, Time& time) const; 

			void getValue_CurrBallRelAngle(Footballer_Brain& footballer, float& value) const; 
			void getValue_CurrBallRelAngle(Footballer_Brain& footballer, const Time& currTime, float& value, Time& time) const; 

			bool getValue_BallPathInterceptTime(Footballer_Brain& footballer, float& value) const; 
			bool getValue_BallPathInterceptTime(Footballer_Brain& footballer, const Time& currTime, float& value, Time& time) const; 

			bool getValue_BallPathInterceptDist(Footballer_Brain& footballer, float& value) const; 
			bool getValue_BallPathInterceptDist(Footballer_Brain& footballer, const Time& currTime, float& value, Time& time) const; 

			bool getValue_RestBallDist(Footballer_Brain& footballer, float& value) const; 
			bool getValue_RestBallDist(Footballer_Brain& footballer, const Time& currTime, float& value, Time& time) const; 
		};

	};


} }

#endif