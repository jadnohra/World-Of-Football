#ifndef h_WOF_match_CollEngine
#define h_WOF_match_CollEngine

#include "WE3/spatialManager/WESpatialManager.h"
#include "WE3/render/WERenderer.h"
using namespace WE;

#include "Colliders.h"
#include "CollRecord.h"

namespace WOF { namespace match {

	class Match;

	class CollEngine {
	public:

		void destroy();

		void init(Match& match, SpatialManager& spatialManager, SpatialManagerBuildPhase* pBuildPhase);
		void setBuildPhase(SpatialManagerBuildPhase* pBuildPhase);

		void init(Match& match, Object& object, PolygonalCollider& collider);
		bool add(Match& match, Object& object, PolygonalCollider& collider, Mesh& mesh, const Matrix4x3Base* pTransform = NULL, bool simplifiyIfEnabled = true, bool* pResultIsEmpty = NULL);
		void destroy(Match& match, Object& object, PolygonalCollider& collider);

		void init(Match& match, Object& object, VolumeCollider& collider, const WorldMaterialID& materialID);
		bool add(Match& match, Object& object, VolumeCollider& collider, VolumeStruct& volume);
		void destroy(Match& match, Object& object, VolumeCollider& collider);

	
		void init(Match& match, Object& object, DynamicVolumeCollider& collider, const WorldMaterialID& materialID);
		bool add(Match& match, Object& object, DynamicVolumeCollider& collider, VolumeStruct& volume);
		Volume& updateStart(Match& match, Object& object, DynamicVolumeCollider& collider);
		void updateEnd(Match& match, Object& object, DynamicVolumeCollider& collider, Volume& volume);
		void destroy(Match& match, Object& object, DynamicVolumeCollider& collider);


	public:

		inline WorldMaterialManager& getMaterialManager() { return mMatManager; }
		inline SpatialManager& getSpatialManager() { return mSpatialManager; }
		inline CollRecordPool2& getCollRecordPool() { return mCollRecordPool; }
		inline CollMaskMatrix& getCollMaskMatrix() { return mCollMaskMatrix; }

		SpatialObjectGroup* getQueryObject() { return mQueryObject; }
		void putQueryObject(SpatialObjectGroup* pObj) { }

	protected:

		SoftPtr<Match> mMatch;

		HardPtr<WorldMaterialManager> mMatManager;

		SoftPtr<SpatialManager> mSpatialManager;
		SoftPtr<SpatialManagerBuildPhase> mSpatialManagerBuildPhase;
		HardPtr<SpatialObjectGroup> mQueryObject;

		CollRecordPool2 mCollRecordPool;
		CollMaskMatrix mCollMaskMatrix;
		
	};

} }

#endif