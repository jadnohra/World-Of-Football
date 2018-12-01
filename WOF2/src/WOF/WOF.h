#ifndef _WOF_h
#define _WOF_h

#include "WE3/WitchEngine.h"
#include "WE3/WETime.h"
#include "WE3/WEPtr.h"
#include "WE3/binding/directx/9/d3d9/renderer/WERendererD3D9.h"
#include "WE3/device/video/WEVideoDeviceSelector.h"
#include "WE3/native/WENativeAppInfo.h"
#include "WE3/WELog.h"
#include "WE3/binding/directx/9/dxut9/includeDXUT9.h"
#include "WE3/audio/WEAudioEngine.h"
#include "WE3/render/WERenderSprite.h"
using namespace WE;

#include "script/ScriptEngine.h"
#include "WOFConsole.h"
#include "WOF_Debugger.h"

namespace WOF {

	namespace match {

		class Match;
	}

	struct FramePerformance {

		float frameMoveInterval;
		float renderInterval;
		float presentInterval;

		int sleepCount;
		int sleepInterval;

		int lagCount;

		float frameMoveTime;
		float renderTime;

		FramePerformance() {

			frameMoveInterval = 0.0f;
			renderInterval = 0.0f;
			presentInterval = 0.0f;

			sleepCount = 0;
			sleepInterval = 0;

			lagCount = 0;

			frameMoveTime = 0.0f;
			renderTime = 0.0f;
		}
	};

	class Game : public VideoDeviceSelector, public WOFDebugSource {
	public:

		Game();
		~Game();

		void destroy();
	
	public:

		bool init(NativeAppInfo& nativeAppInfo, HWND hWnd, IDirect3DDevice9* pDevice, Log& log, Console& console, const TCHAR* pConfigPath = NULL);
		
		virtual bool fillDeviceSelection(VideoDeviceSelectionContext& selection, int currTryCount);

		void resetDevice(IDirect3DDevice9* pDevice);
		void onLostDevice();
				
		bool load(bool enableLoadTrack);
		bool isLoaded();
		
		bool showFPS();
		inline void setHasWOFDebugger(bool value) { mHasWOFDebugger = value; }
		inline const bool& hasWOFDebugger() { return mHasWOFDebugger; }

		bool processWndMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
		void frameMove(double fTime, float fElapsedTime, bool& isRenderFrame);
		void render(ID3DXSprite* pSprite, ID3DXFont* pFont, ID3DXFont* pBigFont);

		void processConsoleCommand(const TCHAR* command);

		Console& getConsole() { return mConsole; }
		inline ScriptEngine& getScriptEngine() { return mScriptEngine; }
		inline FramePerformance& framePerformance() { return mFramePerformance; } 
		
		const TCHAR* getVersion();

		void renderSplashScreen(bool enableLoadTrack);

	public:

		Time getTime();

		bool createMatch();
		void destroyMatch();

		inline match::Match* getMatch() { return mMatch; }
		inline Renderer* getRenderer() { return mRenderer; }
		inline AudioEngine* getAudioEngine() { return mAudioEngine; }

		const String& getScriptPath();
		const String& getConfigPath() { return mConfigPath; }
		const String& getAssetsPath() { return mAssetsPath; }
		const String& getLocalPath() { return mLocalPath; }

	public:

		virtual float debug_team_getTacticalInstantaneousBallOwnership(int team);
		virtual float debug_team_getTacticalBallOwnership(int team);
		virtual unsigned int debug_getBallInterceptFootballerCount();
		virtual void debug_getBallInterceptFootballer(unsigned int index, FootballerInfo& info, bool& doesInterceptBeforeBallIsResting);
		virtual unsigned int debug_getBallDistFootballerCount();
		virtual void debug_getBallDistFootballer(unsigned int index, FootballerInfo& info);

	public:

		static const TCHAR* ScriptClassName;

		static void declareInVM(SquirrelVM& target);

	protected:

		enum State {

			State_None = -1, State_Loading, State_Selecting, State_PlayingMatch
		};

	protected:

		FramePerformance mFramePerformance;

		//Input mInput;
		SoftPtr<NativeAppInfo> mNativeAppInfo;
		HardPtr<RendererD3D9> mRenderer;
		HardPtr<match::Match> mMatch;
		HardPtr<ScriptEngine> mScriptEngine;
		SoftPtr<Log> mLog;
		SoftPtr<Console> mConsole;
		HardPtr<RenderSprite> mSplashSprite;
		
		HardPtr<AudioEngine> mAudioEngine;
		HardPtr<AudioTrack> mLoadTrack;

		String mConfigPath;
		String mVersion;
		String mSplashPath;
		String mLoadTrackPath;
		String mAssetsPath;
		String mLocalPath;
		String mPluginsPath;

		float mLoadTrackVolume;
		bool mShowFPS;
		bool mHasWOFDebugger;

		State mState;

	public:

		bool mTweakEnableDebugKeys;
	};

	inline ScriptObject scriptArg(Game* pGame) { return ScriptObject(pGame); }
}

#endif