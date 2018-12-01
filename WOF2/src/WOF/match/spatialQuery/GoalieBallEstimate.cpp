/*
#include "../Match.h"

#include "GoalieBallEstimate.h"

namespace WOF { namespace match {

GoalieBallEstimate::GoalieBallEstimate() : mIsSimulEstimate(false), mSimulID(-1) {
}

GoalieBallEstimate::UpdateResult GoalieBallEstimate::update(SpatialPitchModel& pitchModel, Ball& ball, Footballer& footballer, const Time& currTime, BallSimulation* pSimul) {

	if (mIsSimulEstimate) {

		if (pSimul) {

			if (pSimul->getID() == mSimulID) {

				return Update_NoChange;
			}

		} 
	}

	Match& match = footballer.mNodeMatch.dref();
	Team& team = match.getTeam(footballer.getTeamEnum());


	if (pSimul) {

		mPlaneIntersections.count = 0;
		mSortedPlaneIntersections.count = 0;

		bool isLastSamplFallback;

		//pretty expensive ... might wanna split into multiple frames

		{
			const Plane& plane = pitchMode.getPlane_Back(team);

			PlaneIntersection& inter = mPlaneIntersections.addOne();
			inter.planeType = Plane_Out;
			
			if (!estimateStateAtPlane(plane, true, inter.sample, false, NULL, false)) {

				--mPlaneIntersections.count;
			}
		}

		{
			const Plane& plane = pitchMode.getPlane_Left(team);

			PlaneIntersection& inter = mPlaneIntersections.addOne();
			inter.planeType = Plane_Left;
			
			if (!estimateStateAtPlane(plane, true, inter.sample, false, NULL, false)) {

				--mPlaneIntersections.count;
			}
		}

		{
			const Plane& plane = pitchMode.getPlane_Right(team);

			PlaneIntersection& inter = mPlaneIntersections.addOne();
			inter.planeType = Plane_Right;
			
			if (!estimateStateAtPlane(plane, true, inter.sample, false, NULL, false)) {

				--mPlaneIntersections.count;
			}
		}

	} else {

	}

}

} }
*/