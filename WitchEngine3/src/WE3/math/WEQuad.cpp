#include "WEQuad.h"
#include "float.h"
#include "WEIntersect.h"
#include "WEDistance.h"

namespace WE {

const Vector2 AAQuad::axis[2] = {Vector2(1.0f, 0.0f), Vector2(0.0f, 1.0f) };

void AAQuad::empty() {
	min.x = min.y = FLT_MAX;
	max.x = max.y = -FLT_MAX;
}

void AAQuad::centeredRadius(Vector2& center, Vector2& radius) const { 

	min.add(max, center); 
	center.mul(0.5f);

	max.subtract(min, radius);
	radius.mul(0.5f);
}

int AAQuad::getMinSizeIndex(float& minSize) const {

	int minIndex = 0;
	
	float dx = max.x - min.x;
	float dy = max.y - min.y;

	minSize = dx;

	if (dy < minSize) {

		minIndex = 1;
		minSize = dy;
	}

	return minIndex;
}

void AAQuad::getPoint(const int& index, Vector2& point) {

	switch (index) {

		case 1: {

			point.x = max.x;
			point.y = min.y;

		} break;

		case 2: {

			point.x = max.x;
			point.y = max.y;

		} break;

		case 3: {

			point.x = min.x;
			point.y = max.y;

		} break;

		default: {

			point.x = min.x;
			point.y = min.y;

		} break;
	}
}

bool AAQuad::contains(const CtVector2& point) {

	return spanOverlap(min.x, max.x, point.x)
			&& spanOverlap(min.y, max.y, point.y);
}

void AAQuad::clip(const CtVector2& point, Vector2& result) {

	result.x = tcap(point.x, min.x, max.x);
	result.y = tcap(point.y, min.y, max.y);
}


bool AAQuad::rayIntersect(const CtVector2& point, const CtVector2& dir, Span& result) {

	Span span;

	if (!intersectAASlab(0, min.el[0], max.el[0], point, dir, span)) 
		return false;

	if (!intersectAASlab(1, min.el[1], max.el[1], point, dir, result)) 
		return false;

	if (!spanCheckGetOverlap(span, result, result)) {

		return false;
	}

	return true;
}

bool AAQuad::segIntersect(const CtVector2& point, const CtVector2& point2, Span& result, Vector2& lineDir) {

	Span span;

	point2.subtract(point, lineDir);

	if (!intersectAASlab(0, min.el[0], max.el[0], point, lineDir, span)) 
		return false;

	if (!intersectAASlab(1, min.el[1], max.el[1], point, lineDir, result)) 
		return false;

	if (!spanCheckGetOverlap(span, result, result)) {

		return false;
	}

	float lineDirMag = lineDir.mag();
	Span segmentSpan(0.0f, lineDirMag);

	if (!spanCheckGetOverlap(result, segmentSpan, result)) {

		return false;
	}

	span.min /= lineDirMag;
	span.max /= lineDirMag;

	return true;
}

int AAQuad::rayIntersect(const CtVector2& point, const CtVector2& dir, float result[2]) {

	Span span;
	int resultCount = 0;

	if (rayIntersect(point, dir, span)) {

		if (span.min >= 0.0f) {

			result[resultCount] = span.min;
			++resultCount;
		}

		if (span.max >= 0.0f) {

			result[resultCount] = span.max;
			++resultCount;
		}
	}

	return resultCount;
}

int AAQuad::segIntersect(const CtVector2& point, const CtVector2& point2, float result[2]) {

	Span span;
	Vector2 lineDir;
	int resultCount = 0;

	if (segIntersect(point, point2, span, lineDir)) {

		if (span.min > 0.0f) {

			result[resultCount] = span.min;
			++resultCount;
		}

		if (span.max <= 1.0f) {

			result[resultCount] = span.max;
			++resultCount;
		}
	}

	return resultCount;
}

/*
void AAQuad::getClosestPoint(const CtVector2& point, Vector2& result) {

	if (contains(point)) {

		result = point;
		return true;
	}

	float minDistSq = FLT_MAX;
	int minDistIndex = 0;
	float distSq;
	int distIndex = 0;

	Vector2 tempPoint[2];
	bool isBetweenPoints;

	{
		tempPoint[0].x = min.x;
		tempPoint[0].y = min.y;
		distSq = distanceSq(point, tempPoint[0]);

		if (distSq < minDistSq) {

			minDistSq = distSq;
			minDistIndex = distIndex;
		}

		++distIndex;
	}

	{
		tempPoint[1].x = max.x;
		tempPoint[1].y = min.y;
		distSq = distanceSq(point, tempPoint[1]);

		if (distSq < minDistSq) {

			minDistSq = distSq;
			minDistIndex = distIndex;
		}

		++distIndex;
	}

	{
		tempPoint[0].x = max.x;
		tempPoint[0].y = max.y;
		distSq = distanceSq(point, tempPoint[0]);

		if (distSq < minDistSq) {

			minDistSq = distSq;
			minDistIndex = distIndex;
		}

		++distIndex;
	}

	{
		tempPoint[0].x = min.x;
		tempPoint[0].y = max.y;
		distSq = distanceSq(point, tempPoint[0]);

		if (distSq < minDistSq) {

			minDistSq = distSq;
			minDistIndex = distIndex;
		}

		++distIndex;
	}

	int minPointDistIndex = minDistIndex;
	int segtPtIndex[2];
	segtPtIndex[0] = (minPointDistIndex + 1) % 4;
	segtPtIndex[1] = (minPointDistIndex - 1) % 4;

	getPoint(minPointDistIndex, tempPoint[0]);

	{
		getPoint(segtPtIndex[0], tempPoint[1]);

		distSq = distanceSq(tempPoint[0], tempPoint[1], point, isBetweenPoints);
		
		if (isBetweenPoints && distSq < minDistSq) {

			minDistSq = distSq;
			minDistIndex = distIndex;
		}

		++distIndex;
	}

	{
		getPoint(segtPtIndex[1], tempPoint[1]);

		distSq = distanceSq(tempPoint[0], tempPoint[1], point, isBetweenPoints);
		
		if (isBetweenPoints && distSq < minDistSq) {

			minDistSq = distSq;
			minDistIndex = distIndex;
		}

		++distIndex;
	}

	if (distIndex == minPointDistIndex) {

		result = tempPoint[0];

	} else {

		getPoint(segtPtIndex[minDistIndex - 4], tempPoint[1]);

		float u;

		distanceSq(tempPoint[0], tempPoint[1], point, result, u);
	}

	return true;
}
*/


void Quad::empty() {

	coordFrame.identity();
	extents.zero();
}

void Quad::getPoint(const int& index, Vector2& point) {

	switch (index) {

		case 1: {

			point.x = extents.x;
			point.y = -extents.y;

		} break;

		case 2: {

			point.x = extents.x;
			point.y = extents.y;

		} break;

		case 3: {

			point.x = -extents.x;
			point.y = extents.y;

		} break;

		default: {

			point.x = -extents.x;
			point.y = -extents.y;

		} break;
	}

	coordFrame.transformPoint(point);
}

bool Quad::contains(const CtVector2& point) {

	float centerDot = coordFrame.row[2].dot(coordFrame.row[0]);

	if (!intersectSlab(coordFrame.row[0], centerDot - extents.el[0], centerDot + extents.el[0], point))
		return false;

	centerDot = coordFrame.row[2].dot(coordFrame.row[1]);

	if (!intersectSlab(coordFrame.row[1], centerDot - extents.el[1], centerDot + extents.el[1], point))
		return false;

	return true;
}

void Quad::clip(const CtVector2& point, Vector2& result) {

	Matrix3x2 toLocal;
	Vector2 localPoint;

	inverse(coordFrame, toLocal);
	toLocal.transformPoint(point, localPoint);

	localPoint.x = tcap(localPoint.x, -extents.x, extents.x);
	localPoint.y = tcap(localPoint.y, -extents.y, extents.y);

	coordFrame.transformPoint(localPoint, result);
}

int Quad::rayIntersect(const CtVector2& point, const CtVector2& dir, float result[2]) {

	Span span;
	int resultCount = 0;

	if (rayIntersect(point, dir, span)) {

		if (span.min >= 0.0f) {

			result[resultCount] = span.min;
			++resultCount;
		}

		if (span.max >= 0.0f) {

			result[resultCount] = span.max;
			++resultCount;
		}
	}

	return resultCount;
}


int Quad::segIntersect(const CtVector2& point, const CtVector2& point2, float result[2]) {

	Vector2 lineDir;
	Span span;
	int resultCount = 0;

	if (segIntersect(point, point2, span, lineDir)) {

		if (span.min > 0.0f) {

			result[resultCount] = span.min;
			++resultCount;
		}

		if (span.max <= 1.0f) {

			result[resultCount] = span.max;
			++resultCount;
		}
	}

	return resultCount;
}

bool Quad::rayIntersect(const CtVector2& point, const CtVector2& dir, Span& result) {

	Span span;

	float centerDot = coordFrame.row[2].dot(coordFrame.row[0]);

	if (!intersectSlab(coordFrame.row[0], centerDot - extents.el[0], centerDot + extents.el[0], point, dir, span)) 
		return false;

	centerDot = coordFrame.row[2].dot(coordFrame.row[1]);

	if (!intersectSlab(coordFrame.row[1], centerDot - extents.el[1], centerDot + extents.el[1], point, dir, result)) 
		return false;

	if (!spanCheckGetOverlap(span, result, result)) {

		return false;
	}

	return true;
}

bool Quad::segIntersect(const CtVector2& point, const CtVector2& point2, Span& result, Vector2& lineDir) {

	Span span;

	point2.subtract(point, lineDir);

	float centerDot = coordFrame.row[2].dot(coordFrame.row[0]);

	if (!intersectSlab(coordFrame.row[0], centerDot - extents.el[0], centerDot + extents.el[0], point, lineDir, span)) 
		return false;

	centerDot = coordFrame.row[2].dot(coordFrame.row[1]);

	if (!intersectSlab(coordFrame.row[1], centerDot - extents.el[1], centerDot + extents.el[1], point, lineDir, result)) 
		return false;

	if (!spanCheckGetOverlap(span, result, result)) {

		return false;
	}

	float lineDirMag = lineDir.mag();
	Span segmentSpan(0.0f, lineDirMag);

	if (!spanCheckGetOverlap(result, segmentSpan, result)) {

		return false;
	}

	span.min /= lineDirMag;
	span.max /= lineDirMag;

	return true;
}

}