#ifndef _Tracker_h_WE_Input
#define _Tracker_h_WE_Input

namespace WE { namespace Input {

	class Source;

	/*
		P.S: Tracker functions might be called from different I/O threads...
	*/
	class Tracker {
	public:

		virtual void onSourceChanged(Source* pVarImpl) = 0;
	};

} }

#endif