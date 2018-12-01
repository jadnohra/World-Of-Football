#ifndef h_WOF_match_SceneCamera_ControllerBufferZone_h
#define h_WOF_match_SceneCamera_ControllerBufferZone_h

#include "SceneCamera_Controller.h"
#include "../../SpeedProfile.h"


namespace WOF { namespace match { namespace sceneCamera {


	class Controller_BufferZone : public ControllerBase {
	public:

		virtual void start(CamState& state);
		virtual void stop(CamState& state);

		virtual void forceStarted(CamState& state);

		virtual void update(CamState& state);
		virtual void getModifs(Vector3& eyePosModif, Vector3& targetPosModif, Vector3& targetOffsetModif);

	public:

		Controller_BufferZone();

		void init(BufferString& tempStr, DataChunk& chunk, CoordSysConv* pConv);
	
		float getRadius(); //already converted

	protected:

		enum State {

			S_None = 0, S_Stopped, S_Started
		};

	protected:

		void stateSwitch(const Time& t, const State& newState, bool* pRecurse);
		bool statePrepare();

		void stateHelperMoveTo(CamState& state, const Time& t, const State& nextState, 
								const bool& isNewState, const SpeedProfile::Setup& speedProfileSetup, 
								const Vector3& wantedDistance, const bool& wantedDistanceChanged, 
								bool& recurse, CoordSysConv* pConv);


	protected:

		bool mEnable;

		SpeedProfile::Setup mSpeedSetup;
		
		float mRefRadius;
		float mRefScreenWidth;
		
		bool mCenterEnable;
		
		State mPreparingState;
		State mState;
		Time mStateStartTime;

		Distance mOffset;
		SpeedProfile mSpeedProfile;
		float mSpeed;

		bool mRadiusIsDirty;
		float mRadius;
		bool mCenterIsActive;
	};


} } }

#endif