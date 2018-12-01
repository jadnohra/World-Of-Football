#ifndef _WESpatialTriContainerImplT_TImpl_h
#define _WESpatialTriContainerImplT_TImpl_h

namespace WE {


template<typename TriBufferT>
void SpatialTriContainerImplT<TriBufferT>::ElementAddressBatch::initWith(AbstractContainer& _container) {

	SpatialTriContainerImplT& container = (SpatialTriContainerImplT&) _container;

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

template<typename TriBufferT>
SpatialTriContainerImplT<TriBufferT>::SpatialTriContainerImplT(Object* pOwner) 
	: SpatialTriContainerImplBase(pOwner), mElementCount(0) {
}

template<typename TriBufferT>
AbstractContainer::ElementAddressBatch* SpatialTriContainerImplT<TriBufferT>::createElementAddressBatch() {

	SoftPtr<ElementAddressBatch> batch;

	WE_MMemNew(batch.ptrRef(), ElementAddressBatch());

	return batch.ptr();
}

template<typename TriBufferT>
AbstractContainer::Index SpatialTriContainerImplT<TriBufferT>::getElementCount() {

	if (mElementCount == 0 && mTriBuffers.size) {
		
		mElementCount = 0;

		for (Index bufferIndex = 0; bufferIndex < mTriBuffers.size; ++bufferIndex) {

			TriBuffer& buff = dref(mTriBuffers.el[bufferIndex]);
			mElementCount += buff.getCount();
		}
	}

	return mElementCount;
}

template<typename TriBufferT>
void SpatialTriContainerImplT<TriBufferT>::process(ElementProcessorInfo& processor) {

#ifdef _DEBUG

	assrt(isCompatibleWith(processor.argType));

#endif

	Index processingIndex = 0;

	if (processor.functionSignature == FS_1) {

		fct_process_1 fct = (fct_process_1) processor.function;

		ElementAddress address;
		Tri1 tempTri;

		for (address.bufferIndex = 0; address.bufferIndex < mTriBuffers.size; ++address.bufferIndex) {

			TriBuffer& buff = dref(mTriBuffers.el[address.bufferIndex]);
			Index count = buff.getCount();

			for (address.elIndex = 0; address.elIndex < count; ++address.elIndex) {

				(*fct) (processingIndex, &buff.get(address.elIndex, tempTri), processor.context, &address, NULL);
				++processingIndex;
			}
		}

	} else {

		assrt(false);
	}
}

template<typename TriBufferT>
void SpatialTriContainerImplT<TriBufferT>::process(ElementProcessorInfo& processor, const AbstractContainer::ElementAddress& _address) {

	ElementAddress& address = (ElementAddress&) _address;

	fct_process_1 fct = (fct_process_1) processor.function;

	Tri1 tempTri;

	Index processingIndex = 0;

	TriBuffer& buff = dref(mTriBuffers.el[address.bufferIndex]);
	(*fct) (processingIndex, &buff.get(address.elIndex, tempTri), processor.context, &address, NULL);
}

template<typename TriBufferT>
void SpatialTriContainerImplT<TriBufferT>::process(ElementProcessorInfo& processor, const AbstractContainer::ElementAddressBatch& _addressBatch) {

	ElementAddressBatch& addressBatch = (ElementAddressBatch&) _addressBatch;

	fct_process_1 fct = (fct_process_1) processor.function;

	ElementAddress address;
	Tri1 tempTri;

	Index processingIndex = 0;

	for (Index si = 0; si < addressBatch.mSpans.count; ++si) {

		IndexSpanEx& span = addressBatch.mSpans.el[si];

		address.bufferIndex = span.bufferIndex;
		TriBuffer& buff = dref(mTriBuffers.el[address.bufferIndex]);

		for (address.elIndex = span.min; address.elIndex <= span.max; ++address.elIndex) {

			(*fct) (processingIndex, &buff.get(address.elIndex, tempTri), processor.context, &address, NULL);
			++processingIndex;
		}
	}
}


template<typename TriBufferT>
void SpatialTriContainerImplT<TriBufferT>::render(Renderer& renderer, bool renderMain, const RenderColor* pColor, bool renderDetails, const RenderColor* pDetailColor) {

	if (renderDetails) {

		renderer.setTransform(Matrix4x3Base::kIdentity, RT_World);

		for (Index i = 0; i < mTriBuffers.size; ++i) {

			mTriBuffers.el[i]->render(renderer, pDetailColor);
		}
	}
}

template<typename TriBufferT>
void SpatialTriContainerImplT<TriBufferT>::simplify(const Simplification* pSettings) {

	for (Index i = 0; i < mTriBuffers.size; ++i) {

		mTriBuffers.el[i]->simplify(pSettings);
	}
}

}

#endif