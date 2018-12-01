#ifndef _WOFMatchControlSwitchStates_h
#define _WOFMatchControlSwitchStates_h

#include "float.h"

#include "../entity/ball/ballSimul/WOFMatchBallSimulation.h"
#include "../WOFMatchTime.h"

namespace WOF { namespace match { 

	class Footballer;

	struct SwitchControl {

		struct PathInterception {

			bool isValid;
			bool isExpired;

			float interceptTime;
			float interceptDist;

			PathInterception();

			//bool checkValidity(const Time& currTime); //return ok if valid and not expired
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

			bool isValid(BallSimulation* pSimul) const;
			float getInterceptTime(const Time& currTime) const;
			float getInterceptDist(const Time& currTime) const;
		};

		struct FootballerState {

			Value_PathIntercept value_pathIntercept;
			float switchScore;

			FootballerState();

			float getTotalScore(Footballer& footballer);
			float getPlayerScoreInfluence(Footballer& footballer);

			void getValue_PlayerControlled(Footballer& footballer, float& value) const; 
			void getValue_BallOwnership(Footballer& footballer, float& value) const; 
			void getValue_CurrBallDist(Footballer& footballer, float& value) const; 
			void getValue_CurrBallRelHeight(Footballer& footballer, float& value) const; 
			void getValue_CurrBallRelAngle(Footballer& footballer, float& value) const; 

			Value_PathIntercept* getValue_PathIntercept(Footballer& footballer); 
			Value_PathIntercept* getValue_PathIntercept(Footballer& footballer, BallSimulation* pSimul); 
		};

		struct PlayerState {

			bool isSignaledShot;
			SimulationID signaledShotSimulID;
			int shotSwitchControlValue;
			float footballerScoreInfluence;

			PlayerState() : isSignaledShot(false) {}
		};
	};


} }

#endif