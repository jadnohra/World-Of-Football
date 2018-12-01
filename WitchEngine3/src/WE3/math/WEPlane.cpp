#include "WEPlane.h"

#include "../WEAssert.h"

namespace WE {

#define MDefaultPlaneThickness 0.0f;
float AAPlane::kPlaneThickness = MDefaultPlaneThickness; 

AAPlane::AAPlane() {

	normalDim = -1;
}

void AAPlane::toPoints(Point points[4], float size) const {

	if (normalDim < 0) {

		for (short i = 0; i < 4; i++) {
			points[i].zero();
		}

	}

	
	points[0].el[normalDim] = position.el[normalDim];
	points[0].el[(normalDim + 1) % 3] = position.el[(normalDim + 1) % 3] - size;
	points[0].el[(normalDim + 2) % 3] = position.el[(normalDim + 2) % 3] - size;

	points[1].el[normalDim] = position.el[normalDim];
	points[1].el[(normalDim + 1) % 3] = position.el[(normalDim + 1) % 3] - size;
	points[1].el[(normalDim + 2) % 3] = position.el[(normalDim + 2) % 3] + size;

	points[2].el[normalDim] = position.el[normalDim];
	points[2].el[(normalDim + 1) % 3] = position.el[(normalDim + 1) % 3] + size;
	points[2].el[(normalDim + 2) % 3] = position.el[(normalDim + 2) % 3] + size;

	points[3].el[normalDim] = position.el[normalDim];
	points[3].el[(normalDim + 1) % 3] = position.el[(normalDim + 1) % 3] + size;
	points[3].el[(normalDim + 2) % 3] = position.el[(normalDim + 2) % 3] - size;

}

void AAPlane::init(const Point& position, const Vector& _normal, bool isNormalized) {

	Vector tempNormal;
	const Vector* pNormal;
	Vector mNormal;

	if (isNormalized) {

		pNormal = &_normal;

	} else {

		_normal.normalize(mNormal);
		pNormal = &tempNormal;
	}

	const Vector& normal = *pNormal;

	normalDim = -1;

	if (fabs(normal[0]) == 1.0f) {
		normalDim = 0;
	} else if (fabs(normal[1]) == 1.0f) {
		normalDim = 1;
	} else if (fabs(normal[2]) == 1.0f) {
		normalDim = 2;
	}

	assrt(normalDim != -1); //not axis aligned!

	negative = normal.el[normalDim] < 0.0f;

	this->position = position;
	//this->normal = normal;
}

void inline AAPlane_FloatDiff(const AAPlane& plane, const float& point, float& diff) {

	diff = plane.negative ? plane.position.el[plane.normalDim] - point : point - plane.position.el[plane.normalDim];
}

void inline AAPlane_PointDiff(const AAPlane& plane, const Point& point, float& diff) {

	diff = plane.negative ? plane.position.el[plane.normalDim] - point.el[plane.normalDim] : point.el[plane.normalDim] - plane.position.el[plane.normalDim];
}

PlanePartitionResult AAPlane_PointClassify(const AAPlane& plane, const Point& point, float& classifDiff) {

	AAPlane_PointDiff(plane, point, classifDiff);

	if (classifDiff > AAPlane::kPlaneThickness) {
		return PP_Front;
	} 

	return PP_Back;
}

PlanePartitionResult AAPlane_FloatClassify(const AAPlane& plane, float point) {

	float diff;
	AAPlane_FloatDiff(plane, point, diff);

	if (diff > AAPlane::kPlaneThickness) {
		return PP_Front;
	} 

	return PP_Back;
}

PlanePartitionResult AAPlane::classify(const Point& point) const {

	float classifDiff;

	return AAPlane_PointClassify(*this, point, classifDiff);
}

PlanePartitionResult AAPlane::classify(const Point& point, float& classifDiff) const {

	return AAPlane_PointClassify(*this, point, classifDiff);
}

PlanePartitionResult AAPlane::classify(const AAB& box) const {

	PlanePartitionResult res[2];
	float classifDiff;

	res[0] = AAPlane_PointClassify(*this, box.min, classifDiff);
	res[1] = AAPlane_PointClassify(*this, box.max, classifDiff);

	if (res[0] == res[1]) {
		return res[0];
	}

	return PP_BackAndFront;
}

PlanePartitionResult AAPlane::classify(const Triangle& tri) const {

	PlanePartitionResult res[3];
	float classifDiff;

	res[0] = AAPlane_PointClassify(*this, tri.vertices[0], classifDiff);
	res[1] = AAPlane_PointClassify(*this, tri.vertices[1], classifDiff);
	res[2] = AAPlane_PointClassify(*this, tri.vertices[2], classifDiff);

	if (res[0] == res[1] &&  res[0] == res[2]) {
		return res[0];
	}

	return PP_BackAndFront;
}


PlanePartitionResult AAPlane::classify(const OBB& box) const {

	/*
	possible Axis Aligned obb detection optimization
	use static var to determin axis aligned axis in box.coordFrame
	and check this before testing general case?

	if (box.coordFrame.row[normalDim].el[normalDim] == 1.0f) {

		PlanePartitionResult res[2];

		res[0] = AAPlane_FloatClassify(*this, box.toDim(box.extents.el[normalDim], normalDim));
		res[1] = AAPlane_FloatClassify(*this, box.toDim(-box.extents.el[normalDim], normalDim));

		if (res[0] == res[1]) {
			return res[0];
		}

		return PP_FrontAndBack;

	} */

	Point points[8];
	box.toPoints(points);

	return classify(points, 8);
}

PlanePartitionResult AAPlane::classify(const Point* points, WETL::Index::Type count) const {

	PlanePartitionResult firstPointRes;
	PlanePartitionResult pointRes;
	float classifDiff;

	
	if (count == 0) {
		assrt(false);
		return PP_BackAndFront;
	}

	firstPointRes = AAPlane_PointClassify(*this, points[0], classifDiff);

	for (WETL::Index i = 1; i < count; i++) {

		pointRes = AAPlane_PointClassify(*this, points[i], classifDiff);

		if ((pointRes != firstPointRes) || (pointRes == PP_BackAndFront)) {

			return PP_BackAndFront;
		}	
	}

	return firstPointRes;
}

PlanePartitionResult AAPlane::classify(const Sphere& sphere) const {

	PlanePartitionResult res[2];

	res[0] = AAPlane_FloatClassify(*this, sphere.center.el[normalDim] + sphere.radius);
	res[1] = AAPlane_FloatClassify(*this, sphere.center.el[normalDim] - sphere.radius);
	

	if (res[0] == res[1]) {
		return res[0];
	}

	return PP_BackAndFront;
}

Plane::Plane() {
}

void Plane::init(const Point& position, const Vector& _normal, bool isNormalized) {

	Vector tempNormal;
	const Vector* pNormal;

	if (isNormalized) {

		pNormal = &_normal;

	} else {

		_normal.normalize(normal);
		pNormal = &tempNormal;
	}

	const Vector& normal = *pNormal;

	this->position = position;
	this->normal = normal;

	Vector pt;
	pt.x = position.y;
	pt.y = position.z;
	pt.z = position.x;

	if (pt.isZero()) {

		pt.x = 1.0f;
	}

	normal.cross(pt, t[0]);

	if (t[0].isZero()) {

		pt.y += 1.0f;
	}

	normal.cross(pt, t[0]);
	normal.cross(t[0], t[1]);

	//assrt(t[0].isZero() == false);

	t[0].normalize();
	t[1].normalize();
}

void Plane::toPlaneEquation(float& a, float& b, float& c, float& d) const {

	a = normal.x;
	b = normal.y;
	c = normal.z;

	d = -normal.dot(position);
}

void Plane::toPoints(Point points[4], float size) const {

	Vector3 temp[2];
	Vector3 offset;

	t[0].mul(-size, temp[0]);
	t[1].mul(-size, temp[1]);

	temp[0].add(temp[1], offset);

	position.add(offset, points[0]);
	

	t[1].mul(size, temp[1]);
	temp[0].add(temp[1], offset);

	position.add(offset, points[1]);

	t[0].mul(size, temp[0]);
	temp[0].add(temp[1], offset);

	position.add(offset, points[2]);

	t[1].mul(-size, temp[1]);
	temp[0].add(temp[1], offset);

	position.add(offset, points[3]);
}

PlanePartitionResult Plane::classify(const Point& point, float& classifDiff) const {

	Vector3 dir;

	point.subtract(position, dir);

	return (classifDiff = normal.dot(dir)) >= 0.0f ? PP_Front : PP_Back;
}


}