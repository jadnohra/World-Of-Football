#ifndef h_WOF_match_ContactGeometry
#define h_WOF_match_ContactGeometry

#include "WE3/math/WETriangle.h"
#include "WE3/math/WESphere.h"
#include "WE3/math/dynamicIntersect/WEDynamicIntersect.h"
#include "WE3/WETL/WETLArray.h"
#include "WE3/phys/WEPhysTypes.h"
#include "WE3/phys/WEPhysRigidBody.h"
#include "WE3/phys/WEPhysMaterial.h"
#include "WE3/defaultImpl/world/WEWorldMaterialManager.h"
#include "WE3/object/WEObject.h"
using namespace WE;

#include "../inc/Index.h"
#include "SweptDataTypes.h"

namespace WOF { namespace match {
	
	class ContactGeometry {
	protected:
	
		enum CandidateSearchResult {

			CRT_NotRelated, CRT_Related, CRT_Duplicate
		};

		struct ContactGeometryCandidate {

			SweptIntersection* pSource;
			PlanarFaceIntersection::Type type;

			short pointCount;
			unsigned short pointIndices[2];
		};

		typedef WETL::IndexShort::Type IndexType;
		typedef WETL::CountedArray<ContactGeometryCandidate, false, IndexType> CandidateArray;
		typedef WETL::CountedArray<ContactGeometryCandidate*, false, IndexType> CandidatePointerArray;

		struct ContactGeometryGroup {

			IndexType candidateCount;
			CandidatePointerArray elements[3]; //0-Face, 1-Edge, 2-Point

			void reset();
			~ContactGeometryGroup();

			CandidateSearchResult testCandidate(Point* p, short pointCount, unsigned short* indices = NULL);
			
			static ContactGeometryGroup* merge(ContactGeometryGroup* pGroup1, ContactGeometryGroup* pGroup2);
		};

		typedef WETL::CountedPtrArray<ContactGeometryGroup*, true, IndexType> GroupArray;
		typedef WETL::CountedArray<ContactGeometryGroup*, false, IndexType> GroupPtrArray;


		GroupArray mGroups;
		CandidateArray mCandidates[3]; //0-Face, 1-Edge, 2-Point

		bool mGeometryChanged;

		Sphere mSphere;
		Vector3 mSwept;

		GroupPtrArray mFoundGroups;

		PhysRigidBody* mpBody1;
		PhysRigidBody* mpBody2;
		PhysMaterial* mpMaterial;

		#ifdef _DEBUG
		WETL::IndexShort mMaxCounts[3];
		#endif

//		void addCandidateToArrays(ContactGeometryCandidate& candidate, SweptContactArray& contactContainer, SweptDetectionFillArrays& arrays);
		void setResultContact(SweptIntersection& newContact, ContactGeometryCandidate& source);
		void addCandidate(ContactGeometryGroup* pGroup, short pointCount, SweptIntersection* pSource, unsigned short* indices = NULL);
		ContactGeometryGroup* findGroupFor(Point* p, short pointCount, unsigned short* indices = NULL);

	public:

		ContactGeometry();
		~ContactGeometry();

		void init(WETL::IndexShort expectContactCount);
		void destroy();

		void setBodies(PhysRigidBody* pBody1, PhysRigidBody* pBody2, PhysMaterial* pMaterial);
				
		void begin(const Point& sphereCenter, float sphereRadius, Vector3& swept);
		void end();
		bool geometryChanged();

		void addCandidate(SweptIntersection* pCandidate); //pCandidate must stay valid until end() is called
		//void addCandidate(ContactGeometryPreCandidate& candidate);
		

//		void getContacts(SweptContactArray& contactContainer, SweptDetectionFillArrays& arrays);
		void getContacts(SweptIntersectionArray& container);
	};
} }


#endif