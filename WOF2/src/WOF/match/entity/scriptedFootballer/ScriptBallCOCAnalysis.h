#ifndef _ScriptBallCOCAnalysis_h
#define _ScriptBallCOCAnalysis_h

#include "WE3/script/WEScriptEngine.h"
using namespace WE;


namespace WOF { namespace match {

	class ScriptBallCOCAnalysis {
	public:

		typedef unsigned int Index;

		struct Sample {
		};

	public:

		inline Index getPitchCollCount() { return mPitchColls.count; }
		inline Sample* getPitchColl(Index i) { return &(mPitchColls[i]); }

		inline Index getFootballerCollCount() { return mFootballerColls.count; }
		inline Sample* getFootballerColl() { return &(mFootballerColls[i]); }

	protected:

		typedef WETL::CountedArray<Sample, false, Index, WETL::ResizeExact> Samples;

		Samples mPitchColls;
		Samples mFootballerColls;
		Samples mFootControlTriggers;
		
		Sample mPrePassSweetSpot;
		Sample mLastInCOC;
		Sample mRestInCOC;

	public:

		static const TCHAR* ScriptClassName;

		static void declareInVM(SquirrelVM& target);
	};

} }

#endif