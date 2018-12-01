#ifndef _WESpatialProcessorFactory_h
#define _WESpatialProcessorFactory_h

#include "WESpatialManagerObjects.h"

#include "../math/WEAAB.h"
#include "../math/WEPlane.h"

namespace WE {

	class SpatialProcessorFactory {
	public:

		typedef AbstractContainer::ElementProcessorInfo ElementProcessorInfo;

		struct PlaneTriClassifyCount {

			struct Count {

				AbstractContainer::Index count[3];  //indexed by PlanePartitionResult

				Count() {

					count[0] = 0;
					count[1] = 0;
					count[2] = 0;
				}

				bool equals(const Count& comp) const {

					return (count[0] == comp.count[0]) && (count[1] == comp.count[1]) && (count[2] == comp.count[2]);
				}
			};

			AAPlane* planes;
			int planeCount;
			Count* counts;
						

			PlaneTriClassifyCount(int _planeCount = 0, AAPlane* _planes = NULL, Count* _counts = NULL) {

				planes = _planes;
				planeCount = _planeCount;

				counts = _counts;
			}
		};

		struct PlaneTriClassifyIndexes {

			AAPlane* pPlane;

			PlaneTriClassifyCount::Count resultSummary; //filled by algortihm

			PlanePartitionResult* results;
			int stride;
						
			void* _currResults;

			PlaneTriClassifyIndexes(AAPlane* _pPlane, PlanePartitionResult* _results = NULL, int _stride = sizeof(PlanePartitionResult)) {

				pPlane = _pPlane;

				results = _results;
				stride = _stride;

				_currResults = results;
			}
		};

		struct PlaneTriSplit {

			SoftPtr<AbstractContainer> container;
			const AAPlane* pPlane;
			bool includeSharedInBothNodes; //in this case nodes[PP_BackAndFront] is not used
			PlaneTriClassifyCount::Count resultSummary; //filled by algortihm

			struct SplitNode {

				HardPtr<AbstractContainer::ElementAddressBatch> batch;
			};

			SplitNode nodes[3];

			PlaneTriSplit(const AAPlane* _pPlane, bool _includeSharedInBothNodes) {

				pPlane = _pPlane;
				includeSharedInBothNodes = 	_includeSharedInBothNodes;
			}
		};

		struct Render {

			SoftPtr<Renderer> renderer;
			const RenderColor* pColor;
			bool wireFrame;

			Render(Renderer* pRenderer, const RenderColor* _pColor, bool _wireFrame) {

				renderer = pRenderer;
				pColor = _pColor;
				wireFrame = _wireFrame;
			}
		};

	public:

		static ElementProcessorInfo* create_addToVol_Tri1(AAB& volume, const SpatialManagerTypeBase& typeBase, ElementProcessorInfo& result);
		static ElementProcessorInfo* create_classify_Tri1(PlaneTriClassifyCount& planeTriClassify, const SpatialManagerTypeBase& typeBase, ElementProcessorInfo& result);
		static ElementProcessorInfo* create_classify_Tri1(PlaneTriClassifyIndexes& planeTriClassify, const SpatialManagerTypeBase& typeBase, ElementProcessorInfo& result);
		static ElementProcessorInfo* create_split_Tri1(PlaneTriSplit& planeTriSplit, const SpatialManagerTypeBase& typeBase, ElementProcessorInfo& result);
		static ElementProcessorInfo* create_render_Tri1(Render& render, const SpatialManagerTypeBase& typeBase, ElementProcessorInfo& result);
	};
	
}

#endif