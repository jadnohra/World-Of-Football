#ifndef _WETLResize_h
#define _WETLResize_h


namespace WETL {

	struct ResizeDouble {

		static unsigned long resize(unsigned long curr, unsigned long minNew);
	};

	struct ResizeExact {

		static unsigned long resize(unsigned long curr, unsigned long minNew);
	};
}

#endif