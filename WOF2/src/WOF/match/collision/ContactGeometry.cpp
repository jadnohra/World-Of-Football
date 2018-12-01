#include "ContactGeometry.h"

#include "WE3/WEMem.h"
#include "WE3/WEString.h"
#include "WE3/math/WEIntersect.h"
using namespace WE;


namespace WOF { namespace match {

#define MTypeToPointCount(type) (3 - (short) type)
#define MPointCountToType(count) ((PlanarFaceIntersection::Type) (3 - count))

inline Point& _pt(Point* pts, short index, unsigned short* indices) {

	return (indices ? pts[indices[index]] : pts[index]);
}

inline Point& _pti(Point* pts, short index, unsigned short* indices) {

	return pts[indices[index]];
}

ContactGeometry::ContactGeometryGroup::~ContactGeometryGroup() {
}

void ContactGeometry::ContactGeometryGroup::reset() {

	candidateCount = 0;

	elements[PlanarFaceIntersection::T_Face].count = 0;
	elements[PlanarFaceIntersection::T_Edge].count = 0;
	elements[PlanarFaceIntersection::T_Point].count = 0;
}

ContactGeometry::CandidateSearchResult 
	ContactGeometry::ContactGeometryGroup::testCandidate(Point* pts, short pointCount, unsigned short* indices) {

	if (candidateCount == 0) {

		return CRT_NotRelated;
	}

	bool relatedFound;
	short geomType;
	IndexType ptCount;
	short matchCount;
	short lastCount;
	ContactGeometryCandidate* pCandidate;
	Point* candidatePoints;
	unsigned short* candidateIndices;

	//1st search for duplicates
	bool cannotBeDuplicate;
	relatedFound = false;
	ptCount = pointCount;
	geomType = MPointCountToType(ptCount);
	
	for (IndexType i = 0; i < elements[geomType].count; i++) {

		pCandidate = elements[geomType].el[i];
		candidatePoints = pCandidate->pSource->points;
		candidateIndices = (pointCount == 3 ? NULL : pCandidate->pointIndices);

		matchCount = 0;
		cannotBeDuplicate = false;
		for (IndexType p = 0; p < ptCount; p++) {

			lastCount = matchCount;

			for (IndexType q = 0; q < pointCount; q++) {

				if (_pt(candidatePoints, p, candidateIndices).equals((_pt(pts, q, indices)))) {

					matchCount++;

					break;
				}
			}

			if (lastCount == matchCount) {

				cannotBeDuplicate = true;
			}

			if (matchCount && cannotBeDuplicate) {
				break;
			}

		}

		if (matchCount == ptCount) {

			return CRT_Duplicate;

		} else if (matchCount > 0) {

			relatedFound = true;
		} 
	}

	if (relatedFound) {

		return CRT_Related;
	}

	//search for related in 2 remaining geomTypes
	
	for (short _test = 0; _test < 2; _test++) {

		geomType = (geomType == 2 ? 0 : (geomType + 1));
		ptCount =  MTypeToPointCount(geomType);
		 
	
		for (IndexType i = 0; i < elements[geomType].count; i++) {

			pCandidate = elements[geomType].el[i];
			candidatePoints = pCandidate->pSource->points;
			candidateIndices = (pointCount == 3 ? NULL : pCandidate->pointIndices);

			for (IndexType p = 0; p < ptCount; p++) {

				for (IndexType q = 0; q < pointCount; q++) {

					if (_pt(candidatePoints, p, candidateIndices).equals((_pt(pts, q, indices)))) {

						return CRT_Related;
					}
				}
			}
		}
	} 

	return CRT_NotRelated;
}

/*
bool ContactGeometry::ContactGeometryGroup::containsAnyOf(Point* pts, short pointCount) {

	if (candidateCount) {

		return false;
	}

	int geomType = PlanarFaceIntersection::T_Point;
	IndexType ptCount = 1;

	while (geomType >= 0) {

		for (IndexType i = 0; i < elements[geomType].count; i++) {

			for (IndexType p = 0; p < ptCount; p++) {
				for (IndexType q = 0; q < pointCount; q++) {
					if (elements[geomType].el[i]->points[p].equals((pts[q]))) {
						return true;
					}
				}
			}

		}

		geomType--;
		ptCount++;
	} 

	return false;
}
*/

/*
ContactGeometry::ContactGeometryGroup* ContactGeometry::ContactGeometryGroup::merge(ContactGeometryGroup* pGroup1, ContactGeometryGroup* pGroup2) {

	ContactGeometryGroup* pNewMain;
	ContactGeometryGroup* pNewAttached;

	if (pGroup1->pNextAttachedGroup == NULL) {

		pNewMain = pGroup1;
		pNewAttached = pGroup2;

	} else {

		pNewMain = pGroup2;
		pNewAttached = pGroup1;
	}
	
	ContactGeometryGroup* pLastInMain = pNewMain;

	while (pLastInMain->pNextAttachedGroup) {
		pLastInMain = pLastInMain->pNextAttachedGroup;
	}

	pLastInMain->pNextAttachedGroup = pNewAttached;
	pNewAttached->pMainGroup = pNewMain;

	return pNewMain;
}
*/

ContactGeometry::ContactGeometryGroup* ContactGeometry::ContactGeometryGroup::merge(ContactGeometryGroup* pGroup1, ContactGeometryGroup* pGroup2) {

	ContactGeometryGroup* pNewMain;
	ContactGeometryGroup* pNewAttached;

	if (pGroup1->candidateCount > pGroup2->candidateCount) {

		pNewMain = pGroup1;
		pNewAttached = pGroup2;

	} else {

		pNewMain = pGroup2;
		pNewAttached = pGroup1;
	}
	
	for (short i = 0; i < 3; i++) {

		for (short j = 0; j < pNewAttached->elements[i].count; j++) {

			pNewMain->elements[i].addOne(pNewAttached->elements[i].el[j]);
		}
	}

	pNewMain->candidateCount += pNewAttached->candidateCount;
	pNewAttached->reset();
		
	return pNewMain;
}

ContactGeometry::ContactGeometry() {

	#ifdef _DEBUG
	mMaxCounts[PlanarFaceIntersection::T_Face] = 0;
	mMaxCounts[PlanarFaceIntersection::T_Edge] = 0;
	mMaxCounts[PlanarFaceIntersection::T_Point] = 0;
	#endif

}

ContactGeometry::~ContactGeometry() {

	destroy();
}

void ContactGeometry::init(WETL::IndexShort expectContactCount) {

	mFoundGroups.reserve(3);

	mCandidates[PlanarFaceIntersection::T_Face].reserve(expectContactCount);
	mCandidates[PlanarFaceIntersection::T_Edge].reserve(expectContactCount);
	mCandidates[PlanarFaceIntersection::T_Point].reserve(expectContactCount);
}

void ContactGeometry::destroy() {

	#ifdef _DEBUG
	String::debug(L"ContactGeometry maxCounts = %d, %d, %d\n", 
					mMaxCounts[PlanarFaceIntersection::T_Face], mMaxCounts[PlanarFaceIntersection::T_Edge], mMaxCounts[PlanarFaceIntersection::T_Point]);
	#endif

	mCandidates[PlanarFaceIntersection::T_Face].destroy();
	mCandidates[PlanarFaceIntersection::T_Edge].destroy();
	mCandidates[PlanarFaceIntersection::T_Point].destroy();
	mFoundGroups.destroy();
}

void ContactGeometry::setBodies(PhysRigidBody* pBody1, PhysRigidBody* pBody2, PhysMaterial* pMaterial) {

	mpBody1 = pBody1;
	mpBody2 = pBody2;

	mpMaterial = pMaterial;
}

void ContactGeometry::begin(const Point& sphereCenter, float sphereRadius, Vector& swept) {

	mSphere.center = sphereCenter;
	mSphere.radius = sphereRadius;
	mSwept = swept;

	mGeometryChanged = false;
}

bool ContactGeometry::geometryChanged() {

	return mGeometryChanged;
}

void ContactGeometry::end() {

	#ifdef _DEBUG
	mMaxCounts[PlanarFaceIntersection::T_Face] = MMax(mMaxCounts[PlanarFaceIntersection::T_Face], mCandidates[PlanarFaceIntersection::T_Face].count);
	mMaxCounts[PlanarFaceIntersection::T_Edge] = MMax(mMaxCounts[PlanarFaceIntersection::T_Edge], mCandidates[PlanarFaceIntersection::T_Edge].count);
	mMaxCounts[PlanarFaceIntersection::T_Point] = MMax(mMaxCounts[PlanarFaceIntersection::T_Point], mCandidates[PlanarFaceIntersection::T_Point].count);
	#endif

	mCandidates[PlanarFaceIntersection::T_Face].count = 0;
	mCandidates[PlanarFaceIntersection::T_Edge].count = 0;
	mCandidates[PlanarFaceIntersection::T_Point].count = 0;
	mGroups.count = 0;
}

ContactGeometry::ContactGeometryGroup* ContactGeometry::findGroupFor(Point* p, short pointCount, unsigned short* indices) {

	ContactGeometryGroup* pGroup;

	mFoundGroups.count = 0;

	for (IndexType i = 0; i < mGroups.count; i++) {

		switch (mGroups.el[i]->testCandidate(p, pointCount, indices)) {
			case CRT_Related:
				mGeometryChanged = true;
				mFoundGroups.addOne(mGroups.el[i]);
				break;
			case CRT_Duplicate:
				mGeometryChanged = true;
				return NULL;
				break;
			default:
				break;
		}
	}

	if (mFoundGroups.count == 0) {

		ContactGeometryGroup*& pNewGroup = (mGroups.addOne());
		if (pNewGroup == NULL) {

			MMemNew(pNewGroup, ContactGeometryGroup());
		}
		pGroup = pNewGroup;
		pGroup->reset();

		return pGroup;
	} 

	pGroup = mFoundGroups.el[0];
	
	for (IndexType i = 1; i < mFoundGroups.count; i++) {

		pGroup = ContactGeometryGroup::merge(pGroup, mFoundGroups.el[i]);
	}

	return pGroup;
}

void ContactGeometry::addCandidate(ContactGeometryGroup* pGroup, short pointCount, 
								   SweptIntersection* pSource, unsigned short* indices) {

	PlanarFaceIntersection::Type type;

	type = MPointCountToType(pointCount);
	ContactGeometryCandidate* pCandidate = &(mCandidates[type].addOne());

	pCandidate->pSource = pSource;
	pCandidate->pointCount = pointCount;
	pCandidate->type = type;

	if (indices && (pointCount != 3)) {

		for (short i = 0; i < pointCount; i++) {

			pCandidate->pointIndices[i] = indices[i];
		}
	} 

	pGroup->elements[type].addOne(pCandidate);
	pGroup->candidateCount++;
}

void ContactGeometry::addCandidate(SweptIntersection* pCandidate) {

	ContactGeometryGroup* pGroup;
	short ptCount;


	String::debug(L"begin: sphere center: [%f, %f, %f]\n", MExpand3(mSphere.center));

	String::debug(L"addCandidate: tri: [%f, %f, %f], [%f, %f, %f], [%f, %f, %f]\n", 
					MExpand3(pCandidate->points[0]), MExpand3(pCandidate->points[1]), MExpand3(pCandidate->points[2]));

	if (pCandidate->recheckGeometry == false) {

		ptCount = MTypeToPointCount(pCandidate->type);

		pGroup = findGroupFor(pCandidate->points, ptCount);
		if (pGroup) {
			addCandidate(pGroup, ptCount, pCandidate, pCandidate->edge); 
			String::debug(L"added Face\n");
		}

		return;
	}

	Vector3 swept;
	Point posAtContact;
	Point* vertices;

	mSwept.mul(pCandidate->sweptFraction, swept);
	mSphere.center.add(swept, posAtContact);

	vertices = pCandidate->points;
	
	ptCount = 3;

	pGroup = findGroupFor(vertices, ptCount);
	if (pGroup) {
		addCandidate(pGroup, ptCount, pCandidate); 
		String::debug(L"added Face\n");
	}

	unsigned short indices[2] = {0, 1};
	ptCount = 2;

	for (short edge = 0; edge < 3; edge++) {

		if (intersect(posAtContact, mSphere.radius, vertices[indices[0]], vertices[indices[1]])) {

			pGroup = findGroupFor(vertices, ptCount, indices);
			if (pGroup) {
				addCandidate(pGroup, ptCount, pCandidate, indices); 
				String::debug(L"added edge [%d %d]\n", indices[0], indices[1]);
				mGeometryChanged = true;
			}
		}

		indices[0] = (indices[0] + 1) % 3;
		indices[1] = (indices[1] + 1) % 3;
	}


	float radiusSquared = mSphere.radius * mSphere.radius;

	ptCount = 1;
	for (indices[0] = 0; indices[0] < 3; indices[0]++) {

		if (sphereSquaredContains(posAtContact, radiusSquared, vertices[indices[0]])) {

			pGroup = findGroupFor(vertices, ptCount, indices);
			if (pGroup) {
				addCandidate(pGroup, ptCount, pCandidate, indices);
				String::debug(L"added point [%d]\n", indices[0]);
				mGeometryChanged = true;
			}
		}
	}

}

void ContactGeometry::setResultContact(SweptIntersection& newContact, ContactGeometryCandidate& source) {

	newContact.recheckGeometry = false;
	newContact.contactPoint = source.pSource->contactPoint;
	newContact.sweptFraction = source.pSource->sweptFraction;

	newContact.type = source.type;

	if (source.pointCount == 3) {

		memcpy(newContact.points, source.pSource->points, 3 * sizeof(Point));

	} else {

		for (short p = 0; p < source.pointCount; p++) {

			newContact.points[p] = _pt(source.pSource->points, p, source.pointIndices);
			newContact.edge[p] = p;
		}
	}

	

	

}

void ContactGeometry::getContacts(SweptIntersectionArray& container) {

	ContactGeometryGroup* pGroup;
	IndexType count;
	IndexType min;
	IndexType minIndex;
	bool groupDone;

	for (IndexType i = 0; i < mGroups.count; i++) {

		pGroup = mGroups.el[i];
		
		min = (IndexType) -1;
		assrt(min > 0);

		groupDone = false;

		for (short j = 0; j < 3; j++) {

			count = pGroup->elements[j].count;

			if (count == 1) {

				//addCandidateToArrays(*(pGroup->elements[j].el[0]), container, arrays);
				
				groupDone = true;
				break;
			} else {

				if (count > 0 && count < min) {

					min = count;
					minIndex = j;
				}
			}
		}

		if (groupDone) {

			continue;
		} else {

			if (min != (IndexType) -1) {

				count = pGroup->elements[minIndex].count;

				for (short j = 0; j < count; j++) {

					//addCandidateToArrays(*(pGroup->elements[minIndex].el[j]), contactContainer, arrays);
				}
			}
		}
	}
}
		
/*
void ContactGeometry::addCandidateToArrays(ContactGeometryCandidate& candidate, SweptContactArray& contactContainer, SweptDetectionFillArrays& arrays) {

	SweptContactPtrArray* pArray = arrays.pArrays[candidate.dynamicType];

	assrt (pArray != NULL);

	SweptContact* pContact = pArray->addOne(Match&(contactContainer.addOne()));

	pContact->sweptFraction = candidate.sweptFraction;
	pContact->dynamics = candidate.dynamicType;


	//initContact(PhysRigidBody* pBody1, PhysRigidBody* pBody2, PhysMaterial* pMaterial, 
	//						Vector3& normal, Vector3& point, PhysScalar distance,
	//						Vector3* pBody1Pos = NULL, Vector3* pBody2Pos = NULL);
							


	Point posAtContact;
	Vector3 swept;
	PhysScalar penetration;

	mSwept.mul(candidate.sweptFraction, swept);
	mSphere.center.add(swept, posAtContact);

	//works for both normal and edge contacts because of sphericity
	//also automatically supports double sided faces becuase we are 
	//actually ignoring the direction of the face normal
	posAtContact.subtract(candidate.contactPoint, pContact->n);
	penetration = mSphere.radius - pContact->n.mag();
	pContact->n.normalize();


	pContact->initContact(mpBody1, mpBody2, mpMaterial, pContact->n, candidate.contactPoint, penetration);
}
*/

/*
void ContactGeometry::getContacts(SweptContactArray& contactContainer, SweptDetectionFillArrays& arrays) {

	ContactGeometryGroup* pGroup;
	IndexType count;
	IndexType min;
	IndexType minIndex;
	bool groupDone;

	for (IndexType i = 0; i < mGroups.count; i++) {

		pGroup = mGroups.el[i];
		
		min = (IndexType) -1;
		assrt(min > 0);

		groupDone = false;

		for (short j = 0; j < 3; j++) {

			count = pGroup->elements[j].count;

			if (count == 1) {

				addCandidateToArrays(*(pGroup->elements[j].el[0]), contactContainer, arrays);
				
				groupDone = true;
				break;
			} else {

				if (count > 0 && count < min) {

					min = count;
					minIndex = j;
				}
			}
		}

		if (groupDone) {

			continue;
		} else {

			if (min != (IndexType) -1) {

				count = pGroup->elements[minIndex].count;

				for (short j = 0; j < count; j++) {

					addCandidateToArrays(*(pGroup->elements[minIndex].el[j]), contactContainer, arrays);
				}
			}
		}
	}
}
*/

} }