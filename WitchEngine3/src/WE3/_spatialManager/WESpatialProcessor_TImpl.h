#ifndef _WESpatialProcessor_TImpl_h
#define _WESpatialProcessor_TImpl_h

namespace WE {

template<typename ContainerPtrArrayT, typename IdxT>
void SpatialProcessor::PtrArray::addToVol_Tri1(AAB& volume, ContainerPtrArrayT& containerArray, IdxT count) {

	if (containerArray.size) {

		AbstractContainer::ElementProcessorInfo processor;

		create_addToVol_Tri1(volume, dref(containerArray.el[0]), processor);

		for (ContainerPtrArrayT::Index i = 0; i < count; ++i) {

			processor.argType = containerArray.el[i]->getType_Tri1();

			containerArray.el[i]->process(processor);
		}
	}
}

template<typename ContainerPtrArrayT, typename IdxT>
void SpatialProcessor::PtrArray::classify_Tri1(PlaneTriClassifyCount& planeTriClassify, ContainerPtrArrayT& containerArray, IdxT count) {

	if (containerArray.size) {

		AbstractContainer::ElementProcessorInfo processor;

		create_classify_Tri1(planeTriClassify, dref(containerArray.el[0]), processor);

		for (ContainerPtrArrayT::Index i = 0; i < count; ++i) {

			processor.argType = containerArray.el[i]->getType_Tri1();

			containerArray.el[i]->process(processor);
		}
	}
}

template<typename ContainerPtrArrayT, typename IdxT>
void SpatialProcessor::PtrArray::classify_Tri1(PlaneTriClassifyIndexes& planeTriClassify, ContainerPtrArrayT& containerArray, IdxT count) {

	if (containerArray.size) {

		AbstractContainer::ElementProcessorInfo processor;

		create_classify_Tri1(planeTriClassify, dref(containerArray.el[0]), processor);

		for (ContainerPtrArrayT::Index i = 0; i < count; ++i) {

			processor.argType = containerArray.el[i]->getType_Tri1();

			containerArray.el[i]->process(processor);
		}
	}
}

template<typename PartialPtrArrayT, typename IdxT>
void SpatialProcessor::PartialPtrArray::addToVol_Tri1(AAB& volume, PartialPtrArrayT& collection, IdxT count) {

	if (collection.size) {

		AbstractContainer::ElementProcessorInfo processor;

		create_addToVol_Tri1(volume, collection.el[0]->container, processor);

		for (PartialPtrArrayT::Index i = 0; i < count; ++i) {

			processor.argType = collection.el[i]->container->getType_Tri1();

			collection.el[i]->container->process(processor, collection.el[i]->batch);
		}
	}
}

template<typename PartialPtrArrayT, typename IdxT>
void SpatialProcessor::PartialPtrArray::classify_Tri1(PlaneTriClassifyCount& planeTriClassify, PartialPtrArrayT& collection, IdxT count) {

	if (collection.size) {

		AbstractContainer::ElementProcessorInfo processor;

		create_classify_Tri1(planeTriClassify, collection.el[0]->container, processor);

		for (PartialPtrArrayT::Index i = 0; i < count; ++i) {

			processor.argType = collection.el[i]->container->getType_Tri1();

			collection.el[i]->container->process(processor, collection.el[i]->batch);
		}
	}
}

template<typename PartialPtrArrayT, typename IdxT>
void SpatialProcessor::PartialPtrArray::classify_Tri1(PlaneTriClassifyIndexes& planeTriClassify, PartialPtrArrayT& collection, IdxT count) {

	if (collection.size) {

		AbstractContainer::ElementProcessorInfo processor;

		create_classify_Tri1(planeTriClassify, collection.el[0]->container, processor);

		for (PartialPtrArrayT::Index i = 0; i < count; ++i) {

			processor.argType = collection.el[i]->container->getType_Tri1();

			collection.el[i]->container->process(processor, collection.el[i]->batch);
		}
	}
}

template<typename PartialPtrArrayT, typename IdxT>
void SpatialProcessor::PartialPtrArray::render_Tri1(Render& render, PartialPtrArrayT& collection, IdxT count) {

	if (collection.size) {

		AbstractContainer::ElementProcessorInfo processor;

		create_render_Tri1(render, collection.el[0]->container, processor);

		for (PartialPtrArrayT::Index i = 0; i < count; ++i) {

			processor.argType = collection.el[i]->container->getType_Tri1();

			collection.el[i]->container->process(processor, collection.el[i]->batch);
		}
	}
}


template<typename PartialPtrArrayT, typename IdxT>
void SpatialProcessor::PartialPtrArray::split_Tri1(PlaneTriSplit& planeTriSplit, PartialPtrArrayT& collection, IdxT count, PartialPtrArrayT& split_Back, PartialPtrArrayT& split_Front, PartialPtrArrayT* pSplit_Shared) {

	if (collection.size) {

#ifdef _DEBUG
		if (!planeTriSplit.includeSharedInBothNodes && (pSplit_Shared == NULL)) {

			assrt(false);
			return;
		}
#endif

		AbstractContainer::ElementProcessorInfo processor;

		create_split_Tri1(planeTriSplit, collection.el[0]->container, processor);

		PartialPtrArrayT* pSplitTargets[3];

		pSplitTargets[PP_Back] = &split_Back;
		pSplitTargets[PP_Front] = &split_Front;
		pSplitTargets[PP_BackAndFront] = pSplit_Shared;
		int targetCount = planeTriSplit.includeSharedInBothNodes ? 2 : 3;

		for (PartialPtrArrayT::Index i = 0; i < count; ++i) {

			processor.argType = collection.el[i]->container->getType_Tri1();
			planeTriSplit.container = collection.el[i]->container;

			collection.el[i]->container->process(processor, collection.el[i]->batch);

			for (int ni = 0; ni < targetCount; ++ni) {

				if (planeTriSplit.nodes[ni].batch.isValid()) {

					pSplitTargets[ni]->addOne(collection.el[i]->container, planeTriSplit.nodes[ni].batch);

					//we transferred pointer ownership
					planeTriSplit.nodes[ni].batch.ptrRef() = NULL;
				}
			}
		}

		pSplitTargets[0]->updateElementCount();
		pSplitTargets[1]->updateElementCount();
		
		if (pSplitTargets[2])
			pSplitTargets[2]->updateElementCount();
	}
}

}

#endif