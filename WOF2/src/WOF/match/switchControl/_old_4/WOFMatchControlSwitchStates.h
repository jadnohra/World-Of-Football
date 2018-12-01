#ifndef _WOFMatchControlSwitchStates_h
#define _WOFMatchControlSwitchStates_h

#include "float.h"

#include "../WOFMatchBallSimulManager.h"
#include "../WOFMatchTime.h"

namespace WOF { namespace match { 

	class Footballer;
	class Footballer;

	struct SwitchControl {

		struct ValueValidity {

			bool isValidPathIntercept;
		};

		struct PathInterception {

			bool isValid;
			float interceptTime;
			float interceptDist;

			PathInterception();

			bool checkValidity(const Time& currTime); //will invalidate if time passed
			bool equals(const PathInterception& comp, const Time& timeTolerance);
		};

		struct Value_PathIntercept {

			SimulationID simulID;

			//an addition step would be that in addition to registering
			//those 2 intercepts use an array to register ALL intercepts
			//but for now we assume a ball trajectory has too short a time for this
			//to be really useful
			PathInterception shortestTimeIntercept;
			PathInterception shortestDistIntercept;

			PathInterception restBallIntercept;

			bool isValid(BallSimulManager::Simul* pSimul) const;
		};

		struct FootballerState {

			Value_PathIntercept value_pathIntercept;
			float switchScore;

			float playerScoreInfluence;

			inline float getTotalScore() { return switchScore + playerScoreInfluence; }

			FootballerState();

			void getValue_PlayerControlled(Footballer& footballer, float& value) const; 
			void getValue_BallOwnership(Footballer& footballer, float& value) const; 
			void getValue_CurrBallDist(Footballer& footballer, float& value) const; 
			void getValue_CurrBallRelHeight(Footballer& footballer, float& value) const; 
			void getValue_CurrBallRelAngle(Footballer& footballer, float& value) const; 

			Value_PathIntercept* getValue_PathIntercept(Footballer& footballer); 
			Value_PathIntercept* getValue_PathIntercept(Footballer& footballer, BallSimulManager::Simul* pSimul); 
		};

		struct PlayerState {

			bool doSwitch;
		};
	};


} }

#endif