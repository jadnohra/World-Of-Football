#include "WESpatialProcessor.h"

namespace WE {

void SpatialProcessor::addToVol_Tri1(AAB& volume, SpatialAbstractContainer& container) {

	AbstractContainer::ElementProcessorInfo processor;

	SpatialProcessorFactory::create_addToVol_Tri1(volume, container, processor);

	container.process(processor);
}

void SpatialProcessor::classify_Tri1(SpatialProcessorFactory::PlaneTriClassifyCount& planeTriClassify, SpatialAbstractContainer& container) {

	AbstractContainer::ElementProcessorInfo processor;

	SpatialProcessorFactory::create_classify_Tri1(planeTriClassify, container, processor);

	container.process(processor);
}

void SpatialProcessor::classify_Tri1(SpatialProcessorFactory::PlaneTriClassifyIndexes& planeTriClassify, SpatialAbstractContainer& container) {

	AbstractContainer::ElementProcessorInfo processor;

	SpatialProcessorFactory::create_classify_Tri1(planeTriClassify, container, processor);

	container.process(processor);
}

void SpatialProcessor::split_Tri1(SpatialProcessorFactory::PlaneTriSplit& planeTriSplit, SpatialAbstractContainer& container) {

	AbstractContainer::ElementProcessorInfo processor;

	SpatialProcessorFactory::create_split_Tri1(planeTriSplit, container, processor);

	container.process(processor);
}

}

