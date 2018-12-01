#ifndef h_WOF_match_Spatial_ScriptClasses
#define h_WOF_match_Spatial_ScriptClasses

#include "WE3/script/WEScriptEngine.h"
#include "../CoverEstimate.h"
#include "../PitchRegionShape.h"
#include "../BallInterceptEstimate.h"
#include "../BallShotEstimator.h"
#include "../SpatialTeamState.h"
#include "../SpatialPitchModel.h"


namespace WOF { namespace match {

	class SpatialDeclareClasses {
	public:

		static void declareInVM(SquirrelVM& target) {

			BallSimulation::TrajSample::declareInVM(target);
			BallInterceptEstimate::declareInVM(target);
			BallShotEstimator::declareInVM(target);
			
			//SpatialBlockEstimate::declareInVM(target);
			CoverEstimate2D::declareInVM(target);
			PitchRegionShape::declareInVM(target);

			SpatialMatchState::BallState::declareInVM(target);

			SpatialPitchModel::declareInVM(target);
		}
	};

} }

#endif