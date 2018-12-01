#ifndef _WESpatialTriContainerImplT_h
#define _WESpatialTriContainerImplT_h

#include "WESpatialTriContainerImplBase.h"
#include "WESpatialTriBufferImpl.h"

namespace WE {

	template<typename TriBufferT>
	class SpatialTriContainerImplT : public SpatialTriContainerImplBase {
	public:

		typedef TriBufferT TriBuffer;

	protected:

		class ElementAddressBatch : public SpatialTriContainerImplBase::ElementAddressBatch {
		public:

			virtual void initWith(AbstractContainer& container);
		};

		
	public:

		SpatialTriContainerImplT(Object* pOwner);

		virtual AbstractContainer::ElementAddressBatch* createElementAddressBatch();

		virtual Index getElementCount();

		virtual void process(ElementProcessorInfo& processor);
		virtual void process(ElementProcessorInfo& processor, const AbstractContainer::ElementAddress& address);
		virtual void process(ElementProcessorInfo& processor, const AbstractContainer::ElementAddressBatch& address);

		virtual void render(Renderer& renderer, bool renderMain = true, const RenderColor* pColor = NULL, bool renderDetails = false, const RenderColor* pDetailColor = NULL);
		virtual void simplify(const Simplification* pSettings);

	protected:

		friend class ElementAddressBatch;

	protected:

		typedef WETL::PtrArrayEx<TriBuffer, Index> TriBuffers;
		
		TriBuffers mTriBuffers;
		Index mElementCount;

	protected:

		inline void buffersDestroy() { mTriBuffers.destroy(); }

		inline void buffersReserve(const Index& count) { }
		inline TriBuffer& buffersAddOne() { return dref(mTriBuffers.addNewOne()); }

		inline const Index& buffersGetCount() { return mTriBuffers.size; }
		inline TriBuffer& buffersAt(const Index& i) { return dref(mTriBuffers.el[i]); }
	};

}

#include "WESpatialTriContainerImplT_TImpl.h"

namespace WE {

	typedef SpatialTriContainerImplT<SpatialTriBufferImpl> SpatialTriContainerImpl;
}

#endif