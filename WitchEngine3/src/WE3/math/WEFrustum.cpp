#include "WEFrustum.h"

#include "WESpan.h"

namespace WE {

Frustum::Frustum() {

	invalidate();
}

void Frustum::invalidate() {

	isValidPointsExtents = false;
	isValidExtraPoints = false;
	isValidNormalsEdges = false;
	isValidPlanes = false;
}


void Frustum::updatePlanes() {

	if (isValidPlanes)
		return;

	isValidPlanes = true;

	updatePointsExtents();
	updateNormalsEdges();

	Vector3 normal;

	normals[N_Far].negate(normal);
	planes[Pl_Near].init(points[P_Near], normal, true);

	planes[Pl_Far].init(points[P_Far], normals[N_Far], true);
	planes[Pl_Left].init(extraPoints[EP_NearLeft], normals[N_Left], true);
	planes[Pl_Right].init(extraPoints[EP_NearRight], normals[N_Right], true);
	planes[Pl_Top].init(extraPoints[EP_NearUp], normals[N_Top], true);
	planes[Pl_Bottom].init(extraPoints[EP_NearDown], normals[N_Bottom], true);
}

void Frustum::updateNormalsEdges() {

	if (isValidNormalsEdges)
		return;

	isValidNormalsEdges = true;

	updatePointsExtents();
	updateExtraPoints();

	normals[N_Far] = dVector();

	edges[0] = lVector();
	edges[1] = uVector();

	Vector3 normal;

	const Vector3& nearPoint = points[P_Near];
	
	int edgeIndex = 2;

	{
		extraPoints[EP_NearUp].subtract(origin(), edges[edgeIndex]);

		edges[edgeIndex].cross(lVector(), normals[N_Top]);
		normals[N_Top].normalize();
		++edgeIndex;
	}

	{
		extraPoints[EP_NearLeft].subtract(origin(), edges[edgeIndex]);

		edges[edgeIndex].cross(uVector(), normals[N_Left]);
		normals[N_Left].normalize();
		++edgeIndex;
	}
	
	{
		extraPoints[EP_NearDown].subtract(origin(), edges[edgeIndex]);

		edges[edgeIndex].cross(lVector(), normals[N_Bottom]);
		normals[N_Bottom].normalize();
		++edgeIndex;
	}

	{
		extraPoints[EP_NearRight].subtract(origin(), edges[edgeIndex]);

		edges[edgeIndex].cross(uVector(), normals[N_Right]);
		normals[N_Right].normalize();
		++edgeIndex;
	}
}

void Frustum::updateExtraPoints() {

	if (isValidExtraPoints)
		return;

	isValidExtraPoints = true;

	updatePointsExtents();

	const Vector3& nearPoint = points[P_Near];
	Vector3 offset;
	Vector3 offset2;
	Vector3 tempVect;

	{
		uVector().mul(uBound(), offset);
		nearPoint.add(offset, extraPoints[EP_NearUp]);
	}

	{
		offset2 = offset;

		lVector().mul(lBound(), offset);
		nearPoint.add(offset, extraPoints[EP_NearLeft]);

		extraPoints[EP_NearLeft].add(offset2, extraPoints[EP_NearLeftUp]);
	}
	
	{
		offset2 = offset;

		uVector().mul(-uBound(), offset);
		nearPoint.add(offset, extraPoints[EP_NearDown]);

		extraPoints[EP_NearDown].add(offset2, extraPoints[EP_NearLeftDown]);
	}

	{
		offset2 = offset;

		lVector().mul(-lBound(), offset);
		nearPoint.add(offset, extraPoints[EP_NearRight]);

		extraPoints[EP_NearRight].add(offset2, extraPoints[EP_NearRightDown]);
	}

	{
		
		lVector().mul(-lBound(), offset);
		uVector().mul(uBound(), offset2);

		nearPoint.add(offset, tempVect);
		tempVect.add(offset2, extraPoints[EP_NearRightUp]);
	}
}

void Frustum::updatePointsExtents() {

	if (isValidPointsExtents)
		return;

	isValidPointsExtents = true;

	Vector3 offset;

	coordFrame.row[Frustum::DVector].mul(extents[Frustum::Near], offset);
	coordFrame.row[Frustum::Origin].add(offset, points[P_Near]);

	coordFrame.row[Frustum::DVector].mul(extents[Frustum::Far], offset);
	coordFrame.row[Frustum::Origin].add(offset, points[P_Far]);

	float dratio = dRatio();

	extraExtents[E_FarLeft] = l() * dratio;
	extraExtents[E_FarUp] = u() * dratio;
}

void Frustum::computeSpan(const Vector& axis, float& spanMin, float& spanMax) {

	//not sure about this or maybe theres a better way...

	updatePointsExtents();

	float p[2];
	float r[2];

	{
		const Vector3& nearPoint = points[P_Near];
		
		p[0] = _span(axis, nearPoint);
		r[0] = fabs(_span(axis, lVector())) * lBound()
					+ fabs(_span(axis, uVector())) * uBound();
	}

	{
		const Vector3& farPoint = points[P_Far];
		const float& farLeftExtent = extraExtents[E_FarLeft];
		const float& farUpExtent = extraExtents[E_FarUp];
		
		p[1] = _span(axis, farPoint);
		r[1] = fabs(_span(axis, lVector())) * farLeftExtent
					+ fabs(_span(axis, uVector())) * farUpExtent;
	}

	spanMin = MMin(p[0] - r[0], p[1] - r[1]);
	spanMax = MMax(p[0] + r[0], p[1] + r[1]);
}




}