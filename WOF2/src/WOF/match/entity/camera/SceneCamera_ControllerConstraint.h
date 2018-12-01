#ifndef h_WOF_match_SceneCamera_ControllerConstraint_h
#define h_WOF_match_SceneCamera_ControllerConstraint_h

#include "SceneCamera_Controller.h"
#include "../../SpeedProfile.h"
#include "../../inc/Scene.h"

#include "WE3/data/WEDataSource.h"
#include "WE3/WETL/WETLArray.h"
using namespace WE;

namespace WOF { namespace match { namespace sceneCamera {

	class Controller_Constraint : public ControllerBase {
	public:

		virtual void start(CamState& state);
		virtual void stop(CamState& state);

		virtual void forceStarted(CamState& state);

		virtual void update(CamState& state);
		virtual void getModifs(Vector3& eyePosModif, Vector3& targetPosModif, Vector3& targetOffsetModif);
		virtual void getPostModifs(Vector3& postModif);

	public:

		Controller_Constraint();

		void init(BufferString& tempStr, DataChunk& chunk, CoordSysConv* pConv);
	
	protected:

		enum State {

			S_None = 0, S_Stopped, S_Started
		};

		struct Constraint_Axis {

			struct Limit {

				bool enable;
				float val;
			};

			SceneDirectionEnum axis;

			Limit min;
			Limit max;

			void addConstraintOffset(const Vector3& ref, Vector3& offset);
		};

		typedef WETL::CountedPtrArray<Constraint_Axis*, true, unsigned int, WETL::ResizeExact> Constraints;

	protected:

		void stateSwitch(const Time& t, const State& newState, bool* pRecurse);
		bool statePrepare();


	protected:

		bool mEnable;

		State mPreparingState;
		State mState;
		Time mStateStartTime;

		Constraints mConstraints;

		Vector3 mOffset;
	};

} } }

#endif