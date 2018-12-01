#ifndef h_WOF_match_SceneCamera_ControllerSmoother_h
#define h_WOF_match_SceneCamera_ControllerSmoother_h

#include "SceneCamera_Controller.h"
#include "../../SpeedProfile.h"
#include "../../inc/Scene.h"

#include "WE3/data/WEDataSource.h"
#include "WE3/WETL/WETLArray.h"
using namespace WE;

namespace WOF { namespace match { namespace sceneCamera {

	class Controller_Smoother : public ControllerBase {
	public:

		virtual void start(CamState& state);
		virtual void stop(CamState& state);

		virtual void forceStarted(CamState& state);

		virtual void update(CamState& state);
		virtual void getModifs(Vector3& eyePosModif, Vector3& targetPosModif, Vector3& targetOffsetModif);
		virtual void getPostModifs(Vector3& postModif);

	public:

		Controller_Smoother();

		void init(BufferString& tempStr, DataChunk& chunk, CoordSysConv* pConv);
	
	protected:

		enum State {

			S_None = 0, S_Stopped, S_Started
		};
	
	protected:

		void stateSwitch(const Time& t, const State& newState, bool* pRecurse);
		bool statePrepare();


	protected:

		bool mEnable;

		State mPreparingState;
		State mState;
		Time mStateStartTime;

		Vector3 mOffset;
		Vector3 mLastSpeedDir;

		bool mHasFixedPoint;
		float mFixedPointTime;
		Vector3 mFixedPoint;

		bool mDynamicFixPoint;
		float mTimeTolerance;
		float mOffsetCancelSpeed;
		float mDistTolerance;
	};

} } }

#endif