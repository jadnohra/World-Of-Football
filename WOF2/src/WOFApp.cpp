#pragma warning( disable : 4995 4996 4018 4312 4311 4244 )

#include "WOFApp.h"
#include "../project/resource.h"
#include "WOF/WOF.h"

#include "WE3/binding/directx/9/WED3D9DeviceSelection.h"
#include "WE3/binding/directx/9/dxut9/includeDXUT9.h"
#include "WE3/native/WENativeAppInfo.h"
#include "WE3/WitchEngine.h"
#include "WE3/WEAppPath.h"
#include "WE3/WEPtr.h"
#include "WE3/WEMem.h"
#include "WE3/WELog.h"
#include "WE3/data/WEDataSourceCreator.h"
#include "WE3/string/WETCHAR.h"

#include "WOF_Debugger.h"


//#include "includeDXUT.h"

#include "autoVersionBuildNumber.h"

#define AppWindowName L"World Of Football - Kimera Studios"
#define AppIconResource IDI_ICON_WOF
int AppWidth = 1024;
int AppHeight = 768;
bool AppWindowed = true;
bool AppVSync = false;
bool AppAntiAlias = false;
int AppAntiAliasSamples = 2;
int AppMaxColorDepth = 0;
bool AppShowLoadTime = true;
bool AppMultiThreadLoad = true;

HardPtr<WOF::Game> gGame;
WE::Log logger;

bool gEnableWOFDebugger = false;
String mWOFDebuggerDLLPath(L"WOF_Debugger.dll");
WOFDebugger mWOFDebugger;

bool gIsRenderFrame = false;

WE::NativeAppInfo gAppInfo;
VideoDeviceSelectionContext gVideoDeviceSelection;

SoftRef<ID3DXFont> g_pFont;
SoftRef<ID3DXFont> g_pBigFont;
SoftRef<ID3DXSprite> g_pTextSprite;

SoftRef<WE::RenderTexture> gSplashTexture;

/**************
	UI START
***************/

CDXUTDialogResourceManager g_DXUTResManager;
CDXUTDialog g_UI;
CDXUTDialog g_UIBack;

CDXUTListBox* pUIConsoleListBox = NULL;
CDXUTEditBox* pUIConsoleInput = NULL;

bool OnStartShowUI = false;
int ConsoleItemHistory = 32;

bool g_UIPassThroughInput = false;

int UIWidth = 100;
int UIHeight = 100;

int ConsoleWidth = 100;
int ConsoleHeight = 100;
int ConsoleOffset = 0;

bool varGet_ShowUI() {

	return g_UI.GetVisible();
}

void varSet_ShowUI(bool val) {

	g_UIBack.SetVisible(val);
	g_UI.SetVisible(val);
}

void varSet_UIPassThroughInput(bool pass) {

	g_UIPassThroughInput = pass;
}

bool varGet_UIPassThroughInput() {

	return g_UIPassThroughInput;
}

void ui_OnCreateDevice(IDirect3DDevice9* pd3dDevice) {

	g_DXUTResManager.OnCreateDevice( pd3dDevice );
}

void ui_InitSize() {

	UIWidth = AppWidth - (AppWidth / 25);
	UIHeight = 200;

	ConsoleWidth = UIWidth - (2 * ConsoleOffset);
	ConsoleHeight = 150;
}

void ui_OnResetDevice() {

	g_DXUTResManager.OnResetDevice();

	g_UI.SetLocation( (AppWidth / 25) / 2 , 5 );
    g_UI.SetSize( UIWidth, UIHeight );

	POINT pt;
	g_UI.GetLocation(pt);
	g_UIBack.SetLocation( pt.x , pt.y );
	pt.x = g_UI.GetWidth(); pt.y = g_UI.GetHeight();
    g_UIBack.SetSize( pt.x + 1, pt.y + 2 );
}

void ui_OnLostDevice() {

	g_DXUTResManager.OnLostDevice();
}

void ui_OnDestroyDevice() {

	g_DXUTResManager.OnDestroyDevice();
}

void ui_ConsoleAddText(const WCHAR *wszText) {

	if (pUIConsoleListBox) {

		if (wszText)
			pUIConsoleListBox->AddItem(wszText, NULL);
		else
			pUIConsoleListBox->AddItem(L"", NULL);

		int itemCount = pUIConsoleListBox->GetItemCount();

		while (itemCount >= ConsoleItemHistory) {

			pUIConsoleListBox->RemoveItem(0);

			itemCount = pUIConsoleListBox->GetItemCount();
		}

		if (itemCount)
			pUIConsoleListBox->SelectItem(itemCount - 1);
	}
}

void CALLBACK ui_OnGUIEvent( UINT nEvent, int nControlID, CDXUTControl* pControl, void* pUserContext )
{

	if (pControl == pUIConsoleInput) {

		switch(nEvent) {

			case EVENT_EDITBOX_STRING: {

				ui_ConsoleAddText(pUIConsoleInput->GetText());

				if (gGame.isValid()) {
					
					gGame->processConsoleCommand(pUIConsoleInput->GetText());
				};
			
				pUIConsoleInput->ClearText();
			}
			break;
		}
	}
	
}



void ui_InitApp() {

	g_UI.Init( &g_DXUTResManager );
	g_UIBack.Init( &g_DXUTResManager );

	int iY = 0; 
	int iY2 = 0; 
	int iX = 0;

	g_UIBack.SetBackgroundColors(D3DCOLOR_RGBA(0, 0, 0, 150));
	g_UI.SetBackgroundColors(D3DCOLOR_RGBA(100, 140, 205, 150), D3DCOLOR_RGBA(55, 105, 190, 150), 
									D3DCOLOR_RGBA(55, 105, 190, 150), D3DCOLOR_RGBA(10, 75, 180, 150));
	g_UI.SetCallback( ui_OnGUIEvent ); 

	iX = ConsoleOffset;
	{
		g_UI.AddListBox( 0, iX, iY, ConsoleWidth, ConsoleHeight, 0, &pUIConsoleListBox);
		iY += ConsoleHeight;

		if (pUIConsoleListBox) {

			pUIConsoleListBox->SetScrollBarWidth(0);
		}
	}

	{
		g_UI.AddEditBox(1, _T(""), iX, iY, ConsoleWidth, 32, true, &pUIConsoleInput);
	}


	if (OnStartShowUI) {

		varSet_ShowUI(true);

		if (pUIConsoleInput)
			g_UI.RequestFocus(pUIConsoleInput);
	} else {

		varSet_ShowUI(false);
	}
}


void ui_OnFrameRender(float fElapsedTime) {

	g_UIBack.OnRender( fElapsedTime );
	g_UI.OnRender( fElapsedTime );
}

bool ui_OnMsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, 
                          bool* pbNoFurtherProcessing, void* pUserContext ) {

	if (uMsg == WM_KEYDOWN) {

		switch( wParam ) {

			case 192: {

				if (varGet_UIPassThroughInput() && varGet_ShowUI()) {

					varSet_UIPassThroughInput(false);

				} else {

					varSet_UIPassThroughInput(false);

					varSet_ShowUI(!varGet_ShowUI());

					if (varGet_ShowUI() && pUIConsoleInput)
						g_UI.RequestFocus(pUIConsoleInput);
				}

				return true;
			}
			break;
		}
	}

	if (uMsg == WM_CHAR && wParam == 96) {

		return true;
	}

	*pbNoFurtherProcessing = g_DXUTResManager.MsgProc( hWnd, uMsg, wParam, lParam );
    if( *pbNoFurtherProcessing )
        return true;

	bool ret = false;

	if (varGet_ShowUI()) {

		if (pUIConsoleInput && pUIConsoleInput->m_bHasFocus) {

			if (uMsg == WM_KEYDOWN || uMsg == WM_KEYUP) {

				ret = true;
			}

			if (uMsg == WM_KEYDOWN) {

				switch( wParam ) {

					case VK_UP: {

						int index = pUIConsoleListBox->GetSelectedIndex();
						
						if (index)
							--index;

						if (index >= 0) {

							pUIConsoleListBox->SelectItem(index);

							DXUTListBoxItem* pItem = pUIConsoleListBox->GetSelectedItem(index);
							
							if (pItem)
								pUIConsoleInput->SetText(pItem->strText);
						}
					}
					break;

					case VK_DOWN: {

						int index = pUIConsoleListBox->GetSelectedIndex();
						
						++index;

						if (index <= pUIConsoleListBox->GetItemCount()) {

							if (index < pUIConsoleListBox->GetItemCount()) {

								pUIConsoleListBox->SelectItem(index);

								DXUTListBoxItem* pItem = pUIConsoleListBox->GetSelectedItem(index);
								
								if (pItem)
									pUIConsoleInput->SetText(pItem->strText);

							} else {

								pUIConsoleInput->SetText(L"");
							}
						}

					}
					break;
				}
			}
		
		}

		
		g_UI.MsgProc( hWnd, uMsg, wParam, lParam );
		
		return ret;
	}

	return false;
}


class UIConsole : public WE::Console {
public:

	UIConsole() : mLineNumber(0) {}

	virtual void print(const TCHAR* msg, bool printLineNumber) {

		++mLineNumber;

		if (printLineNumber) {

			mBufferString.assignEx(L"%u: ", mLineNumber);
			mBufferString.append(msg);

			ui_ConsoleAddText(mBufferString.c_tstr());

		} else {

			ui_ConsoleAddText(msg);
		}
	}

	virtual void show(bool show, bool readOnly) {

		varSet_ShowUI(show);
		varSet_UIPassThroughInput(readOnly);
	}

	virtual const TCHAR* getLastLine() {

		int index = pUIConsoleListBox->GetSelectedIndex();
						
		if (index >= 0) {

			DXUTListBoxItem* pItem = pUIConsoleListBox->GetSelectedItem(index);

			return pItem ? pItem->strText : NULL;
		}

		return NULL;
	}
	
protected:
	
	BufferString mBufferString;
	unsigned int mLineNumber;
};

UIConsole gConsole;

WE::Console& getConsole() {

	return gConsole;
}

/**************
	UI END
***************/

DWORD g_cursorLastAcionTime = 0;
DWORD g_cursorShowDelay = 3000;
bool gCursorShown = true;

void onCursorAction() {

	g_cursorLastAcionTime = timeGetTime();
}

void appUpdateCursor() {

	if (mWOFDebugger.isValid())
		return;

	if (timeGetTime() - g_cursorLastAcionTime >= g_cursorShowDelay) {

		if (gCursorShown) {

			gCursorShown = false;
			ShowCursor(FALSE);
		}

	} else {

		if (!gCursorShown) {

			gCursorShown = true;
			ShowCursor(TRUE);
		}
	}
}


//--------------------------------------------------------------------------------------
// Rejects any devices that aren't acceptable by returning false
//--------------------------------------------------------------------------------------
bool CALLBACK IsDeviceAcceptable( D3DCAPS9* pCaps, D3DFORMAT AdapterFormat, 
                                  D3DFORMAT BackBufferFormat, bool bWindowed, void* pUserContext )
{
    VideoDeviceSelectionContext* pContext = (VideoDeviceSelectionContext*) pUserContext;

	return D3D9DeviceSelection::isDeviceAcceptable(pCaps, AdapterFormat, BackBufferFormat, bWindowed, pContext);
}


//--------------------------------------------------------------------------------------
// Before a device is created, modify the device settings as needed
//--------------------------------------------------------------------------------------
bool CALLBACK ModifyDeviceSettings( DXUTDeviceSettings* pDeviceSettings, const D3DCAPS9* pCaps, void* pUserContext )
{
	VideoDeviceSelectionContext* pContext = (VideoDeviceSelectionContext*) pUserContext;

	pDeviceSettings->pp.PresentationInterval = AppVSync ? D3DPRESENT_INTERVAL_ONE : D3DPRESENT_INTERVAL_IMMEDIATE;

	if (pContext->tryAntiAlias && (pContext->antiAliasSamples >= 2) && (pContext->antiAliasSamples <= 16)) {

		D3DSWAPEFFECT save = pDeviceSettings->pp.SwapEffect;

		pDeviceSettings->pp.SwapEffect = D3DSWAPEFFECT_DISCARD;
		pDeviceSettings->pp.MultiSampleType = (D3DMULTISAMPLE_TYPE) pContext->antiAliasSamples;
		

		if (D3D9DeviceSelection::modifyDeviceSettings(pDeviceSettings, pCaps, pContext) == false) {

			log(L"Multi-Sampling Not Supported");

		} else {

			return true;
		}

		pDeviceSettings->pp.SwapEffect = save;
		pDeviceSettings->pp.MultiSampleType = D3DMULTISAMPLE_NONE;
	}

	if (D3D9DeviceSelection::modifyDeviceSettings(pDeviceSettings, pCaps, pContext) == false) {

		return false;
	}

    return true;
}

//--------------------------------------------------------------------------------------
// Load Thread Start
//--------------------------------------------------------------------------------------

bool gIsLoading = false;
bool gDoneLoading = false;
bool gLoadSuccess = false;

struct LoadThreadProcParam {
};

LoadThreadProcParam gLoadThreadParam;
HANDLE gLoadThreadHandle;
DWORD gLoadThreadId;

DWORD WINAPI loadThreadProc(LPVOID lpParameter) {

	LoadThreadProcParam* pParam = (LoadThreadProcParam*) lpParameter;

	while (!gDoneLoading) {
	
		DXUTWESetIsRenderFrame(true);
		DXUTRender3DEnvironment(false);

		Sleep(20);
	}

	return 0;
}

bool startLoading(const TCHAR* configPath) {
	
	if (!gGame->init(gAppInfo, DXUTGetHWND(), DXUTGetD3DDevice(), logger, gConsole, configPath)) {

		gIsLoading = false;
		gDoneLoading = true;
		gLoadSuccess = false;

		return false;
	}

	
	gIsLoading = true;
	gDoneLoading = false;
	gLoadThreadHandle = NULL;

	if (AppMultiThreadLoad) {

		gLoadThreadHandle = CreateThread(NULL, 0, loadThreadProc, &gLoadThreadParam, 0, &gLoadThreadId); 
	}

	if (gLoadThreadHandle == NULL) {

		DXUTWESetIsRenderFrame(true);
		DXUTRender3DEnvironment(false);
	}

	if (gGame->load(gLoadThreadHandle != NULL)) {

		gDoneLoading = true;
		gLoadSuccess = true;

	} else {

		gDoneLoading = true;
		gLoadSuccess = false;
	}

	if (gLoadThreadHandle != NULL) {

		CloseHandle((HANDLE) gLoadThreadHandle);
		gLoadThreadHandle = NULL;
	}
		
	gIsRenderFrame = false;
	DXUTWESetIsRenderFrame(false);

	gIsLoading = false;

	return gLoadSuccess;
}

//--------------------------------------------------------------------------------------
// Load Thread End
//--------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------
// Create any D3DPOOL_MANAGED resources here 
//--------------------------------------------------------------------------------------
HRESULT CALLBACK OnCreateDevice( IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext )
{
	
	ui_OnCreateDevice( pd3dDevice );

	g_pFont.destroy();
	D3DXCreateFont( pd3dDevice, 15, 0, FW_BOLD, 0, FALSE, DEFAULT_CHARSET, 
                         OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, 
                         L"Arial", g_pFont.pptr() );

	g_pBigFont.destroy();
	D3DXCreateFont( pd3dDevice, 36, 0, 0, 0, FALSE, DEFAULT_CHARSET, 
                         OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, 
                         L"Tahoma", g_pBigFont.pptr() );

	if (!g_pBigFont.isValid()) {

		D3DXCreateFont( pd3dDevice, 36, 0, 0, 0, FALSE, DEFAULT_CHARSET, 
                         OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, 
                         L"Arial", g_pBigFont.pptr() );
	}

	return S_OK;
}


//--------------------------------------------------------------------------------------
// Create any D3DPOOL_DEFAULT resources here 
//--------------------------------------------------------------------------------------
HRESULT CALLBACK OnResetDevice( IDirect3DDevice9* pd3dDevice, 
                                const D3DSURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext )
{

	if (gIsLoading)
		assrt(false);

	if( g_pFont )
         g_pFont->OnResetDevice();

	if( g_pBigFont )
         g_pBigFont->OnResetDevice();

	g_pTextSprite.destroy();
	D3DXCreateSprite( pd3dDevice, g_pTextSprite.pptr() );

	if (gGame && gGame->isLoaded()) {

		gGame->resetDevice(pd3dDevice);
	}
	
	ui_OnResetDevice();

	if (pd3dDevice) {

		logger.log(LOG_INFO, true, true, L"Graphics Mode");
		logger.log(LOG_INFO, true, true, L"-------------");
		DXUTUpdateFrameStats();

		logger.log(LOG_INFO, true, true, DXUTGetFrameStats() );
		
		if (pBackBufferSurfaceDesc) {

			logger.log(LOG_INFO, false, false, L" (AA = %d)", (int) pBackBufferSurfaceDesc->MultiSampleType);
		}

		logger.log(LOG_INFO, true, true, DXUTGetDeviceStats() );

		logger.log(LOG_INFO, true, true, L"");
	}

	return S_OK;
}


//--------------------------------------------------------------------------------------
// Handle updates to the scene
//--------------------------------------------------------------------------------------
void CALLBACK OnFrameMove( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext )
{
	if (!gIsLoading) {
		
		if (gGame) {

			WOF::FramePerformance& per = gGame->framePerformance();
			per.frameMoveInterval = ((float) DXUTWEGetFrameMoveTime()) / 1000.0f;

			gGame->frameMove(fTime, fElapsedTime, gIsRenderFrame);
		}
		
		DXUTWESetIsRenderFrame(gIsRenderFrame);
	}

	appUpdateCursor();
}


//--------------------------------------------------------------------------------------
// Render the help and statistics text. This function uses the ID3DXFont interface for 
// efficient text rendering.
//--------------------------------------------------------------------------------------
void RenderText()
{
	if (gGame.isNull()) {

		return;
	}

	
    // The helper object simply helps keep track of text position, and color
    // and then it calls pFont->DrawText( m_pSprite, strMsg, -1, &rc, DT_NOCLIP, m_clr );
    // If NULL is passed in as the sprite object, then it will work however the 
    // pFont->DrawText() will not be batched together.  Batching calls will improves performance.
   
	if (gGame->showFPS()) {

		const D3DSURFACE_DESC* pd3dsdBackBuffer = DXUTGetBackBufferSurfaceDesc();
		CDXUTTextHelper txtHelper( g_pFont, g_pTextSprite, 16 );

		// Output statistics
		txtHelper.Begin();
		txtHelper.SetInsertionPos(5, 5 );
		txtHelper.SetForegroundColor( D3DXCOLOR( 1.0f, 1.0f, 0.0f, 1.0f ) );
		txtHelper.DrawTextLine( DXUTGetFrameStats() );
		txtHelper.DrawTextLine( DXUTGetDeviceStats() );
		
		txtHelper.DrawTextLine(L"");


		static WE::BufferString tempStr;
		
		tempStr.assignEx(L"%dxSleep(%d), [%d Lags]", gGame->framePerformance().sleepCount, gGame->framePerformance().sleepInterval, gGame->framePerformance().lagCount);
		txtHelper.DrawTextLine( tempStr.c_tstr() );

		tempStr.assignEx(L"%.3f = %.3f + %.3f", 1000.0f * (gGame->framePerformance().frameMoveTime + gGame->framePerformance().renderTime), 1000.0f * gGame->framePerformance().frameMoveTime, 1000.0f * gGame->framePerformance().renderTime);
		txtHelper.DrawTextLine( tempStr.c_tstr() );

		txtHelper.DrawTextLine(L"");

		tempStr.assignEx(L"Sim Time: %.3f", (float) gGame->getTime());
		txtHelper.DrawTextLine( tempStr.c_tstr() );

		//log(L"dt = %u", (unsigned int) DXUTWEGetPresentTime());
		
		/*
		static float maxElapsedTime = 0.0f;
		static int startWorseFPSCounter = 0;
		static bool startWorseFPSTest = false;
		float elapsedTime = DXUTGetElapsedTime();

		
		if (startWorseFPSTest) {

			maxElapsedTime = MMax(maxElapsedTime, elapsedTime);
		}

		
		static WE::BufferString matchTime;
		
		matchTime.assignEx(L"Sim Time: %.3f, Worst FPS: %.2f", (float) gGame->getTime(), (1.0f / maxElapsedTime));
		txtHelper.DrawTextLine( matchTime.c_tstr() );
		

		if (startWorseFPSTest == false) {
			
			if (startWorseFPSCounter >= 10) {

				startWorseFPSTest = true;
			} else {

				if(gGame->isLoaded()) {
					startWorseFPSCounter++;
				}
			}
		}
		*/

		txtHelper.End();
	}
}


//--------------------------------------------------------------------------------------
// Render the scene 
//--------------------------------------------------------------------------------------
void CALLBACK OnFrameRender( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext )
{

	HRESULT hr;

	 // Clear the render target and the zbuffer 
	 V( pd3dDevice->Clear(0, NULL,	D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | 
									(gVideoDeviceSelection.minStencilBitDepth ? D3DCLEAR_STENCIL : 0), 
									D3DCOLOR_ARGB(0, 45, 50, 170), 1.0f, 0) );

    
	// Render the scene
	if( SUCCEEDED( pd3dDevice->BeginScene() ) )
	{

		if (gIsLoading) {

			gGame->renderSplashScreen(gLoadThreadHandle != NULL);

			if (AppShowLoadTime) {

				static TCHAR sign[4] = {L'|', L'/', L'-', L'\\'};
				static short signIndex = 0;	
				static float inc = 0.0f;
				static float loadTime = -1.0f;
				
				if (loadTime == -1.0f)
					loadTime = 0.0;
				else
					loadTime += fElapsedTime;

				inc += (fElapsedTime / 0.2f);

				if (inc > 1.0f) {

					int intInc = (int) inc;
					inc -= (float) intInc;

					++signIndex;
				}

				signIndex %= 4;

				String tempStr;

				tempStr.assignEx(L"%.1f %c%c%c", loadTime, sign[signIndex], sign[signIndex], sign[signIndex]);
				
				const D3DSURFACE_DESC* pd3dsdBackBuffer = DXUTGetBackBufferSurfaceDesc();
				CDXUTTextHelper txtHelper( g_pFont, g_pTextSprite, 16 );

				// Output statistics
				txtHelper.Begin();
				txtHelper.SetInsertionPos(2, pd3dsdBackBuffer->Height - 14);
				txtHelper.SetForegroundColor( D3DXCOLOR( 1.0f, 1.0f, 0.0f, 1.0f ) );
				
				txtHelper.DrawTextLine(tempStr.c_tstr());
				txtHelper.End();
			}
			
				
		} else {


			WOF::FramePerformance& per = gGame->framePerformance();
			per.renderInterval = ((float) DXUTWEGetRenderTime()) / 1000.0f;
			per.presentInterval = ((float) DXUTWEGetPresentTime()) / 1000.0f;

			if (gGame) {

				gGame->render(g_pTextSprite, g_pFont, g_pBigFont);
			}

			ui_OnFrameRender(fElapsedTime);

			RenderText();		
		}

		if (pd3dDevice->EndScene() != D3D_OK)
			assrt(false);

	} else {

		log(L"BeginScene Failed");
	}

	if (mWOFDebugger.isValid()) {

		mWOFDebugger.update();
	}
}


//--------------------------------------------------------------------------------------
// Handle messages to the application 
//--------------------------------------------------------------------------------------
LRESULT CALLBACK MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, 
                          bool* pbNoFurtherProcessing, void* pUserContext )
{

	if (gIsLoading) {
	} else {

		if (gGame) {

			if (uMsg == WM_MOUSEMOVE) {

				onCursorAction();
			}

			if (ui_OnMsgProc(hWnd, uMsg, wParam, lParam, 
							  pbNoFurtherProcessing, pUserContext ))
				return 0;

			if (gGame->processWndMessage(uMsg, wParam, lParam)) 
				return 0;
		}
	}
	

    return 0;
}


//--------------------------------------------------------------------------------------
// Release resources created in the OnResetDevice callback here 
//--------------------------------------------------------------------------------------
void CALLBACK OnLostDevice( void* pUserContext )
{
	ui_OnLostDevice();

	 if(g_pFont)
        g_pFont->OnLostDevice();

	 if(g_pBigFont)
        g_pBigFont->OnLostDevice();
    
	g_pTextSprite.destroy();

	if (gGame && gGame->isLoaded()) {
		gGame->onLostDevice();
	}
}


//--------------------------------------------------------------------------------------
// Release resources created in the OnCreateDevice callback here
//--------------------------------------------------------------------------------------
void CALLBACK OnDestroyDevice( void* pUserContext )
{
	ui_OnDestroyDevice();

	g_pFont.destroy();
	g_pBigFont.destroy();
}

WE::String gMediaSearchPath(L"..\\res");

void InitApp() {

	DXUTSetMediaSearchPath(gMediaSearchPath.c_tstr());

	ui_InitSize();
	ui_InitApp();

	gGame.destroy();
	MMemNew(gGame.ptrRef(), WOF::Game());

	if (mWOFDebugger.isValid()) {

		gGame->setHasWOFDebugger(true);
		mWOFDebugger.setGame(gGame);
	}
}

void DestroyApp() {

	mWOFDebugger.destroy();
	gGame.destroy();
}


bool tryCreateDevice(VideoDeviceSelectionContext& context) {

	context.tryAntiAlias = AppAntiAlias;
	context.antiAliasSamples = AppAntiAliasSamples;

	context.hasModifySettingsAlternatives = false;
	context.modifySettingsAlternativeIndex = -1;

	while ((context.tryAntiAlias) && (context.antiAliasSamples >= 2)) {

		if (DXUTCreateDevice( D3DADAPTER_DEFAULT, AppWindowed, AppWidth, AppHeight, 
						IsDeviceAcceptable, ModifyDeviceSettings,
						&context) == S_OK) {
			
				return true;
		}

		--context.antiAliasSamples;
	}

	return (DXUTCreateDevice( D3DADAPTER_DEFAULT, AppWindowed, AppWidth, AppHeight, 
						IsDeviceAcceptable, ModifyDeviceSettings,
						&context) == S_OK);
}

bool createDevice() {

	
	int tryCount = 0;

	gVideoDeviceSelection.maxColorDepth = AppMaxColorDepth;

	while (true) {

		do {

			if (gGame->fillDeviceSelection(gVideoDeviceSelection, tryCount) == false) {

				return false;
			}

			if (tryCreateDevice(gVideoDeviceSelection) == true) {

				return true;
			}

			++gVideoDeviceSelection.modifySettingsAlternativeIndex;

		} while (gVideoDeviceSelection.hasModifySettingsAlternatives);
		
		tryCount++;
	}

	return false;
}


void OnCreateWindow(HWND hWnd) {
	
}

bool processCommandLine() {

	WE::FilePath exePath(WE::AppPath::getPath());

	exePath.getPath(gMediaSearchPath);
	gMediaSearchPath.append(L"../res");

	return true;
}

bool loadConfig(String& outConfigPath) {

	WE::BufferString pathStr(AppPath::getPath());
	
	const TCHAR* logPath = L"config.xml";
	
	WE::PathResolver::appendPath(pathStr, logPath, true, true);

	BufferString tempStr;
	WE::SoftRef<WE::DataSource> source = WE::DataSourceCreator::create(pathStr.c_tstr(), NULL);

	if (source.isNull()) {

		return false;
	}

	outConfigPath.assign(pathStr);

	logger.log(LOG_INFO, true, true, L"Loading Config File [%s]", pathStr.c_tstr());

	WE::SoftRef<WE::DataChunk> child = source->getRootFirstChild();

	while (child.isValid()) {

		child->getValue(tempStr);

		if (tempStr.equals(L"videoSettings")) {

			child->scanAttribute(tempStr, L"windowed", AppWindowed);
			child->scanAttribute(tempStr, L"width", L"%d", &AppWidth);
			child->scanAttribute(tempStr, L"height", L"%d", &AppHeight);
			child->scanAttribute(tempStr, L"vSync", AppVSync);
			child->scanAttribute(tempStr, L"antiAlias", AppAntiAlias);
			child->scanAttribute(tempStr, L"antiAliasSamples", L"%d", &AppAntiAliasSamples);
			child->scanAttribute(tempStr, L"maxColorDepth", L"%d", &AppMaxColorDepth);
		}

		toNextSibling(child);
	}

	child = source->getChild(L"tweak");

	if (child.isValid()) {

		child->scanAttribute(tempStr, L"showLoadTime", AppShowLoadTime);
		child->scanAttribute(tempStr, L"enableDebugger", gEnableWOFDebugger);
		child->scanAttribute(tempStr, L"enableMultiThreadLoad", AppMultiThreadLoad);
		

		if (child->getAttribute(L"debuggerPath", mWOFDebuggerDLLPath)) {

			child->resolveFilePath(mWOFDebuggerDLLPath);
		}
		
	}

	/*
	{
		BufferString pathStr(AppPath::getPath());

		const TCHAR* sceneChunkPath = L"../assets/WOF.xml/plugins";

		PathResolver::appendPath(pathStr, sceneChunkPath, true, true);

	}
	*/

	return true;
}


//--------------------------------------------------------------------------------------
// Initialize everything and go into a render loop
//--------------------------------------------------------------------------------------
INT AppWinMain(HINSTANCE hInst)
{

	gAppInfo.mAppInstance = hInst;

	_memCheckSetAddress((void*) 0);

	if (processCommandLine() == false) {

		return -1;
	}

	WE::BufferString pathStr(AppPath::getPath());
	const TCHAR* logPath = L"log.txt";
	
	WE::PathResolver::appendPath(pathStr, logPath, true, true);

	logger.init(LOG_MASK_LOGLEVEL | LOG_MASK_FILEPATH | LOG_MASK_OPTIONBITS, NULL, pathStr.c_tstr(), LOG_DEBUG, 
				LOG_ENABLED_BIT | LOG_LOG_TOFILE_BIT /*| LOG_LOG_TOCONSOLE_BIT*/);
	
	logger.log(LOG_FORCE, true, true, L"~o~o~o~o~");

	WE::Version ver;
	WitchEngine::getVersion(ver);

	logger.log(LOG_INFO, true, true, L"WitchEngine %d.%d.%lu %s %s", ver.major, ver.minor, ver.build, ver.name, ver.comment);
	logger.log(LOG_INFO, true, true, L"WOF build %lu", _VERSION_BUILD_WOF2);

	logger.log(L"");
	logger.log(L"Command Line: %s", GetCommandLine());
	logger.log(L"App Path: %s", WE::AppPath::getPath());
	logger.log(L"");

	setLog(&logger);

	String configPath;
	loadConfig(configPath);

	//gEnableWOFDebugger = false;
	{
		if (gEnableWOFDebugger)
			mWOFDebugger.load(mWOFDebuggerDLLPath.c_tstr());
	}

	// Set the callback functions
    DXUTSetCallbackDeviceCreated( OnCreateDevice );
    DXUTSetCallbackDeviceReset( OnResetDevice );
    DXUTSetCallbackDeviceLost( OnLostDevice );
    DXUTSetCallbackDeviceDestroyed( OnDestroyDevice );
    DXUTSetCallbackMsgProc( MsgProc );
    DXUTSetCallbackFrameRender( OnFrameRender );
    DXUTSetCallbackFrameMove( OnFrameMove );

	// TODO: Perform any application-level initialization here
	InitApp();

    // Initialize DXUT and create the desired Win32 window and Direct3D device for the application
    DXUTInit( true, true, false ); // Parse the command line, handle the default hotkeys, and show msgboxes
    DXUTSetCursorSettings( true, true ); // Show the cursor and clip it when in full screen

	{
		int windX;
		int windY;

		if (mWOFDebugger.isValid()) {
			
			windX = 0;
			windY = 0;

		} else {

			windX = CW_USEDEFAULT;
			windY = CW_USEDEFAULT;
		}

		DXUTCreateWindow( AppWindowName, 0, LoadIcon(hInst, MAKEINTRESOURCE(AppIconResource)), NULL, windX, windY);
	}

	//DXUTGetD3DDevice() != NULL added becuase of DXUT 'bug' in release build?
	if (createDevice() && DXUTGetD3DDevice() != NULL) {

		OnCreateWindow(DXUTGetHWND());
		gAppInfo.mWnd = DXUTGetHWND();
		
		logger.log(LOG_INFO, true, true, L"");
		DXUTGetD3DDevice()->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, TRUE);

		if (startLoading(configPath.c_tstr())) {

			if (mWOFDebugger.isValid())
				mWOFDebugger.start(DXUTGetHWND());

			//old way
			DXUTWESetIsRenderFrame(false);

			// Start the render loop
			DXUTMainLoop();

		} else {

			logger.log(L"Failed To Start Loading");
		}

	} else {

		logger.log(L"Video Device Creation Failed");
	}

	// TODO: Perform any application-level cleanup here
	DestroyApp();

	logger.log(LOG_INFO, true, true, L"");
	logger.log(LOG_INFO, true, true, L"~o~o~o~o~");
	logger.log(LOG_INFO, true, true, L"Closing Stats");
	logger.log(LOG_INFO, true, true, DXUTGetFrameStats() );
	logger.log(LOG_INFO, true, true, DXUTGetDeviceStats() );

    return DXUTGetExitCode();
}


