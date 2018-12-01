#ifndef h_WOF_match_SceneCamera_Providers_h
#define h_WOF_match_SceneCamera_Providers_h

#include "WE3/WERef.h"
using namespace WE;

#include "SceneCamera_State.h"

namespace WOF { namespace match { namespace sceneCamera {

	class ControllerProvider_Distance : public Ref {
	public:

		virtual bool provideDistance(CamState& state, Vector3& distance) = 0; //returns true if distance changed form last time
	};

	class ControllerProvider_Distance_Impl : public ControllerProvider_Distance {
	public:

		virtual bool provideDistance(CamState& state, Vector3& distance); //returns true if distance changed form last time

	public:

		ControllerProvider_Distance_Impl();

		void setDistance(const Vector3& distance);

	protected:

		bool mChanged;
		Vector3 mDistance;
	};

} } }

#endif
