#ifndef _DebugBypass_h
#define _DebugBypass_h


namespace WE {

	struct DebugBypass {

		int currTri;
		int renderTri;

		DebugBypass() {

			currTri = 0;
			renderTri = 0;
		}
	};

	void setDebugBypass(DebugBypass* pBypass);
	DebugBypass* getDebugBypass();

	DebugBypass& getStaticDebugBypass();
}

#endif