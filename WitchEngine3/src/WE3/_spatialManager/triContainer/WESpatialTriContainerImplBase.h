#ifndef _WESpatialTriContainerImplBase_h
#define _WESpatialTriContainerImplBase_h

#include "../WESpatialManagerObjects.h"
#include "../../WETL/WETLArray.h"
#include "../../WETL/WETLSizeAlloc.h"

namespace WE {

	class SpatialTriContainerImplBase : public SpatialTriContainer {
	protected:

		class ElementAddress : public AbstractContainer::ElementAddress {
		public:

			Index bufferIndex;
			Index elIndex;

			virtual void cloneTo(ElementAddress& target) const {

				AbstractContainer::ElementAddress::cloneTo(target);

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

		class ElementAddressBatch : public AbstractContainer::ElementAddressBatch {
		public:

			virtual void initWith(AbstractContainer& container) = 0;
			virtual void cloneTo(ElementAddressBatch& target) const;

			virtual void destroy();
			virtual void clear();

			virtual void add(const AbstractContainer::ElementAddress& address, AbstractContainer::AddressBatchContext* pCtx);

			virtual Index getBatchCount() { return mSpans.size; }

		public:

			typedef WETL::CountedArray<IndexSpanEx, false, Index, WETL::ResizeExact> Spans;

			Spans mSpans;
		};

		
	public:

		SpatialTriContainerImplBase(Object* pOwner);

		virtual bool isCompatibleWith(const ElementType& type);

		virtual AbstractContainer::ElementAddressBatch* createElementAddressBatch() = 0;

		virtual Index getElementCount() = 0;

		virtual void process(ElementProcessorInfo& processor) = 0;
		virtual void process(ElementProcessorInfo& processor, const AbstractContainer::ElementAddress& address) = 0;
		virtual void process(ElementProcessorInfo& processor, const AbstractContainer::ElementAddressBatch& address) = 0;
		
	protected:

		friend class ElementAddressBatch;
	};

}


#endif