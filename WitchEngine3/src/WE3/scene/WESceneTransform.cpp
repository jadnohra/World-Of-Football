#include "WESceneTransform.h"

#include "math.h"

namespace WE {

SceneTransform SceneTransform::kIdentity(true);

SceneTransform::SceneTransform(bool setIdentity) : RigidMatrix4x3() {

	if (setIdentity) {

		identity();
	}
}

void SceneTransform::getPosition(Vector3& pos) const {

	pos.x = tx; pos.y = ty; pos.z = tz;
}


void SceneTransform::setPosition(const Vector3 &pos) {

	tx = pos.x; ty = pos.y; tz = pos.z;
}

void SceneTransform::movePosition(const Vector3 &pos) {

	tx += pos.x; ty += pos.y; tz += pos.z;
}

void SceneTransform::movePositionByVel(const CtVector3& vel, const float& dt) {

	tx += vel.x * dt; ty += vel.y * dt; tz += vel.z * dt;
}

void SceneTransform::setOrientation(CardinalAxis axis, float theta, bool leftHanded) {

	setRotate(axis, theta, leftHanded);
}

void SceneTransform::setOrientation(const Vector3 &axis, float theta, bool isNormalized, bool leftHanded) {

	setRotate(axis, theta, isNormalized, leftHanded);
}

void SceneTransform::setOrientation2D(const float& dirX, const float& dirY, bool leftHanded) {

	/*
	row[0].set(dirX, dirY, 0.0f);
	row[2] = Vector3::kZ;
	row[2].cross(row[0], row[1]);
	*/

	row[Scene2D_Forward].set(dirX, dirY, 0.0f);
	row[Scene2D_Forward].normalize();
	row[Scene2D_None].zero();
	row[Scene2D_None].el[Scene2D_None] = 1.0f;

	if (leftHanded)
		row[Scene2D_Forward].cross(row[Scene2D_None], row[Scene2D_Right]);
	else
		row[Scene2D_None].cross(row[Scene2D_Forward], row[Scene2D_Right]);

}


void SceneTransform::setOrientation(const Vector3 &eye, const Vector3 &lookAt, const Vector3 &unitUp) {

	Vector3 dir;

	lookAt.subtract(eye, dir);

	row[Scene_Up] = unitUp;
	row[Scene_Up].cross(dir, row[Scene_Right]);
	row[Scene_Right].normalize();
	row[Scene_Right].cross(row[Scene_Up], row[Scene_Forward]);
}

void SceneTransform::setOrientation(const Vector3 &dir, const Vector3 &unitUp) {

	row[Scene_Up] = unitUp;

	if (unitUp.dot(dir) == 0.0f) {

		row[Scene_Forward] = dir;
		row[Scene_Up].cross(dir, row[Scene_Right]);
		row[Scene_Right].normalize();

	} else {
		
		row[Scene_Up].cross(dir, row[Scene_Right]);
		row[Scene_Right].normalize();
		row[Scene_Right].cross(row[Scene_Up], row[Scene_Forward]);
	}
}

bool SceneTransform::isOrientation(CtVector3 &dir, const Vector3 &unitUp) {

	//needed because of cross product drifts

	if (dir.equals(row[2]))
		return true;

	Vector3 row0;
	Vector3 row2;

	unitUp.cross(dir, row0);
	row0.normalize();
	row0.cross(unitUp, row2);

	return dir.equals(row2);
}

void SceneTransform::setOrientation(const Vector3 &eulerAngles, bool leftHanded) {

	setRotate(eulerAngles, leftHanded);
}

void SceneTransform::setOrientation(const Quaternion& orient) {

	orient.toMatrix(castToMatrix3x3Base());
}

void SceneTransform::getOrientation(Quaternion& orient) {

	orient.fromMatrix(castToMatrix3x3Base());
}

void SceneTransform::setTransformation(const SceneTransformation& trans, bool isNormalized, bool leftHanded) {

	setPosition(trans.trans);
	setOrientation(trans.rotaxis, trans.rot, isNormalized, leftHanded);
}

void SceneTransform::moveOrientation(const Vector3 &axis, float theta, bool isNormalized, bool leftHanded) {

	Matrix3x3Base rotDiff;
	rotDiff.setupRotation(axis, theta, isNormalized, leftHanded);

	castToMatrix3x3Base().moveOrientation(rotDiff);
}

void SceneTransform::moveOrientation(Quaternion &quat, bool isNormalized) {

	Matrix3x3Base rotDiff;
	quat.toMatrix(rotDiff, isNormalized);

	castToMatrix3x3Base().moveOrientation(rotDiff);
}

void SceneTransform::syncBox(Box& box, bool syncRotation, bool syncTranslation) const {


	if (syncRotation) {
		copy3x3BaseTo(box.basis());
	}

	if (syncTranslation) {
		box.position() = row[3];
	}
}


}