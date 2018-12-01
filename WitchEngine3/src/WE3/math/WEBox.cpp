#include "WEBox.h"
#include "WESpan.h"

namespace WE {

	

Box::Box() {
}

Box::Box(const Box& ref) {

	*this = ref;
}

void Box::empty() {
	obb.extents.zero();
}

bool Box::isEmpty() {
	return  (0.0f == obb.extents.x) && (0.0f == obb.extents.y) && (0.0f == obb.extents.z);
}


void Box::transform(const AAB3& aab, const Matrix4x3Base& matrix, OBB& result) {

	Matrix4x3Base temp(true);

	aab.centeredRadius(temp.row[3], result.extents);
	mul(temp, matrix, result.coordFrame);
}

void Box::init(WE::AAB3& ref, bool identityBasis) {

	isDirty = true;

	if (ref.isEmpty()) {

		empty();

	} else {

		ref.centeredRadius(centerOffset, obb.extents);

		/*
		obb.extents.x = ref.xSize() * 0.5f;
		obb.extents.y = ref.ySize() * 0.5f;
		obb.extents.z = ref.zSize() * 0.5f;

		ref.center(centerOffset);
		*/
	}

	if (identityBasis) {
		basis().identity();
	}
}

void Box::init(WE::AAB3& ref, const float& scale, bool identityBasis) {

	init(ref, identityBasis);

	obb.extents.mul(scale);
}

void Box::init(WE::Vector& extents, bool identityBasis) {

	isDirty = true;

	obb.extents = extents;
	centerOffset.zero();

	if (identityBasis) {
		basis().identity();
	}
}


void Box::setCoordFrame(const Matrix4x3Base& coordFrame) {

	isDirty = true;
	obb.coordFrame = coordFrame;
}

void Box::mergeTo(WE::AAB3& aab) {

	short pointCount = 8;
	Point points[8];
	
	toPoints(points/*, pointCount*/);

	for (short i = 0; i < pointCount; i++) {
		aab.add(points[i]);
	}

}

Vector& Box::position(bool readOnly) {

	if (readOnly == false) {
		isDirty = true;
	}

	return boxPosition;
}


Matrix3x3Base& Box::basis(bool readOnly) {

	if (readOnly == false) {
		isDirty = true;
	}

	return obb.coordFrame.castToMatrix3x3Base();
}

void Box::_update() {

	Vector rotatedCenterOffset;

	obb.coordFrame.transformVector(centerOffset, rotatedCenterOffset);
	boxPosition.add(rotatedCenterOffset, obb.coordFrame.row[3]);

}

void Box::update(bool force) {

	if (isDirty || force) {
		_update();
	}
}

const OBB& Box::toOBB() {

	if (isDirty) {
		_update();
	}

	return obb;
}


void Box::transform(const Matrix4x3Base& matrix, OBB& result) const {

	mul(obb.coordFrame, matrix, result.coordFrame);
	result.extents = obb.extents;
}


void Box::toPoints(Vector points[8]) {

	if (isDirty) {
		_update();
	}

	obb.toPoints(points/*, pointCount, attempt2DTest, attempt2DTestDim, pAttempt2DTestNormal*/);
}




}