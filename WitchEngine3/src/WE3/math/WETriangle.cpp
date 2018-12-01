#include "WETriangle.h"

#include "../WEMacros.h"
#include "../WEAssert.h"

namespace WE {

void Triangle::addTo(AAB& box) const {

	box.add(vertices[0]);
	box.add(vertices[1]);
	box.add(vertices[2]);
}

void Triangle::computeEdges(Vector edges[3]) const {

	vertices[1].subtract(vertices[0], edges[0]);
	vertices[2].subtract(vertices[1], edges[1]);
	vertices[0].subtract(vertices[2], edges[2]);
}

void Triangle::computeBasis(Vector edges[2]) const {

	vertices[1].subtract(vertices[1], edges[0]);
	vertices[2].subtract(vertices[2], edges[0]);
}

void IndexedTriangle::addTo(AAB& box, Point* vertexArray) const {

	box.add(vertexArray[indices[0]]);
	box.add(vertexArray[indices[1]]);
	box.add(vertexArray[indices[2]]);
}

void IndexedTriangle::resolve(Point* vertexArray, Vector3* vertices) const {

	vertices[0] = vertexArray[indices[0]];
	vertices[1] = vertexArray[indices[1]];
	vertices[2] = vertexArray[indices[2]];
}

void TriangleEx1::initNormal(Point& v0, Point& v1, Point& v2, Vector& normal) {

	Vector temp[2];

	v1.subtract(v0, temp[0]);
	v2.subtract(v0, temp[1]);

	temp[0].cross(temp[1], normal);
	normal.normalize();
}

void TriangleEx1::initNormal() {

	initNormal(vertices[0], vertices[1], vertices[2], normal);

}

void IndexedTriangleEx1::resolve(Point* vertexArray, Vector3* vertices, Vector3& _normal) const {

	IndexedTriangle::resolve(vertexArray, vertices);
	_normal = normal;
}

IndexedTriangleEx1Adj::IndexedTriangleEx1Adj() {

	adjFace[0] = 0;
	adjFace[1] = 0;
	adjFace[2] = 0;
}

TriAdj IndexedTriangleEx1Adj::edgeType(short edgeV1, short edgeV2, bool& isPoint) {

	isPoint = false;

	switch (edgeV1) {
		case 0:
			{
				switch (edgeV2) {
				case 0:
					isPoint = true;
					return TA_Edge0;
				case 1:
					return TA_Edge01;
				case 2:
					return TA_Edge02;
				}
			}
			break;

			case 1:
			{
				switch (edgeV2) {
				case 0:
					return TA_Edge01;
				case 1:
					isPoint = true;
					return TA_Edge1;
				case 2:
					return TA_Edge12;
				}
			}
			break;

			case 2:
			{
				switch (edgeV2) {
				case 0:
					return TA_Edge02;
				case 1:
					return TA_Edge12;
				case 2:
					isPoint = true;
					return TA_Edge2;
				}
			}
			break;
	}

	assrt(false);
	return TA_EdgeNone;
}

void IndexedTriangleEx1Adj::setAdjFace(IndexedTriangleEx1* pFace, short edgeV1, short edgeV2) {

	TriAdj adj;
	bool isPoint;
	
	adj = edgeType(edgeV1, edgeV2, isPoint);

	assrt(isPoint == false);

	adjFace[adj] = pFace;
}

bool IndexedTriangleEx1Adj::setAdjFace(IndexedTriangleEx1Adj* pFace) {

	short sharedCount;
	unsigned int sharedInt[3];
	unsigned int sharedExt[3];

	sharedCount = 0;

	for (short i = 0; i < 3; i++) {
		
		for (short j = 0; j < 3; j++) {

			if (indices[i] == pFace->indices[j]) {

				sharedInt[sharedCount] = i;
				sharedExt[sharedCount] = j;

				sharedCount++;

				break;
			}
		}
	}

	if (sharedCount != 2) {

		return false;
	}

	setAdjFace(pFace, sharedInt[0], sharedInt[1]);
	pFace->setAdjFace(this, sharedExt[0], sharedExt[1]);

	return true;
}

bool IndexedTriangleEx1Adj::setAdjFace(IndexedTriangleEx1Adj* pFace, Point* vertexArray) {

	short sharedCount;
	unsigned int sharedInt[3];
	unsigned int sharedExt[3];

	sharedCount = 0;

	for (short i = 0; i < 3; i++) {
		
		for (short j = 0; j < 3; j++) {

			if (vertexArray[indices[i]] == vertexArray[pFace->indices[j]]) {

				sharedInt[sharedCount] = i;
				sharedExt[sharedCount] = j;

				sharedCount++;

				break;
			}
		}
	}

	if (sharedCount != 2) {

		return false;
	}

	setAdjFace(pFace, sharedInt[0], sharedInt[1]);
	pFace->setAdjFace(this, sharedExt[0], sharedExt[1]);

	return true;
}

inline bool isPoint(TriAdj edge) {

	switch (edge) {
		case TA_Edge0:
		case TA_Edge1:
		case TA_Edge2:
			return true;
	}

	return false;
}

inline void edgeLerp(const Vector3& normal, IndexedTriangleEx1* adjFace[3], TriAdj edge, Vector3& result) {

	//float dot;

	if (adjFace[edge] != 0) {

		//dot = normal.dot(adjFace[edge]->normal);

		//if (dot > 0.0f) {

			lerp(normal, adjFace[edge]->normal, 0.5f, result);
		//} else {

		//	result = normal;
		//}

	} else {

		result = normal;
	}
}

void IndexedTriangleEx1::resolveEdgeVertexIndices(TriAdj edge, VertexIndex& e1, VertexIndex& e2, bool sort) {

	switch(edge) {
		case TA_Edge0:
			e1 = indices[0];
			e2 = e1;
			break;
		case TA_Edge1:
			e1 = indices[1];
			e2 = e1;
			break;
		case TA_Edge2:
			e1 = indices[2];
			e2 = e1;
			break;
		case TA_Edge01:
			e1 = indices[0];
			e2 = indices[1];
			break;
		case TA_Edge12:
			e1 = indices[1];
			e2 = indices[2];
			break;	
		case TA_Edge02:
			e1 = indices[0];
			e2 = indices[2];
			break;
	}


	if (sort && (e1 > e2)) {
		swapt<VertexIndex>(e1, e2);
	}
	
}

void IndexedTriangleEx1Adj::resolveEdgeNormal(Point* vertexArray, TriAdj edge, Vector3& edgeNormal) {

	Vector3 temp[2];

	
	if (isPoint(edge)) {

		switch (edge) {
			case TA_Edge0:
				{
					edgeLerp(normal, adjFace, TA_Edge01, temp[0]);
					edgeLerp(normal, adjFace, TA_Edge02, temp[1]);

					lerp(temp[0], temp[1], 0.5f, edgeNormal);
				}
				break;
			case TA_Edge1:
				{
					edgeLerp(normal, adjFace, TA_Edge01, temp[0]);
					edgeLerp(normal, adjFace, TA_Edge12, temp[1]);

					lerp(temp[0], temp[1], 0.5f, edgeNormal);
				}
				break;
			case TA_Edge2:
				{
					edgeLerp(normal, adjFace, TA_Edge12, temp[0]);
					edgeLerp(normal, adjFace, TA_Edge02, temp[1]);

					lerp(temp[0], temp[1], 0.5f, edgeNormal);
				}
				break;
		}

	} else {

		edgeLerp(normal, adjFace, edge, edgeNormal);
	}

	edgeNormal.normalize();
}

void IndexedTriangleEx1Adj::resolveEdgeNormal(Point* vertexArray, short edgeV1, short edgeV2, Vector3& edgeNormal) {

	TriAdj edge;
	bool isPoint;
	
	edge = edgeType(edgeV1, edgeV2, isPoint);

	resolveEdgeNormal(vertexArray, edge, edgeNormal);
}

void IndexedTriangleEx1Adj::resolve(Point* vertexArray, TriangleEdgeNormals& resolved) {

	IndexedTriangle::resolve(vertexArray, resolved.vertices);

	resolved.normal = normal;
	resolveEdgeNormal(vertexArray, TA_Edge01, resolved.normals[0]);
	resolveEdgeNormal(vertexArray, TA_Edge12, resolved.normals[1]);
	resolveEdgeNormal(vertexArray, TA_Edge02, resolved.normals[2]);
}

void TriangleEx2::initAxis(Point& v0, Point& v1, Point& v2, Vector axis[3]) {

	v1.subtract(v0, axis[0]);
	v2.subtract(v1, axis[1]);
	v0.subtract(v2, axis[2]);

}

void TriangleEx2::initAxis() {

	initAxis(vertices[0], vertices[1], vertices[2], axis);
}

void IndexedTriangleEx1::initNormal(Point* vertexArray) {

	TriangleEx1::initNormal(vertexArray[indices[0]], vertexArray[indices[1]], vertexArray[indices[2]], normal);
}

void IndexedTriangleEx1::initNormal(Point& v0, Point& v1, Point& v2) {

	TriangleEx1::initNormal(v0, v1, v2, normal);
}

void IndexedTriangleEx2::initAxis(Point& v0, Point& v1, Point& v2) {
	
	TriangleEx2::initAxis(v0, v1, v2, axis);
}

}