#ifndef SceneCamera_State_h
#define SceneCamera_State_h

#include "WE3/WETime.h"
#include "WE3/math/WEVector.h"
#include "WE3/coordSys/WECoordSysConv.h"
#include "WE3/WERef.h"
#include "WE3/WEPtr.h"
using namespace WE;

namespace WOF { namespace match { 

	class SceneCamera;
} }

namespace WOF { namespace match { namespace sceneCamera {

	struct CamState {

		struct TrackSettings {

			bool constrainUp;
			float constrainUpValue;
		};

		struct CameraPoint {

			Point lastPos;
			Point currPos;

			Vector3 dist;
			Vector3 speed;
			Vector3 speedDir;

			void setPos(const Point& newPos, const Time& dt, bool reset = false);
		};

		struct CameraState {

			CameraPoint eye;
			CameraPoint target;
		};

		struct TrackState {

			TrackSettings settings;

			CameraPoint pos;
			CameraPoint lookAtBasePos; 

			void constrainPos(const Point& pos, Point& constrainedPos);
		};

		HardRef<CoordSysConv> coordSysConv;
		SoftPtr<SceneCamera> parent;

		bool resolutionChangedFlag;

		int resWidth;
		int resHeight;

		Time t;
		Time updateDt;
		
		bool resetFlag;

		TrackState track;
		CameraState camera;
		CameraState cameraPost; //only if post controllers are used

		void setResetFlag(bool val);
	};

} } }

#endif