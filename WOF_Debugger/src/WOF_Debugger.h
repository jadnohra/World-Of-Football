#ifndef _WOF_DEbugger_h
#define _WOF_DEbugger_h

class WOFDebugSource;

extern "C" {

	void* newWOFDebugger(void);
	void deleteWOFDebugger(void* pDebugger);

	void setGame(void* pDebugger, WOFDebugSource* pGame);
	void start(void* pDebugger, HWND WOFWnd);
	void update(void* pDebugger);
}


typedef void* (*func_newWOFDebugger)();
typedef void (*func_deleteWOFDebugger)(void*);
typedef void (*func_WOFDebugger_setGame)(void*, WOFDebugSource* pGame);
typedef void (*func_WOFDebugger_start)(void*, HWND WOFWnd);
typedef void (*func_WOFDebugger_update)(void*);

#include "windows.h"

struct WOFDebugger {

	HMODULE mModule;

	func_newWOFDebugger mFunc_new;
	func_deleteWOFDebugger mFunc_delete;
	func_WOFDebugger_setGame mFunc_setGame;
	func_WOFDebugger_start mFunc_start;
	func_WOFDebugger_update mFunc_update;

	void* mpDebugger;

	WOFDebugger() : mModule(NULL), mFunc_new(NULL), mFunc_delete(NULL), mpDebugger(NULL) {
	}

	inline bool isValid() { return mpDebugger != NULL; }
	inline void start(HWND WOFWnd) { return mFunc_start(mpDebugger, WOFWnd); }
	inline void setGame(WOFDebugSource* pGame) { return mFunc_setGame(mpDebugger, pGame); }
	inline void update() { return mFunc_update(mpDebugger); }

	bool load(const TCHAR* path) {

		if (mModule == NULL) {

			mModule = LoadLibrary(path);

			if (mModule != NULL) {

				mFunc_new = (func_newWOFDebugger) GetProcAddress(mModule, "newWOFDebugger");

				if (mFunc_new == NULL) {

					FreeLibrary(mModule);
					mModule = NULL;
				}

				mFunc_delete = (func_deleteWOFDebugger) GetProcAddress(mModule, "deleteWOFDebugger");

				if (mFunc_delete == NULL) {

					FreeLibrary(mModule);
					mModule = NULL;
				}

				mFunc_setGame = (func_WOFDebugger_setGame) GetProcAddress(mModule, "setGame");

				if (mFunc_setGame == NULL) {

					FreeLibrary(mModule);
					mModule = NULL;
				}

				mFunc_start = (func_WOFDebugger_start) GetProcAddress(mModule, "start");

				if (mFunc_start == NULL) {

					FreeLibrary(mModule);
					mModule = NULL;
				}

				mFunc_update = (func_WOFDebugger_update) GetProcAddress(mModule, "update");

				if (mFunc_update == NULL) {

					FreeLibrary(mModule);
					mModule = NULL;
				}

				if (mModule) {

					mpDebugger = mFunc_new();
				}
			}
		}

		return mModule != NULL;
	}

	~WOFDebugger() {

		destroy();
	}

	void destroy() {

		if (mpDebugger) {

			mFunc_delete(mpDebugger);
			mpDebugger = NULL;
		}

		if (mModule) {

			FreeLibrary(mModule);
			mModule = NULL;
		}
	}
};

class WOFDebugSource {
public:

	struct FootballerInfo {

		int team;
		int number;
	};

	virtual float debug_team_getTacticalInstantaneousBallOwnership(int team) = 0;
	virtual float debug_team_getTacticalBallOwnership(int team) = 0;

	virtual unsigned int debug_getBallInterceptFootballerCount() = 0;
	virtual void debug_getBallInterceptFootballer(unsigned int index, FootballerInfo& info, bool& doesInterceptBeforeBallIsResting) = 0;

	virtual unsigned int debug_getBallDistFootballerCount() = 0;
	virtual void debug_getBallDistFootballer(unsigned int index, FootballerInfo& info) = 0;
};

#endif