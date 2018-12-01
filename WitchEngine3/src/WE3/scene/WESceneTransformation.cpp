#include "WESceneTransformation.h"

namespace WE {

bool SceneTransformation::validate() {
	
	if (rotaxis.isZero()) {

		rot = 0.0f;
	} else {

		rotaxis.normalize();
	}

	return true;
}

void SceneTransformation::applyConverter(CoordSysConv& conv) {

	conv.toTargetPoint(trans.el);
	conv.toTargetRotation(rotaxis.el, rot);
}

void SceneTransformation::transform(const Matrix4x3Base& transf) {

	transf.transformPoint(trans);
	//transf.transformVector(rotaxis);
}

}