#ifndef _WEVContainerT_ImplBaseT_h
#define _WEVContainerT_ImplBaseT_h

#include "WEVContainerT.h"
#include "../WETL/WETLArray.h"
#include "../WETL/WETLSizeAlloc.h"

namespace WE {

	template<typename IndexDataTypeT, typename ElementTypeDataTypeT>
	class VContainerT_ImplBaseT 
		: public VContainerT<IndexDataTypeT, ElementTypeDataTypeT> {

	protected:

		typedef VContainerT<IndexDataTypeT, ElementTypeDataTypeT> VContainer;

		class ElementAddress : public VContainer::ElementAddress {
		public:

			Index bufferIndex;
			Index elIndex;

			virtual void cloneTo(ElementAddress& target) const {

				VContainer::ElementAddress::cloneTo(target);

				target.bufferIndex = bufferIndex;
				target.elIndex = elIndex;
			}
		};

		struct IndexSpan {

			Index min;
			Index max;
		};

		struct IndexSpanEx : IndexSpan {

			Index bufferIndex;
		};

		class ElementAddressBatch : public VContainer::ElementAddressBatch {
		public:

			virtual void initWith(VContainer& container) = 0;

			virtual void cloneTo(ElementAddressBatch& target) const {

				VContainer::ElementAddressBatch::cloneTo(target);

				target.mElementCount = mElementCount;
				target.mSpans.reserve(mSpans.count);

				for (Spans::Index i = 0; i < mSpans.count; ++i) {

					target.mSpans.el[i] = mSpans.el[i];
				}
			}

			virtual void destroy(){

				mElementCount = 0;
				mSpans.destroy();
			}

			virtual void clear(){

				mElementCount = 0;
				mSpans.count = 0;
			}

			virtual void add(const typename VContainer::ElementAddress& _address, typename VContainer::AddressBatchContext* pCtx) {

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

			virtual Index getBatchCount() { return mSpans.size; }

		public:

			typedef WETL::CountedArray<IndexSpanEx, false, Index, WETL::ResizeExact> Spans;

			Spans mSpans;
		};

		
	public:

		VContainerT_ImplBaseT() {}

		virtual bool isCompatibleWith(const ElementType& type) = 0;

		virtual VContainer::ElementAddressBatch* createElementAddressBatch() = 0;

		virtual Index getElementCount() = 0;

		virtual void process(ElementProcessorInfo& processor) = 0;
		virtual void process(ElementProcessorInfo& processor, const VContainer::ElementAddress& address) = 0;
		virtual void process(ElementProcessorInfo& processor, const VContainer::ElementAddressBatch& address) = 0;
		
	protected:

		friend class ElementAddressBatch;
	};

}


#endif