#ifndef _WESpatialManager_KDTree_h
#define _WESpatialManager_KDTree_h

#include "../WESpatialManager.h"
#include "../../math/WEAAB.h"
#include "../../math/WEPlane.h"
#include "../WESpatialProcessor.h"
#include "../WESpatialManagerCellImpl.h"

namespace WE {

	class SpatialManager_KDTreeBase : public SpatialManagerTypeDefs {
	public:
		
		struct TreeNode;
		struct TreeNodeBranch;

		typedef WETL::CountedArray<TreeNode*, false, Index, WETL::ResizeExact> NodePtrs;
		
		struct DynamicVolumeData {

			NodePtrs nodes;

			void clear(SpatialEventListener* pListener, SpatialDynamicVolume& object, void* pListenerContext);
		};

		class TreeNodeLeaf : public  SpatialManagerCellImpl {
		public:

			AAB boundingVolume;

			void cloneTo(TreeNodeLeaf& target, const bool& includeVolumes);

			void render(Renderer& renderer, 
						bool renderContainers, const RenderColor* pContainersColor, 
						bool renderDetails, const RenderColor* pDetailColor);
			void updateBoundingVolume();

			void add(SpatialVolume& volume);
						
			void add(TreeNode* pOwner, SpatialEventListener* pListener, SpatialDynamicVolume& object, DynamicVolumeData& data, void* pListenerContext);
			void remove(TreeNode* pOwner, SpatialEventListener* pListener, SpatialDynamicVolume& object, DynamicVolumeData& data, void* pListenerContext);
			void onClear(SpatialDynamicVolume& object);
		};

		struct TreeNode {

			SoftPtr<TreeNode> parent;
			AAB volume;
						
			enum ChildEnum {

				Front = 0, Back
			};
			
			HardPtr<TreeNodeBranch> branch;
			HardPtr<TreeNodeLeaf> leaf;

			void split(const SpatialManagerBuilder::SubdivisionSettings& settings, const Index& totalCount, Index depth, bool recurse, Index sharedElementCount = 0, float volumeScale = 1000.0f);
			bool findSplittingPlane(const SpatialManagerBuilder::SubdivisionSettings& settings, const Index& totalCount, AAPlane& plane, float volumeScale);
			bool optimizeSplittingPlane(const SpatialManagerBuilder::SubdivisionSettings& settings, const Index& totalCount,
											const int& dim, const Vector3& normal, const Span& searchSpan, 
											AAPlane& plane, SpatialProcessor::PlaneTriClassifyCount::Count& count, float& outSplitCoeffBack, 
											bool allowSmallResidualCells, 
											const float& volumeScale, const float& volumeSplitCoeffBack, float* pGranulatity = NULL);

			void render(Renderer& renderer, bool renderMain, const RenderColor* pMainColor, 
								bool renderContainers, const RenderColor* pContainersColor, 
								bool renderDetails, const RenderColor* pDetailColor, bool recurse);
			
			bool englobes(const Volume& volume);

			void recurseAdd(SpatialVolume& volume);
			void recurseAdd(SpatialEventListener* pListener, SpatialDynamicVolume& object, DynamicVolumeData& data, const Volume& volume, void* pListenerContext);
		};

		struct TreeNodeBranch {

			AAPlane plane;
			TreeNode child[2];
		};

		typedef WETL::Array<SpatialTriContainer*, false, Index>  SpatialTriObjects;
		typedef WETL::Array<SpatialVolume*, false, Index>  SpatialVolumes;
	};

	class SpatialManager_KDTree : public SpatialManager, public SpatialManager_KDTreeBase {
	public:

		virtual SpatialManagerBuilder* createBuilder();

		virtual void destroy();
		
		virtual bool add(SpatialDynamicVolume& object);
		virtual bool update(SpatialDynamicVolume& object);
		virtual bool remove(SpatialDynamicVolume& object);

		virtual void render(Renderer& renderer, 
								bool renderMain = true, const RenderColor* pMainColor = NULL, 
								bool renderContainers = true, const RenderColor* pContainersColor = NULL, 
								bool renderDetails = false, const RenderColor* pDetailColor = NULL);


		virtual bool add(SpatialTriContainer& object) { return true; }
		virtual bool add(SpatialVolume& object) { return true; }

		virtual Index getCellCount(SpatialDynamicVolume& object);
		virtual SpatialManagerCell& getCell(SpatialDynamicVolume& object, const Index& cellIndex);

	protected:

		friend class SpatialManagerBuilder_KDTree;

		HardPtr<SpatialTriObjects> mTriObjects;
		HardPtr<SpatialVolumes> mVolumes;

		HardPtr<TreeNode> mTreeRoot;
	};

	class SpatialManagerBuilder_KDTree : public SpatialManagerBuilder, public SpatialManager_KDTreeBase {
	public:

		SpatialManagerBuilder_KDTree(SpatialManager* pCreator);

		virtual bool process(const SubdivisionSettings& settings);
		virtual bool build(SpatialManager& targetManager);

		virtual void clear();

		virtual bool add(SpatialTriContainer& object);
		virtual bool add(SpatialVolume& object);

		virtual bool isCacheValid();
		virtual bool loadFromCache();
		virtual bool saveToCache();

		virtual void render(Renderer& renderer, 
								bool renderMain = true, const RenderColor* pMainColor = NULL, 
								bool renderContainers = true, const RenderColor* pContainersColor = NULL, 
								bool renderDetails = false, const RenderColor* pDetailColor = NULL);

	protected:

		HardPtr<SpatialTriObjects> mTriObjects;
		HardPtr<SpatialVolumes> mVolumes;

		HardPtr<TreeNode> mTreeRoot;
	};

}

#endif