#ifndef h_WOF_match_ControlSwitchStates
#define h_WOF_match_ControlSwitchStates

#include "float.h"

#include "../Clock.h"

namespace WOF { namespace match { 

	class Footballer;
	class BallSimulation;

	struct SwitchControl {

		//typedef BallSimulation::Index SampleIndex;
		//typedef BallSimulation::Sample Sample;

		/*
		struct PathInterceptState : PathIntercept {
		};
		*/

		struct FootballerState {

			FootballerState();

			float getTotalScore(Footballer& footballer);
			float getPlayerScoreInfluence(Footballer& footballer);

			void getValue_PlayerControlled(Footballer& footballer, float& value) const; 
			void getValue_BallOwnership(Footballer& footballer, float& value) const; 
			void getValue_CurrBallDist(Footballer& footballer, float& value) const; 
			void getValue_CurrBallRelHeight(Footballer& footballer, float& value) const; 
			void getValue_CurrBallRelAngle(Footballer& footballer, float& value) const; 
		};

		struct PlayerState {

			float footballerScoreInfluence;
			bool isSwitchLocked;
			bool wasSwitchLocked;

			inline bool switchLockFinished() { return wasSwitchLocked && !isSwitchLocked;  }

			PlayerState() : footballerScoreInfluence(1.0f), isSwitchLocked(false), wasSwitchLocked(false) {}
		};
	};


} }

#endif