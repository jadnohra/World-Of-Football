#include "SceneCamera_State.h"

#include "SceneCamera.h"
#include "../../Match.h"

namespace WOF { namespace match { namespace sceneCamera {


void CamState::setResetFlag(bool val) {

	resetFlag = val;
}

void CamState::CameraPoint::setPos(const Point& newPos, const Time& dt, bool reset) {

	if (reset) {

		lastPos = newPos;

	} else {

		lastPos = currPos;
	}

	currPos = newPos;

	currPos.subtract(lastPos, dist);
	dist.div(dt, speed);

	speed.normalize(speedDir);
}

void CamState::TrackState::constrainPos(const Point& pos, Point& constrainedPos) {

	constrainedPos = pos;

	if (settings.constrainUp) {

		constrainedPos.el[Scene_Up] = settings.constrainUpValue;
	} 
}

} } }

