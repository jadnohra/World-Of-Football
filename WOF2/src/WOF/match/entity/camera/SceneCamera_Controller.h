#ifndef h_WOF_match_SceneCamera_Controller_h
#define h_WOF_match_SceneCamera_Controller_h

#include "WE3/WERef.h"
using namespace WE;

#include "SceneCamera_State.h"
#include "SceneCamera_Providers.h"


namespace WOF { namespace match { namespace sceneCamera {

	class ControllerBase : public Ref {
	public:

		enum ControlTarget {

			CT_None = 0, CT_Eye, CT_Target, CT_TargetOffset, CT_Offset
		};

	public:

		virtual ~ControllerBase() {}

		virtual void start(CamState& state) {}
		virtual void stop(CamState& state) {}

		virtual void forceStarted(CamState& state) {}

		virtual void update(CamState& state) {}
		virtual void getModifs(Vector3& eyePosModif, Vector3& targetPosModif, Vector3& targetOffsetModif) {}
		virtual void getPostModifs(Vector3& postModif) {}

		virtual bool setProvider(ControllerProvider_Distance* pProvider) { return false; }
	};

} } }

#endif