#ifndef _WESpatialManager_KDTreeBase_h
#define _WESpatialManager_KDTreeBase_h

#include "../WESpatialManager.h"
#include "../../math/WEAAB.h"
#include "../../math/WEPlane.h"
#include "WESpatialManager_KDTreeTriBufferManager.h"

namespace WE {

	class SpatialObjectGroup_KDTree;

	class SpatialManager_KDTreeBase {
	public:
		
		typedef SpatialListener_DynamicVolumeEvents SpatialDynamicListener;
		typedef SpatialManager_KDTreeTriBufferManager TriBufferManager;
		typedef SpatialManager_KDTreeTriBufferManager::TriBuffer TriBuffer;

		typedef SpatialIndex Index;

		struct TreeNode;
		struct TreeNodeBranch;

		typedef WETL::CountedArray<TreeNode*, false, Index, WETL::ResizeExact> NodePtrs;
		typedef WETL::SizeAllocT<Index, Index, false> SpatialIndices;

		struct SubdivisionSettings {

			Vector3 minCellSize;
			UINT32 maxDepth;
			Index maxCellElementCount;
			Index splitPreferenceFactor; //the bigger this is the more balanced splits will be made, the small, the more 'free space' splits will be made
			float freeSpaceLoad;
			float largeVolumeCoeff;

			SubdivisionSettings(const Vector3* pMinCellSize = NULL, const UINT32* pMaxDepth = NULL, 
								const Index* pMaxCellElementCount = NULL, const Index* pSplitPreferenceFactor = NULL,
								const float* pFreeSpaceLoad = NULL, const float* pLargeVolumeCoeff = NULL); 
		};

		struct PlaneClassifyCount {

			Index count[3];

			PlaneClassifyCount();

			bool equals(const PlaneClassifyCount& comp) const;
		};

		struct StaticTriCont {

			SoftPtr<TriBuffer> buffer;
			SpatialIndices mTriIndices;
		};

		struct StaticVolume {

			SoftPtr<SpatialVolumeBinding> binding;

			inline Volume& volume() { return binding->volume; }
		};

		struct DynamicVolume {

			SoftPtr<SpatialVolumeBinding> binding;
			NodePtrs nodes;

			inline Volume& volume() { return binding->volume; }

			void clear(SpatialDynamicListener* pListener, void* pListenerContext);
			void appendToQuery(SpatialObjectGroup_KDTree& query);
		};

		class Cell {
		public:

			typedef WETL::CountedPtrArrayEx<StaticTriCont, Index, WETL::ResizeExact> StaticTriConts;
			typedef WETL::CountedArray<StaticVolume*, false, Index, WETL::ResizeExact> StaticVols;
			typedef WETL::CountedArray<DynamicVolume*, false, Index, WETL::ResizeExact> DynVols;

		public:		

			Cell();

			Index getStaticTriCount();
			Index updateStaticTriCount();

			void staticTriSplit(const AAPlane& plane, Cell& back, Cell& front, PlaneClassifyCount& splitCounts);
			void staticTriClassify(const AAPlane* planes, PlaneClassifyCount* splitCounts, int planeCount = 1);
			void staticTriAddToVol(AAB& vol);

		public:

			StaticTriConts mStaticTriConts;
			StaticVols mStaticVols;
			DynVols mDynVols;

			Index mStaticTriCount;
		};

		class TreeNodeLeaf : public Cell {
		public:

			AAB staticTriBoundingVolume;

			TreeNodeLeaf() : staticTriBoundingVolume(true) {}

			void cloneTo(TreeNodeLeaf& target, const bool& includeVolumes);

			void render(Renderer& renderer);
			void updateBoundingVolume();

			void add(StaticVolume* pVolume);
						
			void add(TreeNode* pOwner, SpatialDynamicListener* pListener, DynamicVolume* pObject, void* pListenerContext);
			void remove(TreeNode* pOwner, SpatialDynamicListener* pListener, DynamicVolume* pObject, void* pListenerContext);
			void onClear(DynamicVolume* pObject);

			void appendToQuery(SpatialObjectGroup_KDTree& query, DynamicVolume* pExcluded);
		};

		struct TreeNode {

			SoftPtr<TreeNode> parent;
			AAB volume;
						
			enum ChildEnum {

				Front = 0, Back
			};
			
			HardPtr<TreeNodeBranch> branch;
			HardPtr<TreeNodeLeaf> leaf;

			void split(const SubdivisionSettings& settings, const Index& totalCount, Index depth, bool recurse, Index sharedElementCount = 0, float volumeScale = 1000.0f);
			bool findSplittingPlane(const SubdivisionSettings& settings, const Index& totalCount, AAPlane& plane, float volumeScale);
			bool optimizeSplittingPlane(const SubdivisionSettings& settings, const Index& totalCount,
											const int& dim, const Vector3& normal, const Span& searchSpan, 
											AAPlane& plane, PlaneClassifyCount& count, float& outSplitCoeffBack, 
											bool allowSmallResidualCells, 
											const float& volumeScale, const float& volumeSplitCoeffBack, float* pGranulatity = NULL);

			void render(Renderer& renderer);
			
			bool englobes(const VolumeStruct& volume);

			void recurseAdd(StaticVolume* pVolume);
			void recurseAdd(SpatialDynamicListener* pListener, DynamicVolume* pObject, const VolumeStruct& volume, void* pListenerContext);
		};

		struct TreeNodeBranch {

			AAPlane plane;
			TreeNode child[2];
		};

		static const float VolumeScale;

		typedef WETL::CountedPtrArrayEx<StaticVolume, Index, WETL::ResizeExact> StaticVolManager;
		//typedef WETL::Array<SpatialTriContainer*, false, Index>  SpatialTriObjects;
		//typedef WETL::Array<SpatialVolume*, false, Index>  SpatialVolumes;
	};

}

#endif