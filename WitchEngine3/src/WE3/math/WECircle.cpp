#include "WECircle.h"
#include "float.h"
#include "WEIntersect.h"
#include "WEDistance.h"

namespace WE {

void Circle::empty() {
	radius = 0.0f;
}

bool Circle::contains(const CtVector2& point) {

	return distanceSq(center, point) <= radius * radius;
}

void Circle::clip(const CtVector2& point, Vector2& result) {

	Vector2 dist;
	point.subtract(center, dist);

	float scale = radius / dist.mag();

	if (scale < 1.0f) {

		dist.mul(scale);
		center.add(dist, result);

	} else {

		result = point;
	}
}

bool Circle::rayIntersect(const CtVector2& point, const CtVector2& point2, float u[2], int& resultCount) {

	resultCount = 0;

	float b = 2.0f * ( (point2.x - point.x) * (point.x - center.x) + (point2.y - point.y) * (point.y - center.y) );
	float a = 1.0f;
	float c = center.x*center.x + center.y*center.y + point.x*point.x + point.y*point.y - (2.0f * (center.x*point.x + center.y*point.y)) - radius*radius;
	float b2min4ac = b*b - 4.0f * a * c;

	if (b2min4ac < 0.0f)
		return false;

	if (b2min4ac == 0.0f) {

		u[0] = -b / (2.0f * a);
		++resultCount;
		return true;
	}

	float sqrtb2min4ac = sqrtf(b2min4ac);

	u[0] = (-b - sqrtb2min4ac) / (2.0f * a);
	++resultCount;

	u[1] = (-b + sqrtb2min4ac) / (2.0f * a);
	++resultCount;

	return true;
}


int Circle::rayIntersect(const CtVector2& point, const CtVector2& dir, float result[2]) {

	int rayResultCount;
	Vector2 point2;

	int resultCount = 0;

	point.addMultiplication(dir, 1.0f, point2);

	float u[2];

	if (rayIntersect(point, point2, u, rayResultCount)) {

		for (int i = 0; i < rayResultCount; ++i) {

			if (u[i] >= 0.0f) {

				result[resultCount] = u[i];
				++resultCount;
			}
		}
	}

	return resultCount;
}

int Circle::segIntersect(const CtVector2& point, const CtVector2& point2, float result[2]) {

	int rayResultCount;
	float u[2];

	int resultCount = 0;

	if (rayIntersect(point, point2, u, rayResultCount)) {

		for (int i = 0; i < rayResultCount; ++i) {

			if (isBetweenSegPoints(u[i])) {

				result[resultCount] = u[i];
				++resultCount;
			}
		}
	}

	return resultCount;
}

}