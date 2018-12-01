#ifndef h_WOF_match_SceneCamera_ControllerPointTrajectory_h
#define h_WOF_match_SceneCamera_ControllerPointTrajectory_h

#include "SceneCamera_Controller.h"
#include "SceneCamera_Providers.h"
#include "../../SpeedProfile.h"


namespace WOF { namespace match { namespace sceneCamera {


	class Controller_PointTrajectory : public ControllerBase {
	public:

		virtual void start(CamState& state);
		virtual void stop(CamState& state);

		virtual void forceStarted(CamState& state);

		virtual void update(CamState& state);
		virtual void getModifs(Vector3& eyePosModif, Vector3& targetPosModif, Vector3& targetOffsetModif);

		virtual bool setProvider(ControllerProvider_Distance* pProvider);

	public:

		Controller_PointTrajectory();

		void init(BufferString& tempStr, DataChunk& chunk, CoordSysConv* pConv);
		void setControlTarget(const ControlTarget& target);
	
	protected:

		enum State {

			S_None = 0, S_Stopped, S_DelayingRestart, S_DelayingStart, S_Starting, S_Started, S_DelayingChange, S_Changing, S_DelayingStop, S_Stopping
		};


	protected:

		void stateSwitch(const Time& t, const State& newState, bool* pRecurse);
		bool statePrepare();

		void stateHelperDelay(const Time& t, const Time& delay, const State& nextState, bool& recurse);
		void stateHelperMoveTo(CamState& state, const Time& t, const State& nextState, 
								const bool& isNewState, const SpeedProfile::Setup& speedProfileSetup, 
								const Vector3& wantedDistance, const bool& wantedDistanceChanged, 
								bool& recurse, CoordSysConv* pConv);


	protected:

		bool mEnable;

		HardRef<ControllerProvider_Distance> mProvider;
		ControlTarget mControlTarget;

		SpeedProfile::Setup mStartSpeedSetup;
		SpeedProfile::Setup mChangeSpeedSetup;
		SpeedProfile::Setup mStopSpeedSetup;
		
		Time mStartDelay;
		Time mStopDelay;
		Time mChangeDelay;
		Time mRestartDelay;

		State mPreparingState;
		State mState;
		Time mStateStartTime;

		Distance mDistance;
		SpeedProfile mSpeedProfile;
		float mSpeed;
	};


} } }

#endif