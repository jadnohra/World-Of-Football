// WOF_Debugger.cpp : Defines the entry point for the DLL application.
//

#include "windows.h"
#include "WOF_Debugger.h"

#include "WE3/binding/directx/9/dxut9/includeDXUT9.h"
#include "WE3/WEAppPath.h"
#include "WE3/string/WEBufferString.h"


BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
    return TRUE;
}

class CWOF_Debugger {
public:

	CWOF_Debugger();
	~CWOF_Debugger();

	void setGame(WOFDebugSource* pGame);

	void start(HWND WOFWnd);
	void update();
	
public:

	WOFDebugSource* mGame;


	ID3DXFont* mpFont;
	ID3DXSprite* mpTextSprite;

	CDXUTDialogResourceManager mDialogResourceManager; 
	CDXUTDialog mHUD; 
	CDXUTDialog mUI;
	CDXUTDialog mUIBack;
	CD3DSettingsDlg mSettingsDlg;

	bool mShowBallTactical;
	bool mShowBallIntercept;
	bool mShowBallDist;
};

void* newWOFDebugger(void)
{
	return new CWOF_Debugger();
}

void deleteWOFDebugger(void* pDebugger)
{
	if (pDebugger)
		delete (CWOF_Debugger*) pDebugger;
}

CWOF_Debugger::CWOF_Debugger() : mGame(NULL) {

	mpFont = NULL;
	mpTextSprite = NULL;

	mShowBallTactical = false;
	mShowBallIntercept = false;
	mShowBallDist = false;
}

CWOF_Debugger::~CWOF_Debugger() {

	DXUTShutdown();
}

void setGame(void* pDebugger, WOFDebugSource* pGame) {

	((CWOF_Debugger*) pDebugger)->setGame(pGame);
}


void start(void* pDebugger, HWND WOFWnd) {

	((CWOF_Debugger*) pDebugger)->start(WOFWnd);
}

void update(void* pDebugger) {

	((CWOF_Debugger*) pDebugger)->update();
}

//--------------------------------------------------------------------------------------
// Rejects any devices that aren't acceptable by returning false
//--------------------------------------------------------------------------------------
bool CALLBACK IsDeviceAcceptable( D3DCAPS9* pCaps, D3DFORMAT AdapterFormat, 
                                  D3DFORMAT BackBufferFormat, bool bWindowed, void* pUserContext ) {
    return true;
}

bool CALLBACK ModifyDeviceSettings( DXUTDeviceSettings* pDeviceSettings, const D3DCAPS9* pCaps, void* pUserContext ) {

	return true;
}

void CWOF_Debugger::update() {

	DXUTWESetIsRenderFrame(true);
	DXUTRender3DEnvironment();
}

void CWOF_Debugger::setGame(WOFDebugSource* pGame) {

	mGame = pGame;
}




//--------------------------------------------------------------------------------------
// Create any D3DPOOL_MANAGED resources here 
//--------------------------------------------------------------------------------------
HRESULT CALLBACK OnCreateDevice( IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext )
{

	CWOF_Debugger* pDeb = (CWOF_Debugger*) pUserContext;

	pDeb->mDialogResourceManager.OnCreateDevice( pd3dDevice );
	pDeb->mSettingsDlg.OnCreateDevice( pd3dDevice );

	D3DXCreateFont( pd3dDevice, 15, 0, FW_BOLD, 0, FALSE, DEFAULT_CHARSET, 
                         OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, 
                         L"Arial", &pDeb->mpFont );

	return S_OK;
}


//--------------------------------------------------------------------------------------
// Create any D3DPOOL_DEFAULT resources here 
//--------------------------------------------------------------------------------------
HRESULT CALLBACK OnResetDevice( IDirect3DDevice9* pd3dDevice, 
                                const D3DSURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext )
{

	CWOF_Debugger* pDeb = (CWOF_Debugger*) pUserContext;

	pDeb->mDialogResourceManager.OnResetDevice();
	pDeb->mSettingsDlg.OnResetDevice();

	if( pDeb->mpFont )
         pDeb->mpFont->OnResetDevice();
	D3DXCreateSprite( pd3dDevice, &pDeb->mpTextSprite );

	pDeb->mHUD.SetLocation( 5, 0 );
    pDeb->mHUD.SetSize( 170, 170 );

	pDeb->mUI.SetLocation( 5 , 0 );
    pDeb->mUI.SetSize( 320, 640 );

	POINT pt;
	pDeb->mUI.GetLocation(pt);
	pDeb->mUIBack.SetLocation( pt.x , pt.y );
	pt.x = pDeb->mUI.GetWidth(); pt.y = pDeb->mUI.GetHeight();
    pDeb->mUIBack.SetSize( pt.x + 1, pt.y + 2 );

	return S_OK;
}



//--------------------------------------------------------------------------------------
// Render the help and statistics text. This function uses the ID3DXFont interface for 
// efficient text rendering.
//--------------------------------------------------------------------------------------
void RenderText(CWOF_Debugger* pDeb)
{
	
    const D3DSURFACE_DESC* pd3dsdBackBuffer = DXUTGetBackBufferSurfaceDesc();
    CDXUTTextHelper txtHelper( pDeb->mpFont, pDeb->mpTextSprite, 15 );

    // Output statistics
    txtHelper.Begin();

	txtHelper.SetInsertionPos( 3, 25 );
	txtHelper.SetForegroundColor( D3DXCOLOR( 1.0f, 1.0f, 0.0f, 1.0f ) );
	
	WE::BufferString tempStr;

	if (pDeb->mGame && pDeb->mUI.GetVisible()) {
	
		WOFDebugSource& game = *pDeb->mGame;

		if (pDeb->mShowBallTactical) {

			tempStr.assignEx(L"TeamA Ball %.3f - %.3f", 
								game.debug_team_getTacticalInstantaneousBallOwnership(0), 
								game.debug_team_getTacticalBallOwnership(0));
			txtHelper.DrawTextLine(tempStr.c_tstr());

			tempStr.assignEx(L"TeamB Ball %.3f - %.3f", 
								game.debug_team_getTacticalInstantaneousBallOwnership(1), 
								game.debug_team_getTacticalBallOwnership(1));
			txtHelper.DrawTextLine(tempStr.c_tstr());
			txtHelper.DrawTextLine(L"");
		}

		if (pDeb->mShowBallIntercept) {

			unsigned int count = game.debug_getBallInterceptFootballerCount();

			WOFDebugSource::FootballerInfo info;
			bool doesInterceptB4Resting;

			for (unsigned int i = 0; i < count; ++i) {

				game.debug_getBallInterceptFootballer(i, info, doesInterceptB4Resting);

				tempStr.assignEx(L"%d-%d %c", info.team, info.number, doesInterceptB4Resting ? L'*' : L' ');
				txtHelper.DrawTextLine(tempStr.c_tstr());
			}
			
			if (count)
				txtHelper.DrawTextLine(L"");
		}

		if (pDeb->mShowBallDist) {

			unsigned int count = game.debug_getBallDistFootballerCount();

			WOFDebugSource::FootballerInfo info;

			for (unsigned int i = 0; i < count; ++i) {

				game.debug_getBallDistFootballer(i, info);

				tempStr.assignEx(L"%d-%d", info.team, info.number);
				txtHelper.DrawTextLine(tempStr.c_tstr());
			}
			
			if (count)
				txtHelper.DrawTextLine(L"");
		}
	}

    txtHelper.End();
	
}



//--------------------------------------------------------------------------------------
// Render the scene 
//--------------------------------------------------------------------------------------
void CALLBACK OnFrameRender( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext )
{

	CWOF_Debugger* pDeb = (CWOF_Debugger*) pUserContext;

    HRESULT hr;

	 if( pDeb->mSettingsDlg.IsActive() )
    {
       pDeb-> mSettingsDlg.OnRender( fElapsedTime );
        return;
    }

    // Clear the render target and the zbuffer 
    V( pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER /*| D3DCLEAR_STENCIL*/, D3DCOLOR_ARGB(0, 45, 50, 170), 1.0f, 0) );

    // Render the scene
    if( SUCCEEDED( pd3dDevice->BeginScene() ) )
    {
		
		pDeb->mUIBack.OnRender( fElapsedTime );
		pDeb->mUI.OnRender( fElapsedTime );
		
		pDeb->mHUD.OnRender( fElapsedTime );

		RenderText(pDeb);	
        V( pd3dDevice->EndScene() );
    }
}


//--------------------------------------------------------------------------------------
// Handle messages to the application 
//--------------------------------------------------------------------------------------
LRESULT CALLBACK MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, 
                          bool* pbNoFurtherProcessing, void* pUserContext )
{
	
	CWOF_Debugger* pDeb = (CWOF_Debugger*) pUserContext;

	if( pDeb->mSettingsDlg.IsActive() )
    {
        pDeb->mSettingsDlg.MsgProc( hWnd, uMsg, wParam, lParam );
        return 0;
    }

	*pbNoFurtherProcessing = pDeb->mDialogResourceManager.MsgProc( hWnd, uMsg, wParam, lParam );
    if( *pbNoFurtherProcessing )
        return 0;

	*pbNoFurtherProcessing = pDeb->mHUD.MsgProc( hWnd, uMsg, wParam, lParam );
    if( *pbNoFurtherProcessing )
        return 0;

	*pbNoFurtherProcessing = pDeb->mUI.MsgProc( hWnd, uMsg, wParam, lParam );
    if( *pbNoFurtherProcessing )
        return 0;

	return 0;
}


//--------------------------------------------------------------------------------------
// Release resources created in the OnResetDevice callback here 
//--------------------------------------------------------------------------------------
void CALLBACK OnLostDevice( void* pUserContext )
{

	CWOF_Debugger* pDeb = (CWOF_Debugger*) pUserContext;

	pDeb->mDialogResourceManager.OnLostDevice();
	pDeb->mSettingsDlg.OnLostDevice();

	if(pDeb->mpFont)
        pDeb->mpFont->OnLostDevice();
    MSafeRelease(pDeb->mpTextSprite);
}


//--------------------------------------------------------------------------------------
// Release resources created in the OnCreateDevice callback here
//--------------------------------------------------------------------------------------
void CALLBACK OnDestroyDevice( void* pUserContext )
{

	CWOF_Debugger* pDeb = (CWOF_Debugger*) pUserContext;

	pDeb->mDialogResourceManager.OnDestroyDevice();
	pDeb->mSettingsDlg.OnDestroyDevice();

	MSafeRelease( pDeb->mpFont );
}

enum IDCS {

	IDC_None = -1, IDC_HUD_Toggle, 
	IDC_SHOW_BallTactical, IDC_SHOW_BallIntercept,IDC_SHOW_BallDist,
};

void CALLBACK OnGUIEvent(UINT nEvent, int nControlID, CDXUTControl* pControl, void* pUserContext) {

	CWOF_Debugger* pDeb = (CWOF_Debugger*) pUserContext;

	switch (nControlID) {

		case IDC_HUD_Toggle: {

			pDeb->mHUD.SetVisible(!pDeb->mHUD.GetVisible());

			pDeb->mUIBack.SetVisible(!pDeb->mHUD.GetVisible());
			pDeb->mUI.SetVisible(!pDeb->mHUD.GetVisible());

		} break;

		case IDC_SHOW_BallTactical: {

			pDeb->mShowBallTactical = ((CDXUTCheckBox*) pControl)->GetChecked();

		} break;

		case IDC_SHOW_BallIntercept: {

			pDeb->mShowBallIntercept = ((CDXUTCheckBox*) pControl)->GetChecked();

		} break;

		case IDC_SHOW_BallDist: {

			pDeb->mShowBallDist = ((CDXUTCheckBox*) pControl)->GetChecked();

		} break;

	}
}

void CWOF_Debugger::start(HWND WOFWnd) {

	WE::String gMediaSearchPath(L"..\\res");
	WE::FilePath exePath(WE::AppPath::getPath());

	exePath.getPath(gMediaSearchPath);
	gMediaSearchPath.append(L"../res");

	DXUTSetMediaSearchPath(gMediaSearchPath.c_tstr());

	mSettingsDlg.Init( &mDialogResourceManager );
	mHUD.Init( &mDialogResourceManager );
	mUI.Init( &mDialogResourceManager );
	mUIBack.Init( &mDialogResourceManager );
	
	mHUD.SetVisible(true);
	mUI.SetVisible(false);
	mUIBack.SetVisible(false);

	int iY = 0; 
	int iY2 = 0; 
	int iX = 0;

	mHUD.SetCallback(OnGUIEvent, this); 
	iY = 10;
	
	/*
	mUIBack.SetBackgroundColors(D3DCOLOR_RGBA(0, 0, 0, 150));
	mUI.SetBackgroundColors(D3DCOLOR_RGBA(100, 140, 205, 150), D3DCOLOR_RGBA(55, 105, 190, 150), 
									D3DCOLOR_RGBA(55, 105, 190, 150), D3DCOLOR_RGBA(10, 75, 180, 150));
									*/
	mUI.SetCallback(OnGUIEvent, this); 

	iX = 3;
	iY = 10;
	mHUD.AddButton(IDC_HUD_Toggle, L"Toggle", iX, iY = 5, 103, 18, true);
	mHUD.AddCheckBox(IDC_SHOW_BallTactical, L"Ball Tactical", iX, iY += 25, 103, 18, false);
	mHUD.AddCheckBox(IDC_SHOW_BallIntercept, L"Ball Intercept", iX, iY += 20, 103, 18, false);
	mHUD.AddCheckBox(IDC_SHOW_BallDist, L"Ball Dist", iX, iY += 20, 103, 18, false);
	
	iX = 3;
	iY = 10;
	mUI.AddButton(IDC_HUD_Toggle, L"Toggle", iX, iY = 5, 103, 18, true);
	
	/*
	iX = 3;
	iY = 10;
	//mUI.AddStatic(IDC_RQ, L"Render Quality", iX, iY = 5, 140, 24);
	//mUI.AddStatic(IDC_ST_FULLSCREEN, L"Full Screen:", iX, iY += 20, 70, 24);
	//mUI.AddCheckBox(IDC_FULLSCREEN, L"", iX + 70 + 7, iY + 5, 103, 18, false);

	mUI.AddStatic(IDC_ST_GLOBAMBIENT, L"Ambient:", iX, iY, 50, 24);
	mUI.AddSlider(IDC_GLOBAMBIENT, iX + 70 + 7, iY + 5, 103, 18);
	
	mUI.AddStatic(IDC_ST_LIGHT, L"Light:", iX, iY += 20, 40, 24);
	mUI.AddCheckBox(IDC_LIGHT, L"", iX + 70 + 7, iY + 5, 103, 18, true);
	mUI.AddStatic(IDC_ST_AMBIENT, L"Ambient:", iX, iY += 20, 50, 24);
	mUI.AddSlider(IDC_AMBIENT, iX + 70 + 7, iY + 5, 103, 18);
	mUI.AddStatic(IDC_ST_DIFFUSE, L"Diffuse:", iX, iY += 20, 50, 24);
	mUI.AddSlider(IDC_DIFFUSE, iX + 70 + 7, iY + 5, 103, 18);
	mUI.AddStatic(IDC_ST_SPECULAR, L"Specular:", iX, iY += 20, 50, 24);
	mUI.AddCheckBox(IDC_ENABLE_SPECULAR, L"", iX + 70 + 7, iY + 5, 18, 18, gSpecular);
	mUI.AddSlider(IDC_SPECULAR, iX + 70 + 7 + 20, iY + 5, 103 - 20, 18);
	mUI.AddStatic(IDC_ST_LIGHTDIR, L"Dir.:", iX, iY += 20, 50, 24);
	mUI.AddSlider(IDC_LIGHTDIR_X, iX + 70 + 7, iY + 5, 31, 18);
	mUI.AddSlider(IDC_LIGHTDIR_Y, iX + 70 + 7 + 35, iY + 5, 31, 18);
	mUI.AddSlider(IDC_LIGHTDIR_Z, iX + 70 + 7 + 35*2, iY + 5, 31, 18);
	mUI.AddStatic(IDC_ST_LIGHTATTACH, L"Track:", iX, iY += 20, 50, 24);
	mUI.AddCheckBox(IDC_LIGHTATTACH, L"", iX + 70 + 7, iY + 5, 103, 18, gLightAttach);
	

	iY2 = 10;
	
	mUI.AddButton(IDC_CHANGEDEVICE, L"Video Settings", iX + 200 + 3, iY2, 100, 20 );
	mUI.AddRadioButton(IDC_QUALITY_LOW, IDC_QUALITY_GROUP, L"Low Quality", iX + 200 + 3, iY2 += 25, 103, 18, false);
	mUI.AddRadioButton(IDC_QUALITY_NORMAL, IDC_QUALITY_GROUP, L"Normal Quality", iX + 200 + 3, iY2 += 20, 103, 18, true);
	mUI.AddRadioButton(IDC_QUALITY_HIGH, IDC_QUALITY_GROUP, L"High Quality", iX + 200 + 3, iY2 += 20, 103, 18, false);
	//mUI.AddCheckBox(IDC_FULLSCREEN, L"Full Screen", iX + 200 + 3, iY2 += 30, 103, 18, false);
	mUI.AddCheckBox(IDC_MIPMAP, L"Mipmap", iX + 200 + 3, iY2 += 20, 103, 18, true);
	mUI.AddRadioButton(IDC_CAM_MODEL, IDC_CAM_GROUP, L"Model Camera", iX + 200 + 3, iY2 += 30, 103, 18, !gUseSceneCam);
	mUI.AddRadioButton(IDC_CAM_SCENE, IDC_CAM_GROUP, L"Scene Camera", iX + 200 + 3, iY2 += 20, 103, 18, gUseSceneCam);
	mUI.AddCheckBox(IDC_SCENE_BPV, L"Scene Volumes", iX + 200 + 3, iY2 += 30, 103, 18, gScene.mRenderBroadPhaseVolumes);
	mUI.AddCheckBox(IDC_SCENE_COLL, L"Scene Collision", iX + 200 + 3, iY2 += 20, 103, 18, gSceneColl);

	mUI.AddComboBox(IDC_ENTITY_COMBO, iX, iY += 115, 200, 24);

	mUI.AddStatic(IDC_COUNT, L"1", 200 + iX + 3, iY, 100, 24);
	mUI.AddComboBox( IDC_ANIMS, iX, iY += 20, 200, 24);
	mUI.AddSlider(IDC_ANIMSPEED, iX + 200 + 3, iY + 5, 103, 18);

	mUI.AddListBox( IDC_DEBUGLIST, iX + 3, iY += 40, 308, 328);
	*/
	
	/*
	mHUD.SetFont(1, L"Courier New", 16, FW_NORMAL );
	mHUD.AddEditAABox( IDC_EDITBOX1, L"", 20, 20, 200, 32 );
	*/

	
	DXUTSetCallbackDeviceCreated( OnCreateDevice, this );
    DXUTSetCallbackDeviceReset( OnResetDevice, this );
    DXUTSetCallbackDeviceLost( OnLostDevice, this );
    DXUTSetCallbackDeviceDestroyed( OnDestroyDevice, this );
    DXUTSetCallbackMsgProc( MsgProc, this );
    DXUTSetCallbackFrameRender( OnFrameRender, this );


	DXUTInit( true, true, false ); 
	DXUTSetCursorSettings( true, true ); 
    
	int x = 0;
	int y = 0;
	int width = 200;
	int height = 200;

	WINDOWINFO WOFWndInfo;

	if (GetWindowInfo(WOFWnd, &WOFWndInfo)) {

		x = WOFWndInfo.rcWindow.right + WOFWndInfo.cxWindowBorders;
		y = WOFWndInfo.rcWindow.top;

		height = (WOFWndInfo.rcClient.bottom - WOFWndInfo.rcClient.top);
	}
	
	DXUTCreateWindow( L"World Of Footballer Debugger - Kimera Studios", 0, 0,  NULL, x, y, L"WOFDebugger");


	DXUTCreateDevice( D3DADAPTER_DEFAULT, true, width, height, 
						IsDeviceAcceptable, ModifyDeviceSettings,
						NULL);
}