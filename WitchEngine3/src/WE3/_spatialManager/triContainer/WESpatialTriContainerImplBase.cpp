#include "WESpatialTriContainerImplBase.h"

namespace WE {

bool SpatialTriContainerImplBase::isCompatibleWith(const ElementType& type) {

	return (type == mType_Tri1);
}

void SpatialTriContainerImplBase::ElementAddressBatch::destroy() {

	mElementCount = 0;
	mSpans.destroy();
}

void SpatialTriContainerImplBase::ElementAddressBatch::clear() {

	mElementCount = 0;
	mSpans.count = 0;
}

void SpatialTriContainerImplBase::ElementAddressBatch::cloneTo(ElementAddressBatch& target) const {

	AbstractContainer::ElementAddressBatch::cloneTo(target);

	target.mElementCount = mElementCount;
	target.mSpans.reserve(mSpans.count);

	for (Spans::Index i = 0; i < mSpans.count; ++i) {

		target.mSpans.el[i] = mSpans.el[i];
	}
}

void SpatialTriContainerImplBase::ElementAddressBatch::add(const AbstractContainer::ElementAddress& _address, AbstractContainer::AddressBatchContext* pCtx) {

	ElementAddress& address = (ElementAddress&) _address;

	++mElementCount;

	if (mSpans.count) {

		IndexSpanEx& span = mSpans.el[mSpans.count - 1];

		if (span.bufferIndex == address.bufferIndex) {

			if (span.max + 1 == address.elIndex) {

				++span.max;
				return;
			}

			if (span.min == address.elIndex + 1) {

				--span.min;
				return;
			}
		}
		
	} 
	
	IndexSpanEx& span = mSpans.addOne();

	span.bufferIndex = address.bufferIndex;
	span.min = address.elIndex;
	span.max = address.elIndex;
}

/*
void SpatialTriContainerImplBase::ElementAddressBatch::initWith(AbstractContainer& _container) {

	SpatialTriContainerImplBase& container = (SpatialTriContainerImplBase&) _container;

	destroy();

	mElementCount = container.getElementCount();
	mSpans.reserve(container.mTriBuffers.size);

	for (Index bufferIndex = 0; bufferIndex < container.mTriBuffers.size; ++bufferIndex) {

		TriBuffer& buff = dref(container.mTriBuffers.el[bufferIndex]);
		
		if (buff.getCount()) {

			IndexSpanEx& span = mSpans.addOne();

			span.bufferIndex = bufferIndex;
			span.min = 0;
			span.max =  buff.getCount() - 1;
		}
	}
}
*/

SpatialTriContainerImplBase::SpatialTriContainerImplBase(Object* pOwner) 
	: SpatialTriContainer(pOwner) {
}

/*
AbstractContainer::ElementAddressBatch* SpatialTriContainerImplBase::createElementAddressBatch() {

	SoftPtr<ElementAddressBatch> batch;

	WE_MMemNew(batch.ptrRef(), ElementAddressBatch());

	return batch.ptr();
}

SpatialTriContainerImplBase::Index SpatialTriContainerImplBase::getElementCount() {

	if (mElementCount == 0 && mTriBuffers.size) {
		
		mElementCount = 0;

		for (Index bufferIndex = 0; bufferIndex < mTriBuffers.size; ++bufferIndex) {

			TriBuffer& buff = dref(mTriBuffers.el[bufferIndex]);
			mElementCount += buff.getCount();
		}
	}

	return mElementCount;
}
*/

}