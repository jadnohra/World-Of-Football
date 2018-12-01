#include "WESpatialTriBufferImpl.h"

#include "../../WETL/WETLArray.h"

namespace WE {

/*
	This will be rewritten as a geometry algo, working on flexible vertex formats

	Inspired from 

	'Evaluation of Memoryless Simplification, Peter Lindstrom and Greg Turk' 1999
	(tcvg99.pdg)
*/

bool compatibleNormals(const Vector3& n1, const Vector3& n2, const float& planarAngle) {

	float angleCos = n1.dot(n2) / n1.mag() * n2.mag();
	float e = sinf(planarAngle);

	return fabs(1.0f - angleCos) <= e;
}


typedef SpatialTriBufferImpl::Index TriIndex;

struct TriState;
class EdgeVertex;
struct Edge;

typedef WETL::CountedPtrArray<EdgeVertex*, false, TriIndex> EdgeVertices;
typedef WETL::CountedPtrArray<Edge*, false, TriIndex> Edges;

typedef WETL::StaticArray<TriState, false, TriIndex> TriStates;

struct TriVertex {

	TriIndex tri;
	TriIndex vertex;

	TriVertex() {}
};

typedef WETL::Array<TriVertex, false, TriIndex> TriVertices;

struct TriState {

	enum StateType {

		Intact = 0, Collapsed, Modified
	};

	StateType state;
	TriIndex edgeVertices[3];
	TriIndex lastModifBatchIndex;

	TriState();

	void replaceEdgeVertexIndex(const TriIndex& oldIndex, const TriIndex& newIndex, const TriIndex& batchIndex);
	bool checkVertexMoved(EdgeVertices& edgeVertexArray, const TriIndex& oldIndex, const TriIndex& newIndex, const Point& newIndexPos);
	bool isCurrBatchModified(const TriIndex& batchIndex);
};

class EdgeVertex {
public:

	enum StateType {

		Normal = 0, Collapsed
	};

	
	typedef WETL::Array<TriIndex, false, TriIndex> EdgeIndices;

	StateType state;

	Point position;

	TriVertices vertices; //TriVertex
	EdgeIndices edges;

	float totalAngle;
	bool isInsidePoint;

	EdgeVertex();

	inline bool isNotCollapsed() {

		return state != Collapsed;
	}

	void addTriVertex(const TriIndex& tri, const TriIndex& vertex);
	void addEdge(const TriIndex& edge);
	void addAndCheckAngle(const Vector3* vertices, const int& baseIndex);
	bool shares2TrisWith(const EdgeVertex& comp, TriIndex sharedTris[2]) const;
	bool findTriVertex(const TriIndex& tri, TriIndex& vertex);
	bool checkCompatibleNormals(SpatialTriBufferImpl& parent, const Vector3& refNormal, const float& planarAngle);
	void onVertexMoved(TriStates& triStates, const TriIndex& oldIndex, const TriIndex& newIndex, const TriIndex& batchIndex);
	bool checkVertexMoved(TriStates& triStates, EdgeVertices& edgeVertexArray, const TriIndex& oldIndex, const TriIndex& newIndex, const Point& newIndexPos);
	void mergeFrom(Edges& edgeArr, EdgeVertex* pSrc[2], TriIndex srcIndices[2], 
						const TriIndex& newIndex, 
						const TriIndex& exceptionTri1, const TriIndex& exceptionTri2, 
						const TriIndex& exceptionEdge);


};


struct Edge {

	enum StateType {

		Normal = 0, Collapsed, Excluded
	};

	StateType state;

	TriIndex vertices[2]; //EdgeVertex
	TriIndex sharedTris[2];

	bool isCollapsable;

	Edge();

	inline bool isNotCollapsed() {

		return state != Collapsed;
	}

	bool findNoneEdgeVertex(EdgeVertices& edgeVertices, TriIndex tri, TriIndex& noneEdgeVertex);
	bool findTriEdge(EdgeVertices& edgeVertices, const TriIndex& tri, const TriIndex& baseVertex, TriIndex& otherVertex);
	bool findTwoEdges(Edges& edges, EdgeVertices& edgeVertices, const TriIndex& tri, const TriIndex& baseVertex, TriIndex twoEdges[2]);
	bool updateCollapsable(const TriIndex& selfIndex, SpatialTriBufferImpl& parent, Edges& edges, EdgeVertices& edgeVertices, TriStates& triStates, 
							const bool& useBatches, const TriIndex& batchIndex,
							TriIndex& noneEdgeVertex, TriIndex mergedEdges[2][2], bool anythingGoes, const float& planarAngle);
	bool tryCollapse(const TriIndex& selfIndex, SpatialTriBufferImpl& parent, Edges& edges, EdgeVertices& edgeVertices, TriStates& triStates, 
							const bool& useBatches, const TriIndex& batchIndex, bool anythingGoes, const float& planarAngle);

	void mergeWith(Edge* pEdge, const TriIndex& oldIndex, const TriIndex& newIndex);
	void onMovedEdgeVertex(const TriIndex& oldIndex, const TriIndex& newIndex);
};





TriState::TriState() : state(Intact), lastModifBatchIndex(0) {}

void TriState::replaceEdgeVertexIndex(const TriIndex& oldIndex, const TriIndex& newIndex, const TriIndex& batchIndex) {

	for (int i = 0; i < 3; ++i) {

		if (edgeVertices[i] == oldIndex) {

			edgeVertices[i] = newIndex;
		}
	}

	if (state != Collapsed) {

		state = Modified;
		lastModifBatchIndex = batchIndex;
	}

	for (int i = 0; i < 3; ++i) {

		for (int j = i + 1; j < 3; ++j) {

			if (edgeVertices[i] == edgeVertices[j]) {

				state = Collapsed;
			}
		}
	}
}

bool TriState::checkVertexMoved(EdgeVertices& edgeVertexArray, const TriIndex& oldIndex, const TriIndex& newIndex, const Point& newIndexPos) {

	Vector3 currWind;
	Vector3 newWind;

	Vector3 temp[2];

	edgeVertexArray.el[edgeVertices[2]]->position.subtract(edgeVertexArray.el[edgeVertices[0]]->position, temp[0]);
	edgeVertexArray.el[edgeVertices[2]]->position.subtract(edgeVertexArray.el[edgeVertices[1]]->position, temp[0]);

	temp[0].cross(temp[1], currWind);

	TriIndex newEdgeVertices[3];

	for (int i = 0; i < 3; ++i) {

		newEdgeVertices[i] = edgeVertices[i];

		if (newEdgeVertices[i] == oldIndex) {

			newEdgeVertices[i] = newIndex;
		}
	}

	edgeVertexArray.el[newEdgeVertices[2]]->position.subtract(edgeVertexArray.el[newEdgeVertices[0]]->position, temp[0]);
	edgeVertexArray.el[newEdgeVertices[2]]->position.subtract(edgeVertexArray.el[newEdgeVertices[1]]->position, temp[0]);

	temp[0].cross(temp[1], newWind);

	return (currWind.dot(newWind) >= 0.0f);
}

bool TriState::isCurrBatchModified(const TriIndex& batchIndex) {

	return state == Modified && lastModifBatchIndex == batchIndex;
}




EdgeVertex::EdgeVertex() : state(Normal), isInsidePoint(false), totalAngle(0.0f) {}

void EdgeVertex::addTriVertex(const TriIndex& tri, const TriIndex& vertex) {

	TriVertex& newEl = vertices.addOne();
	
	newEl.tri = tri;
	newEl.vertex = vertex;
}

void EdgeVertex::addEdge(const TriIndex& edge) {

	TriIndex& newEl = edges.addOne();
	
	newEl = edge;
}

void EdgeVertex::addAndCheckAngle(const Vector3* vertices, const int& baseIndex) {

	Vector3 temp[2];
	int indices[2] = { (baseIndex + 1) % 3, (baseIndex + 2) % 3 };

	for (int i = 0; i < 2; ++i) {

		vertices[indices[i]].subtract(vertices[baseIndex], temp[i]);
		temp[i].normalize();
	}
	
	float diff = acosf(temp[0].dot(temp[1]));
	totalAngle += diff;

	if (radToDeg(fabs(totalAngle)) - 360.0f >= -10.0f) {

		isInsidePoint = true;
	}
}


bool EdgeVertex::shares2TrisWith(const EdgeVertex& comp, TriIndex sharedTris[2]) const {

	int shareCount = 0;

	for (TriIndex i = 0 ; i < vertices.size; ++i) {

		for (TriIndex j = 0 ; j < comp.vertices.size; ++j) {

			if (vertices.el[i].tri == comp.vertices.el[j].tri) {

				bool found = false;

				for (int si = 0; si < shareCount; ++si) {

					if (sharedTris[si] == vertices.el[i].tri) {

						found = true;
						break;
					}
				}

				if (!found) {

					sharedTris[shareCount] = vertices.el[i].tri;
					++shareCount;

					if (shareCount == 2) {

						return true;
					}
				}
			}
		}
	}

	return false;
}

bool EdgeVertex::findTriVertex(const TriIndex& tri, TriIndex& vertex) {

	if (state == Collapsed)
		return false;

	for (TriIndex i = 0 ; i < vertices.size; ++i) {

		if (vertices.el[i].tri == tri) {

			vertex = vertices.el[i].vertex;
			return true;
		}
	}

	return false;
}

bool EdgeVertex::checkCompatibleNormals(SpatialTriBufferImpl& parent, const Vector3& refNormal, const float& planarAngle) {

	if (state == Collapsed)
		return false;

	SpatialTriBufferImpl::Tri1 holder;

	for (TriIndex i = 0 ; i < vertices.size; ++i) {

		const Vector3& normal = parent.get(vertices.el[i].tri, holder).normal;

		if (!compatibleNormals(refNormal, normal, planarAngle)) {

			return false;
		}
	}

	return true;
}

void EdgeVertex::onVertexMoved(TriStates& triStates, const TriIndex& oldIndex, const TriIndex& newIndex, const TriIndex& batchIndex) {

	for (TriIndex i = 0 ; i < vertices.size; ++i) {

		triStates.el[vertices.el[i].tri].replaceEdgeVertexIndex(oldIndex, newIndex, batchIndex);
	}
}

bool EdgeVertex::checkVertexMoved(TriStates& triStates, EdgeVertices& edgeVertexArray, const TriIndex& oldIndex, const TriIndex& newIndex, const Point& newIndexPos) {

	for (TriIndex i = 0 ; i < vertices.size; ++i) {

		if (!triStates.el[vertices.el[i].tri].checkVertexMoved(edgeVertexArray, oldIndex, newIndex, newIndexPos)) {

			return false;
		}
	}

	return true;
}

void EdgeVertex::mergeFrom(Edges& edgeArr, EdgeVertex* pSrc[2], TriIndex srcIndices[2], 
					const TriIndex& newIndex, 
					const TriIndex& exceptionTri1, const TriIndex& exceptionTri2, 
					const TriIndex& exceptionEdge) {

	isInsidePoint = pSrc[0]->isInsidePoint && pSrc[1]->isInsidePoint;

	{
		TriIndex mergedCount = 0;

		for (int i = 0; i < 2; ++i) {

			for (TriIndex vi = 0 ; vi < pSrc[i]->vertices.size; ++vi) {

				if (pSrc[i]->vertices.el[vi].tri != exceptionTri1 
					&& pSrc[i]->vertices.el[vi].tri != exceptionTri2) {

					++mergedCount;
				}
			}
		}

		vertices.reserve(mergedCount);

		TriIndex mergedIndex = 0;

		for (int i = 0; i < 2; ++i) {

			for (TriIndex vi = 0 ; vi < pSrc[i]->vertices.size; ++vi) {

				if (pSrc[i]->vertices.el[vi].tri != exceptionTri1 
					&& pSrc[i]->vertices.el[vi].tri != exceptionTri2) {

					vertices.el[mergedIndex] = pSrc[i]->vertices.el[vi];

					++mergedIndex;
				}
			}
		}
	}

	{
		TriIndex mergedCount = 0;

		for (int i = 0; i < 2; ++i) {

			for (TriIndex vi = 0 ; vi < pSrc[i]->edges.size; ++vi) {

				if (pSrc[i]->edges.el[vi] != exceptionEdge) {

					++mergedCount;
				}
			}
		}

		edges.reserve(mergedCount);

		TriIndex mergedIndex = 0;

		for (int i = 0; i < 2; ++i) {

			for (TriIndex vi = 0 ; vi < pSrc[i]->edges.size; ++vi) {

				if (pSrc[i]->edges.el[vi] != exceptionEdge) {

					edges.el[mergedIndex] = pSrc[i]->edges.el[vi];
					edgeArr.el[edges.el[mergedIndex]]->onMovedEdgeVertex(srcIndices[i], newIndex);

					++mergedIndex;
				}
			}
		}
	}
}




Edge::Edge() : state(Normal), isCollapsable(false) {}

bool Edge::findNoneEdgeVertex(EdgeVertices& edgeVertices, TriIndex tri, TriIndex& noneEdgeVertex) {

	if (state == Collapsed) {

		return false;
	}

	TriIndex foundVertices[2];

	bool found = true;

	if (!edgeVertices.el[vertices[0]]->findTriVertex(tri, foundVertices[0]))
		found = false;

	if (!edgeVertices.el[vertices[1]]->findTriVertex(tri, foundVertices[1]))
		found = false;

	if (!found)
		return false;

	if (foundVertices[0] == 0) {

		if (foundVertices[1] == 1) {

			noneEdgeVertex = 2;

		} else {

			noneEdgeVertex = 1;
		}

	} else if (foundVertices[0] == 1) {

		if (foundVertices[1] == 0) {

			noneEdgeVertex = 2;

		} else {

			noneEdgeVertex = 0;
		}

	} else {

		if (foundVertices[1] == 0) {

			noneEdgeVertex = 1;

		} else {

			noneEdgeVertex = 0;
		}
	}

	return true;
}

bool Edge::findTriEdge(EdgeVertices& edgeVertices, const TriIndex& tri, const TriIndex& baseVertex, TriIndex& otherVertex) {

	if (state != Collapsed) {

		bool baseVertexFound = false;
		bool otherVertexFound = false;

		for (int i = 0; i < 2; ++i) {

			TriIndex foundVertex;

			if (edgeVertices.el[vertices[i]]->isNotCollapsed() &&
				edgeVertices.el[vertices[i]]->findTriVertex(tri, foundVertex)) {

				if (foundVertex == baseVertex) {

					baseVertexFound = true;

				} else {

					otherVertexFound = true;
					otherVertex = foundVertex;
				}
			}
		}

		return baseVertexFound && otherVertexFound;
	}

	return false;
}

bool Edge::findTwoEdges(Edges& edges, EdgeVertices& edgeVertices, const TriIndex& tri, const TriIndex& baseVertex, TriIndex twoEdges[2]) {

	TriIndex otherVertices[2];
	int otherVertexCount = 0;

	for (TriIndex ei = 0; ei < edges.count; ++ei) {

		TriIndex otherVertex;

		if (edges.el[ei]->isNotCollapsed() && edges.el[ei]->findTriEdge(edgeVertices, tri, baseVertex, otherVertex)) {

			bool found = false;

			for (int ovi = 0; ovi < otherVertexCount; ++ovi) {

				if (otherVertices[otherVertexCount] == otherVertex) {

					found = true;
					break;
				}
			}

			if (!found) {

				otherVertices[otherVertexCount] = otherVertex;
				twoEdges[otherVertexCount] = ei;
				++otherVertexCount;


				if (otherVertexCount == 2) {

					return true;
				}
			}
		}
	}

	return false;
}

bool Edge::updateCollapsable(const TriIndex& selfIndex, SpatialTriBufferImpl& parent, Edges& edges, EdgeVertices& edgeVertices, TriStates& triStates, 
								const bool& useBatches, const TriIndex& batchIndex,
								TriIndex& noneEdgeVertex, TriIndex mergedEdges[2][2], bool anythingGoes,
								const float& planarAngle) {

	if (state == Normal) {

		isCollapsable = false;

		//check for inside points
		for (int i = 0; i < 2; ++i) {

			if (!edgeVertices[vertices[i]]->isInsidePoint) {

				state = Excluded;
				return false;
			}
		}

		if (useBatches) {

			for (int i = 0; i < 2; ++i) {

				if (triStates.el[sharedTris[i]].isCurrBatchModified(batchIndex)) {

					//delay
					return false;
				}
			}
		}
		
		//check for shared tri normals
		Vector3 refNormal;
		SpatialTriBufferImpl::Tri1 holder;

		refNormal = parent.get(sharedTris[0], holder).normal;
		const Vector3& normal = parent.get(sharedTris[1], holder).normal;

		if (!compatibleNormals(refNormal, normal, planarAngle)) {

			state = Excluded;
			return false;
		}

		//now check for normals
		for (int i = 0; i < 2; ++i) {

			if (!edgeVertices.el[vertices[i]]->checkCompatibleNormals(parent, refNormal, planarAngle)) {

				state = Excluded;
				return false;
			}
		}

		//get merged edges
		for (int i = 0; i < 2; ++i) {

			if (!findNoneEdgeVertex(edgeVertices, sharedTris[i], noneEdgeVertex)) {

				state = Excluded;
				return false;
			}

			if (!findTwoEdges(edges, edgeVertices, sharedTris[i], noneEdgeVertex, mergedEdges[i])) {

				state = Excluded;
				return false;
			}
		}

		if (!anythingGoes) {

			EdgeVertex* pCollapsed[2] = { edgeVertices.el[vertices[0]], edgeVertices.el[vertices[1]] };
			TriIndex newIndex = vertices[0];
			Point newPos;

			lerp(pCollapsed[0]->position, pCollapsed[1]->position, 0.5f, newPos);

			//change tri states
			{
				for (int i = 0; i < 2; ++i) {

					if (!edgeVertices.el[vertices[i]]->checkVertexMoved(triStates, edgeVertices, vertices[1], newIndex, batchIndex)) {

						return false;
					}
				}
			}
		}

		isCollapsable = true;
		return true;

	} else {

		isCollapsable = false;
	}

	return isCollapsable;
}

bool Edge::tryCollapse(const TriIndex& selfIndex, SpatialTriBufferImpl& parent, Edges& edges, EdgeVertices& edgeVertices, TriStates& triStates, 
							const bool& useBatches, const TriIndex& batchIndex, bool anythingGoes,
							const float& planarAngle) {

	TriIndex noneEdgeVertex;
	TriIndex mergedEdges[2][2];

	if (updateCollapsable(selfIndex, parent, edges, edgeVertices, triStates, useBatches, batchIndex, noneEdgeVertex, mergedEdges, anythingGoes, planarAngle)) {
		
		//do collapse
		{
			EdgeVertex* pCollapsed[2] = { edgeVertices.el[vertices[0]], edgeVertices.el[vertices[1]] };
			TriIndex newIndex = vertices[0];
			Point newPos;

			lerp(pCollapsed[0]->position, pCollapsed[1]->position, 0.5f, newPos);

			//change tri states
			{
				for (int i = 0; i < 2; ++i) {

					edgeVertices.el[vertices[i]]->onVertexMoved(triStates, vertices[1], newIndex, batchIndex);
				}

				triStates.el[sharedTris[0]].state = TriState::Collapsed;
				triStates.el[sharedTris[1]].state = TriState::Collapsed;
			}


			//change edge vertices
			{
				SoftPtr<EdgeVertex> newVertex;
				WE_MMemNew(newVertex, EdgeVertex());

				newVertex->position = newPos;

				pCollapsed[0]->state = EdgeVertex::Collapsed;
				pCollapsed[1]->state = EdgeVertex::Collapsed;

				newVertex->mergeFrom(edges, pCollapsed, vertices, vertices[0], sharedTris[0], sharedTris[1], selfIndex);

				WE_MMemDelete(edgeVertices.el[newIndex]);
				edgeVertices.el[newIndex] = newVertex.ptr();
			}

			//change edge
			{
				state = Collapsed; //this probably already happened in mergeFrom ... Edge_onMovedEdgeVertex

				for (int i = 0; i < 2; ++i) {

					edges.el[mergedEdges[i][0]]->mergeWith(edges.el[mergedEdges[i][1]], mergedEdges[i][1], mergedEdges[i][0]);
				}
			}

			return true;
		}
	}

	return false;
}

void Edge::mergeWith(Edge* pEdge, const TriIndex& oldIndex, const TriIndex& newIndex) {

	pEdge->state = Collapsed;
	state = Normal;

	//replace tri
	{

		for (int i = 0; i < 2; ++i) {

			for (int j = 0; j < 2; ++j) {

				if (sharedTris[j] == pEdge->sharedTris[i]) {

					sharedTris[j] = pEdge->sharedTris[(i + 1) % 2];
					break;
				}
			}
		}
	}

}

void Edge::onMovedEdgeVertex(const TriIndex& oldIndex, const TriIndex& newIndex) {

	for (int i = 0; i < 2; ++i) {

		if (vertices[i] == oldIndex) {

			vertices[i] = newIndex;
		}
	}

	if (vertices[0] != vertices[1]) {

		state = Edge::Normal;
	}
}

void simplify_Iterate(SpatialTriBufferImpl& parent, int limit, TriStates& triStates, EdgeVertices& edgeVertices, Edges& edges, 
					  bool& centerIsValid, AAB& volume, Point& center, bool anythingGoes, bool useBatches,
					  const float& planarAngle) {

	if (centerIsValid) {

		unsigned long totalCollapsedCount = 0;
		TriIndex collapsedCount = 0;
		TriIndex batchIndex = 0;

		if (totalCollapsedCount < limit) {

			do {

				collapsedCount = 0;
	
				for (TriIndex ei = 0; ei < edges.count; ++ei) {

					if (edges.el[ei]->tryCollapse(ei, parent, edges, edgeVertices, triStates, useBatches, batchIndex, anythingGoes, planarAngle)) {

						++collapsedCount;
						++totalCollapsedCount;
					} 

					if (totalCollapsedCount == limit)
						break;
				}

				if (totalCollapsedCount == limit)
					break;

				if (collapsedCount == 0) 
					anythingGoes = true;


				++batchIndex;

			} while (collapsedCount || (anythingGoes == false));
		}
	}

}

void simplify_ClosestToCenter(SpatialTriBufferImpl& parent, int limit, TriStates& triStates, EdgeVertices& edgeVertices, Edges& edges, 
					  bool& centerIsValid, AAB& volume, Point& center, bool anythingGoes, bool useBatches, 
					  const float& planarAngle) {

	//simplify (collapse)
	if (centerIsValid) {

		unsigned long totalCollapsedCount = 0;
		TriIndex collapsedCount = 0;
		TriIndex collapsableCount = 0;
		TriIndex batchIndex = 0;

		TriIndex noneEdgeVertex;
		TriIndex mergedEdges[2][2];

		float centerDistSq;
		TriIndex closestEdge;

		useBatches = false;

		if (totalCollapsedCount < limit) {

			do {

				collapsedCount = 0;
				collapsableCount = 0;
				centerDistSq = FLT_MAX;
				
				for (TriIndex ei = 0; ei < edges.count; ++ei) {

					if (edges.el[ei]->updateCollapsable(ei, parent, edges, edgeVertices, triStates, useBatches, batchIndex, noneEdgeVertex, mergedEdges, anythingGoes, planarAngle)) {

						++collapsableCount;

						Point midPt;

						lerp(edgeVertices.el[edges.el[ei]->vertices[0]]->position, edgeVertices.el[edges.el[ei]->vertices[1]]->position, 0.5f, midPt);

						float distSq = distanceSq(midPt, center);

						//float distSq = MMin(distanceSq(edgeVertices.el[edges.el[ei]->vertices[0]]->position, center), distanceSq(edgeVertices.el[edges.el[ei]->vertices[1]]->position, center));

						if (distSq < centerDistSq) {

							centerDistSq = distSq;
							closestEdge = ei;
						}
					} 
				}

				if (collapsableCount) {

					if (edges.el[closestEdge]->tryCollapse(closestEdge, parent, edges, edgeVertices, triStates, useBatches, batchIndex, anythingGoes, planarAngle)) {
					
						++collapsedCount;
						++totalCollapsedCount;
					} 

				} else {

					anythingGoes = true;
				}

				if (totalCollapsedCount == limit)
					break;


			} while (collapsedCount || collapsableCount || (anythingGoes == false));
		}
	}
}

void SpatialTriBufferImpl::simplify(const Simplification* _pSettings) {

	TriStates triStates;
	EdgeVertices edgeVertices;
	Edges edges;

	bool centerIsValid = false;
	AAB volume(true);
	Point center;

	Simplification kSettings;
	const Simplification& settings = _pSettings ? *_pSettings : kSettings;


	{
		triStates.setSize(getCount());
		
		//float minDist = FLT_MAX;

		//init vertices
		{
			edgeVertices.reserve(triStates.size * 2);

			Tri1 holder;

			for (TriIndex i = 0; i < triStates.size; ++i) {

				const Tri1& tri = get(i, holder);

				for (int vi = 0; vi < 3; ++vi) {

					/*
					if (tri.normal.el[1] < 0.5f) {

						triStates.el[i].state = TriState::Collapsed;

						continue;
					} 
					*/

					bool found = false;

					for (TriIndex vei = 0; vei < edgeVertices.count; ++vei) {

						if (edgeVertices.el[vei]->position.equals(tri.vertices[vi])) {

							EdgeVertex& edgeVert = dref(edgeVertices.el[vei]);

							edgeVert.addTriVertex(i, vi);
							triStates.el[i].edgeVertices[vi] = vei;

							bool wasInside = edgeVert.isInsidePoint;

							edgeVert.addAndCheckAngle(tri.vertices, vi);
							
							if (!wasInside && edgeVert.isInsidePoint) {

								centerIsValid = true;
								volume.merge(edgeVert.position.el);
							}

							found = true;
							break;
						}

						/*
						if (distance(edgeVertices.el[vei]->position, tri.vertices[vi]) <= minDist) {

							minDist = distance(edgeVertices.el[vei]->position, tri.vertices[vi]);
						}
						*/
					}

					if (!found) {

						TriIndex newIndex = edgeVertices.count;
						EdgeVertex*& newEl = edgeVertices.addOne();

						WE_MMemNew(newEl, EdgeVertex());

						newEl->position = tri.vertices[vi];
						newEl->addTriVertex(i, vi);
						triStates.el[i].edgeVertices[vi] = newIndex;

						newEl->addAndCheckAngle(tri.vertices, vi);
					}
				}
			}
		}

		if (centerIsValid) {

			volume.center(center);
		}

		//init edges
		if (centerIsValid) {

			edges.reserve(triStates.size * 2);

			for (TriIndex vei = 0; vei < edgeVertices.count; ++vei) {

				EdgeVertex& edgeVert = *edgeVertices.el[vei];

				for (TriIndex vej = vei + 1; vej < edgeVertices.count; ++vej) {

					EdgeVertex& edgeVert2 = *edgeVertices.el[vej];
					TriIndex sharedTris[2];

					if (edgeVert.shares2TrisWith(edgeVert2, sharedTris)) {

						TriIndex edgeIndex = edges.count;
						Edge*& newEl = edges.addOne();

						WE_MMemNew(newEl, Edge());

						newEl->vertices[0] = vei;
						newEl->vertices[1] = vej;
						newEl->sharedTris[0] = sharedTris[0];
						newEl->sharedTris[1] = sharedTris[1];

						edgeVert.addEdge(edgeIndex);
						edgeVert2.addEdge(edgeIndex);
					}
				}
			}
		}

		//simplify (collapse)
		if (centerIsValid && settings.mode == Simplification::M_Planar) {

			int limit = settings.stepLimit;
			Simplification::LoopMode loopMode = settings.loopMode;
			bool anythingGoes = !settings.considerAesthetics;
			bool useBatches = settings.useIslands;


			switch (loopMode) {

				case Simplification::LM_ClosestToCenter: {

					simplify_ClosestToCenter(*this, limit, triStates, edgeVertices, edges, 
												centerIsValid, volume, center, anythingGoes, useBatches, settings.planarAngle);

				} break;

				default: {

					simplify_Iterate(*this, limit, triStates, edgeVertices, edges, 
												centerIsValid, volume, center, anythingGoes, useBatches, settings.planarAngle);

				} break;
			}
		}
		

		//rebuild 
		{
			TriBuffer oldTris;

			mTris.memCloneTo(oldTris);

			TriIndex newTriCount = 0;

			for (TriIndex i = 0; i < triStates.size; ++i) {

				if (triStates.el[i].state != TriState::Collapsed) 
					++newTriCount;
			}

			mTris.resize(newTriCount);

			TriIndex newTriIndex = 0;

			for (TriIndex i = 0; i < triStates.size; ++i) {

				switch (triStates.el[i].state) {

					case TriState::Intact: {

						mTris.el[newTriIndex] = oldTris.el[i];

						++newTriIndex;

					} break;
				
					case TriState::Modified: {

						for (int vi = 0; vi < 3; ++vi) {

							mTris.el[newTriIndex].vertices[vi] = edgeVertices.el[triStates.el[i].edgeVertices[vi]]->position;
						}

						mTris.el[newTriIndex].initNormal();

						++newTriIndex;

					} break;

					default: {
					}
				}
			}
		}
	}
}

}