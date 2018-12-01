/*

#ifndef _BallGoalieEstimate_h_WOF_match
#define _BallGoalieEstimate_h_WOF_match

#include "WETL/WETLArray.h"
using namespace WE;

#include "BallSimulation.h"
#include "SpatialPitchModel.h"

namespace WOF { namespace match {

	class Match;

	class GoalieBallEstimate {
	public:

		enum UpdateResult {

			Update_None = -1,
			Update_NoChange,
			Update_NewEstimate,
			Update_Invalidated,
		};

	public:

		UpdateResult update(SpatialPitchModel& pitchModel, Ball& ball, Footballer& footballer, const Time& currTime, BallSimulation* pSimul);

	protected:

		enum PlaneType {

			Plane_None = -1, Plane_Out, Plane_Left, Plane_Right, Plane_Goalie
		};

		struct PlaneIntersection {

			PlaneType planeType;
			BallSimulation::TrajSample sample;

			PlaneIntersection();
		};

		typedef WETL::CountedArray<PlaneIntersection, false, unsigned int> PlaneIntersections;
		typedef WETL::CountedPtrArrayEx<PlaneIntersection*, unsigned int> PlaneIntersectionPtrs;

		bool mIsSimulEstimate;
		SimulationID mSimulID;

		PlaneIntersections mPlaneIntersections;
		PlaneIntersectionPtrs mSortedPlaneIntersections;
	};

} }
*/