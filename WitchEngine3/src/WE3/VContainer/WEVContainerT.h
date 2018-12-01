#ifndef _WEVContainerT_h
#define _WEVContainerT_h

#include "../WEDataTypes.h"

namespace WE {

	template<typename IndexDataTypeT, typename ElementTypeDataTypeT = UINT32>
	class VContainerT {
	public:

		typedef VContainerT<IndexDataTypeT, ElementTypeDataTypeT> ThisClass;

		typedef IndexDataTypeT Index;
		typedef ElementTypeDataTypeT ElementType;

		class ElementAddress {
		public:

			virtual ~ElementAddress() {}

			virtual void cloneTo(ElementAddress& target) const {}
		};

		class AddressBatchContext {
		};

		class ElementAddressBatch {
		public:

			ElementAddressBatch() : mElementCount(0) {}
			virtual ~ElementAddressBatch() {}

			virtual void initWith(ThisClass& container) = 0;
			virtual void cloneTo(ElementAddressBatch& target) const {}

			virtual void destroy() = 0;
			virtual void clear() = 0; //might keep memory allocated

			virtual void add(const ElementAddress& address, AddressBatchContext* pCtx) = 0;

			const Index& getElementCount() { return mElementCount; }

			virtual Index getBatchCount() = 0; //not element count, can be used to estimate batching efficiency, the smaller the better

		protected:

			Index mElementCount;
		};

		struct ElementProcessorInfo;

		enum FunctionSignature {

			FS_1 = 0
		};

		typedef void (*fct_process_1) (const Index& processingIndex, void* pArg, void* context, const ElementAddress* pAddress, AddressBatchContext* pCtx);
		//typedef void (*fct_process_2) (void* pElement, void* context);

		struct ElementProcessorInfo {
		
			FunctionSignature functionSignature;

			union {

				fct_process_1 function;
			};

			void* context;

			ElementType argType;

			ElementProcessorInfo() {};

			ElementProcessorInfo(fct_process_1 function, void* context, ElementType argType) {

				init(function, context, argType);
			};

			void init(fct_process_1 function, void* context, ElementType argType) {

				this->functionSignature = FS_1;
				this->function = function;
				this->context = context;

				this->argType = argType;
			};
		};

		virtual bool isCompatibleWith(const ElementType& type) = 0;

		virtual Index getElementCount() = 0;

		virtual ElementAddressBatch* createElementAddressBatch() = 0;

		virtual void process(ElementProcessorInfo& processor) = 0;
		virtual void process(ElementProcessorInfo& processor, const ElementAddress& address) = 0;
		virtual void process(ElementProcessorInfo& processor, const ElementAddressBatch& address) = 0;
	};
}

#endif