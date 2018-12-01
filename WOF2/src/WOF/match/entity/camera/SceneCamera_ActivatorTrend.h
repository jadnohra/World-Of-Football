#ifndef h_WOF_match_SceneCamera_ActivatorTrend_h
#define h_WOF_match_SceneCamera_ActivatorTrend_h

#include "WE3/data/WEDataSource.h"
#include "WE3/WETL/WETLArray.h"
using namespace WE;

#include "SceneCamera_Controller.h"
#include "SceneCamera_ControllerTable.h"
#include "SceneCamera_Providers.h"

namespace WOF { namespace match { 

	class Match;

} }

namespace WOF { namespace match { namespace sceneCamera {

	class Controller_ActivatorTrend : public ControllerBase {
	public:

		virtual ~Controller_ActivatorTrend();


		virtual void start(CamState& state);
		virtual void stop(CamState& state);

		virtual void forceStarted(CamState& state);

		virtual void update(CamState& state);
		virtual void getModifs(Vector3& eyePosModif, Vector3& targetPosModif, Vector3& targetOffsetModif);
		virtual void getPostModifs(Vector3& postModif);

	public:

		Controller_ActivatorTrend();
			
		void init(Match& match, BufferString& tempStr, DataChunk& chunk, CoordSysConv* pConv, ControllerTable* pTable);
		void setActivationTarget(ControllerBase* pController);
		
	protected:

		void onTrendChanged(CamState& state);
		void updateFollowerProviders(CamState& state);

	protected:

		enum  TrendType {

			TT_Interval = 0, TT_Distance
		};

		struct Follower {

			HardRef<ControllerBase> target;
			
			float distance;
			float invDistance;

			int directionCount;
			Vector3 refDir;
			Vector3 normalDir;

			SoftRef<ControllerProvider_Distance_Impl> provider;
		};

		typedef WETL::CountedPtrArray<Follower*, true, unsigned int> Followers;

	protected:

		HardRef<CoordSysConv> mConv;

		HardRef<ControllerBase> mActivationTarget;
		Followers mFollowers;
		
		bool mIsStarted;
		
		Vector3 mTrend;

		TrendType mTrendType;
		Time mTrendInterval;
		float mTrendDistance;
		Time mIdleInterval;
		float mSensitivity;

		bool mSamplingStarted;
		Time mSamplingStartTime;
		
		bool mIdleDetectStarted;
		Time mIdleDetectStartTime;

		Vector3 mTrendDir;
		float mCumulDist;
	};

} } }

#endif