#ifndef _WETLSpanArray_h
#define _WETLSpanArray_h

namespace WETL {

	template<typename IndexT>
	struct IndexSpan {

		typedef IndexT Index;

		union {

			struct {
				Index min;
				Index max;
			};

			struct {
				Index from;
				Index to;
			};
		};
	};


	template<typename IndexT, typename SpanT>
	class IndexSpanBatch {
	public:

		template<typename IndexSpanBatchT>
		void cloneTo(IndexSpanBatchT& target) const {

			//target.mElementCount = mElementCount;
			target.mSpans.reserve(mSpans.count);

			for (Spans::Index i = 0; i < mSpans.count; ++i) {

				target.mSpans.el[i] = mSpans.el[i];
			}
		}

		void destroy(){

			//mElementCount = 0;
			mSpans.destroy();
		}

		void clear(){

			//mElementCount = 0;
			mSpans.count = 0;
		}

		void add(const IndexT& index) {

			//++mElementCount;

			if (mSpans.count) {

				SpanT& span = mSpans.el[mSpans.count - 1];

				if (span.max + 1 == index) {

					++span.max;
					return;
				}

				if (span.min == index + 1) {

					--span.min;
					return;
				}
			} 
			
			SpanT& span = mSpans.addOne();

			span.min = index;
			span.max = index;
		}

		inline const IndexT& getBatchCount() { return mSpans.count; }
		inline const IndexT& getSpanCount() { return mSpans.count; }
		inline const IndexT& getCount() { return mSpans.count; }

		inline const SpanT& get(const IndexT& i) { return mSpans[i]; }

	public:

		typedef WETL::CountedArray<SpanT, false, IndexT, WETL::ResizeExact> Spans;

		Spans mSpans;
	};

}

#endif