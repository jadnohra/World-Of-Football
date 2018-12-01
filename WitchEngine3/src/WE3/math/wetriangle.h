#ifndef _WETriangle_h
#define _WETriangle_h

#include "../WEDataTypes.h"
#include "WEVector.h"
#include "WEAAB.h"

namespace WE {

	typedef unsigned int VertexIndex;

	struct Triangle {

		Point vertices[3];

		void addTo(AAB& box) const;
		void computeEdges(Vector edges[3]) const;
		void computeBasis(Vector edges[2]) const;
	};

	struct IndexedTriangle {
		UINT32 indices[3];

		void addTo(AAB& box, Point* vertexArray) const;
		void resolve(Point* vertexArray, Vector3* vertices) const;
	};

	
	struct TriangleEx1 : Triangle {
		Vector normal;

		TriangleEx1() {};
		TriangleEx1(const TriangleEx1& ref) {*this = ref;};

		void initNormal();
		static void initNormal(Point& v0, Point& v1, Point& v2, Vector& normal);
	};

	enum TriAdj {

		TA_Edge01 = 0, TA_Edge12, TA_Edge02, TA_Edge0, TA_Edge1, TA_Edge2, TA_EdgeNone
	};

	struct IndexedTriangleEx1 : IndexedTriangle {
		Vector normal;

		void initNormal(Point* vertexArray);
		void initNormal(Point& v0, Point& v1, Point& v2);
		void resolve(Point* vertexArray, Vector3* vertices, Vector3& normal) const;

		void resolveEdgeVertexIndices(TriAdj edge, VertexIndex& e1, VertexIndex& e2, bool sort);
	};

	
	struct TriangleEdgeNormals : TriangleEx1 {
		Vector3 normals[3];
	};

	struct IndexedTriangleEx1Adj : IndexedTriangleEx1 {
		//TODO change this to an index to reduce size?
		IndexedTriangleEx1* adjFace[3];

		IndexedTriangleEx1Adj();

		static TriAdj edgeType(short edgeV1, short edgeV2, bool& isPoint);
		void setAdjFace(IndexedTriangleEx1* pFace, short edgeV1, short edgeV2);
		bool setAdjFace(IndexedTriangleEx1Adj* pFace);
		bool setAdjFace(IndexedTriangleEx1Adj* pFace, Point* vertexArray); //compares the actual positions
		void resolveEdgeNormal(Point* vertexArray, short edgeV1, short edgeV2, Vector3& edgeNormal);
		void resolveEdgeNormal(Point* vertexArray, TriAdj edge, Vector3& edgeNormal);
		
		void resolve(Point* vertexArray, TriangleEdgeNormals& resolved);
	};

	struct TriangleEx2 : TriangleEx1 {
		Vector axis[3];

		void initAxis();
		static void initAxis(Point& v0, Point& v1, Point& v2, Vector axis[3]);
	};

	struct IndexedTriangleEx2 : IndexedTriangleEx1 {
		Vector axis[3];

		void initAxis(Point& v0, Point& v1, Point& v2);
	};


};


#endif