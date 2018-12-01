#ifndef h_WOF_match_CollRegistry
#define h_WOF_match_CollRegistry

#include "WE3/WETL/WETLArray.h"
#include "WE3/spatialManager/WESpatialManager.h"
#include "WE3/math/WEContactNormalSolver.h"
#include "WE3/math/WEContactDepth.h"
using namespace WE;

#include "../inc/CollisionDef.h"
#include "CollRecord.h"

namespace WOF { namespace match {

	class BallSimulation;

	class CollRegistry {
	public:

		typedef CollRecordIndex Index;

	public:

		void update(CollEngine& engine, DynamicVolumeCollider& dynCollider, bool useProxies = false);
		void update(CollMaskMatrix& maskMatrix, SpatialManager& spaceManager, SpatialObjectGroup& queryObject, DynamicVolumeCollider& dynCollider, CollRecordPool2& pool, bool useProxies = false);
		
		void render(Renderer& renderer, bool renderVolumes, const RenderColor* pVolColor, bool renderTris, const RenderColor* pTriColor, bool triWireFrame = true);

		void constrain(const OBB& dynCollider, Vector3& constrainContext, const Vector3& inVel, Vector3& outVel, Vector3& outCorrection, const int& ignoreAxis, const bool& considerTriEdges, float maxPenetrationDepth);
		bool constrainByTriRecord(const Index& recordIndex, const OBB& dynCollider, Vector3& constrainContext, const Vector3& inVel, Vector3& outVel, Vector3& outCorrection, const int& ignoreAxis, const bool& considerTriEdges, float maxPenetrationDepth, bool& isTouching, float guideAngleSinSpread = defGuideAngleSinSpread);
		
		void gatherTriContactNormals(const OBB& dynCollider, ContactNormalSolver& solver, const int& ignoreAxis, bool considerTriEdges = true);
		void gatherTriContactNormals(const OBB& dynCollider, ContactNormalSolver& solver, bool considerTriEdges = true);
		void gatherTriContactNormals(const AAB& dynCollider, ContactNormalSolver& solver, bool considerTriEdges = true);
		//only normals with positive dot product with the filterNormal (if any) will be taken
		void gatherTriNormals(ContactNormalSolver& solver, const Vector3* pFilterNormal = NULL);
		void gatherDynVolContactNormalsSwept(const Vector3& dynMoveDir, const float& moveDist, VolumeStruct& dynCollider, ContactNormalSolver& solver, bool useSimplifiedTest = false);
		void gatherVolContactNormalsSwept(const Vector3& dynMoveDir, const float& moveDist, VolumeStruct& dynCollider, ContactNormalSolver& solver, bool useSimplifiedTest = false);

		void addSweptBallContacts(WorldMaterialManager& matManager, 
											const WorldMaterialID& sphereMat, const float& radius, 
											const Vector3& pos, const Vector3& sweepVector, 
											SweptIntersectionArray& intersections, BallSimulation* pSimul2, bool executeMaterials);

	public:

		inline Index& getTriRecordCount() { return mTriRecords.count; }
		inline Index& getVolRecordCount() { return mVolRecords.count; }
		inline Index& getDynVolRecordCount() { return mDynVolRecords.count; }

		inline Index getTotalRecordCount() { return getTriRecordCount() + getVolRecordCount() + getDynVolRecordCount(); }

		inline CollRecordVol* getDynVolRecord(const Index& index) { return mDynVolRecords[index]; }
		inline CollRecordTri* getTriRecord(const Index& index) { return mTriRecords[index]; }

		void empty(CollRecordPool2& pool);

		CollRecordTri* addTri(CollRecordPool2& pool);
		void removeTri(const Index& index, CollRecordPool2& pool);
		
	protected:

		typedef WETL::CountedPtrArray<CollRecordTri*, false, Index> TriRecords;
		typedef WETL::CountedPtrArray<CollRecordVol*, false, Index> VolRecords;


	protected:

		CollRecordVol* addVol(VolRecords& records, CollRecordPool2& pool);
		void removeVol(VolRecords& records, const Index& index, CollRecordPool2& pool);

		void update(DynamicVolumeCollider& dynCollider, VolumeStruct& dynCollVolume, CollRecordPool2& pool, SpatialObjectGroup& queryObject, SpatialObjectGroup::TriContainer& collider);
		void update(VolRecords& records, DynamicVolumeCollider& dynCollider, VolumeStruct& dynCollVolume, CollRecordPool2& pool, SpatialVolumeBinding* pCollider, VolumeStruct& collVolume);

		void gatherVolContactNormalsSwept(const Vector3& dynMoveDir, const float& moveDist, VolRecords& volRecords, VolumeStruct& dynCollider, ContactNormalSolver& solver, bool useSimplifiedTest);

	protected:

		TriRecords mTriRecords;
		VolRecords mVolRecords;
		VolRecords mDynVolRecords;

	};

} }

#endif