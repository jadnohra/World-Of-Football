#ifndef _WESpatialProcessor_h
#define _WESpatialProcessor_h

#include "WESpatialProcessorFactory.h"

namespace WE {

	class SpatialProcessor : public SpatialProcessorFactory {
	public:

		static void addToVol_Tri1(AAB& volume, SpatialAbstractContainer& container);
		static void classify_Tri1(PlaneTriClassifyCount& planeTriClassify, SpatialAbstractContainer& container);
		static void classify_Tri1(PlaneTriClassifyIndexes& planeTriClassify, SpatialAbstractContainer& container);
		static void split_Tri1(PlaneTriSplit& planeTriSplit, SpatialAbstractContainer& container);


		struct PtrArray {

			template<typename ContainerPtrArrayT, typename IdxT>
			static void addToVol_Tri1(AAB& volume, ContainerPtrArrayT& collection, IdxT count);

			template<typename ContainerPtrArrayT, typename IdxT>
			static void classify_Tri1(PlaneTriClassifyCount& planeTriClassify, ContainerPtrArrayT& collection, IdxT count);

			template<typename ContainerPtrArrayT, typename IdxT>
			static void classify_Tri1(PlaneTriClassifyIndexes& planeTriClassify, ContainerPtrArrayT& collection, IdxT count);
		};

		struct PartialPtrArray {

			template<typename PartialPtrArrayT, typename IdxT>
			static void addToVol_Tri1(AAB& volume, PartialPtrArrayT& collection, IdxT count);

			template<typename PartialPtrArrayT, typename IdxT>
			static void classify_Tri1(PlaneTriClassifyCount& planeTriClassify, PartialPtrArrayT& collection, IdxT count);

			template<typename PartialPtrArrayT, typename IdxT>
			static void classify_Tri1(PlaneTriClassifyIndexes& planeTriClassify, PartialPtrArrayT& collection, IdxT count);

			template<typename PartialPtrArrayT, typename IdxT>
			static void split_Tri1(PlaneTriSplit& planeTriSplit, PartialPtrArrayT& collection, IdxT count, PartialPtrArrayT& split_Back, PartialPtrArrayT& split_Front, PartialPtrArrayT* pSplit_Shared = NULL);

			template<typename PartialPtrArrayT, typename IdxT>
			static void render_Tri1(Render& render, PartialPtrArrayT& collection, IdxT count);
		};
	};
}

#include "WESpatialProcessor_TImpl.h"

#endif