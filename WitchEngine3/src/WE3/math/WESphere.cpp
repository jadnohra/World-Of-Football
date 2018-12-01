#include "WESphere.h"

#include "WEDistance.h"

namespace WE {

Sphere::Sphere() {
	radius = 0.0f;
}

void Sphere::reset() {
	center = 0.0f;
	radius = 0.0f;
}

void Sphere::add(const Sphere& merged) {

	radius = tmax(radius, merged.radius + distance(center, merged.center));
}

void Sphere::init(AAB& refAABox, bool sphereEnglobingBox) {
	
	refAABox.center(center);

	if (sphereEnglobingBox) {

		WE::Vector3 corners[8];
		refAABox.getCorners(corners);

		float dist;

		radius = distance(center, corners[0]);
		for(short i = 1; i < 8; i++) {
			dist = distance(center, corners[i]);

			if (dist > radius) {
				radius = dist;
			}
		}

	} else {

		float diam = refAABox.xSize();
		if (refAABox.ySize() > diam) {
			diam = refAABox.ySize();
		}
		if (refAABox.zSize() > diam) {
			diam = refAABox.zSize();
		}

		radius = diam * 0.5f;
	}
}

bool sphereContains(const Point& center, float radius, const Point& pt) {

	Vector3 dist;
	pt.subtract(center, dist);

	return (radius * radius >= dist.magSquared());
}

bool sphereSquaredContains(const Point& center, float radiusSquared, const Point& pt) {

	Vector3 dist;
	pt.subtract(center, dist);

	return (radiusSquared >= dist.magSquared());
}

void sphereClip(const Point& center, float radius, Vector3& pt) {

	Vector3 dist;
	pt.subtract(center, dist);

	float scale = radius / dist.mag();

	if (scale < 1.0f) {

		dist.mul(scale);
		center.add(dist, pt);
	}
}

}