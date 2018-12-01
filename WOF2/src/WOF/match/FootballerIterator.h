#ifndef h_WOF_match_FootballerIterator
#define h_WOF_match_FootballerIterator

#include "WE3/WETL/WETLArray.h"
#include "WE3/WEPtr.h"
using namespace WE;

#include "DataTypes.h"
#include "Clock.h"

namespace WOF { namespace match {

	class Footballer;

	struct SoftFootballerArray : WETL::CountedArray<Footballer*, false, FootballerIndex, WETL::ResizeExact> {

		inline void reset() { count = 0; }
	};

	class FootballerIterator {
	public:

		enum Filter {

			F_None = 0, F_AI, F_Player
		};

	public:

		virtual FootballerIndex getFootballerCount(Filter filter = F_None) = 0;
		virtual bool iterateFootballers(Footballer*& pInOutFootballer, FootballerIndex& iter, Filter filter = F_None) = 0;

		bool iterateLoopFootballers(Footballer*& pInOutFootballer, FootballerIndex& iter, Filter filter = F_None);

		//slow!!! use sparingly
		Footballer* getFootballerAt(const FootballerIndex& index, Filter filter = F_None);
	};


	struct FootballerMultiFrameProcessingSetup {

		enum Mode {

			M_None = 0, M_SingleFrame, M_MultiFrame
		};

		Mode mode;

		float referenceFPS;
		TickCount referenceFrameCount;

		bool exactIterations;
		TickCount frameCount;

		void init(const Clock& timing, float refFPS, TickCount refFrameCount, bool exactIters);
	};

	struct FootballerMultiFrameProcessingState {

		TickCount frameIndex;
		FootballerIndex allFootballerCount;
		FootballerIndex footballerIndex;
		FootballerIndex footballersPerFrame;

		FootballerMultiFrameProcessingState();

		void reset(const FootballerMultiFrameProcessingSetup& setup, const FootballerIndex& footballerCount);
		void startFrame(const FootballerMultiFrameProcessingSetup& setup, FootballerIndex& startFootballerIndex, FootballerIndex& footballerCount);
		void endFrame(const FootballerMultiFrameProcessingSetup& setup, const FootballerIndex& startFootballerIndex, const FootballerIndex& footballerCount);
		void cancelFrame(const FootballerMultiFrameProcessingSetup& setup, const FootballerIndex& startFootballerIndex, const FootballerIndex& footballerCount);
	};


	struct FootballerMultiFrameIter {

		FootballerIndex startIndex;
		FootballerIndex count;
		SoftPtr<Footballer> footballer;
		FootballerIndex iteratorArg;
		FootballerIndex iterIndex;
		bool isStarted;

		FootballerMultiFrameIter();

		bool startFrame(const FootballerMultiFrameProcessingSetup& setup, FootballerMultiFrameProcessingState& state, FootballerIterator& footballerIter);
		bool next(FootballerIterator& footballerIter, Footballer*& pFootballer);
		void endFrame(const FootballerMultiFrameProcessingSetup& setup, FootballerMultiFrameProcessingState& state);
		void cancelFrame(const FootballerMultiFrameProcessingSetup& setup, FootballerMultiFrameProcessingState& state);
	};
	
	struct FootballerMultiFrameIterOneShot : FootballerMultiFrameIter {

		const FootballerMultiFrameProcessingSetup* pSetup;
		SoftPtr<FootballerMultiFrameProcessingState> state;
		SoftPtr<FootballerIterator> footballerIter;

		FootballerMultiFrameIterOneShot(const FootballerMultiFrameProcessingSetup& setup, FootballerMultiFrameProcessingState& state, FootballerIterator& footballerIter);
		~FootballerMultiFrameIterOneShot();

		bool startFrame();
		bool next(Footballer*& pFootballer);
		void _endFrame(); //will be called on destructor automatically
		void cancelFrame();
	};


	class FootballerIteratorClientBase {
	public:

		FootballerIteratorClientBase() : mFootballerIteratorIsDirty(true) {}

		void resetFootballerIterator(FootballerIterator* pIterator) {

			mFootballerIterator = pIterator;
			mFootballerIteratorIsDirty = true;
		}
	
	protected:

		bool mFootballerIteratorIsDirty;
		SoftPtr<FootballerIterator> mFootballerIterator;
	};

} }


#endif