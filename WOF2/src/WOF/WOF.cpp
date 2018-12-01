#include "WOF.h"

#include "WE3/WEMem.h"
#include "WE3/data/WEDataSourceCreator.h"
#include "WE3/binding/win/includeWindows.h"
#include "WE3/WEAppPath.h"
#include "WE3/filePath/WEPathResolver.h"
#include "WE3/binding/openal/WEAudioOpenAL.h"
#include "WE3/helper/load/WERenderDataLoadHelper.h"
#include "WE3/input/include.h"
#include "WE3/input/ComponentUtil.h"
using namespace WE;

#include "../WOFApp.h"
#include "match/Match.h"
#include "match/DataTypes.h"
#include "match/Player.h"
#include "../autoVersionBuildNumber.h"

DECLARE_INSTANCE_TYPE_NAME(WOF::Game, CGame);

namespace WOF {

const TCHAR* Game::ScriptClassName = L"CGame";

void Game::declareInVM(SquirrelVM& target) {

	SqPlus::SQClassDef<Game>(ScriptClassName).
		func(&Game::getVersion, L"getVersion").
		func(&Game::getMatch, L"getMatch");
}

Game::Game() {

	mState = State_None;
	mShowFPS = false;
	mTweakEnableDebugKeys = false;
	mHasWOFDebugger = false;
	mLoadTrackVolume = 1.0f;

	mVersion.assignEx(L"WOF build %d", _VERSION_BUILD_WOF2);
}

Game::~Game() {

	destroy();
}

void Game::destroy() {

	destroyMatch();
	mRenderer.destroy();
}

const TCHAR* Game::getVersion() {

	return mVersion.c_tstr();
}

const String& Game::getScriptPath() { 

	return mMatch->getScriptPath(); 
}

bool Game::init(NativeAppInfo& nativeAppInfo, HWND hWnd, IDirect3DDevice9* pDevice, Log& log, Console& console, const TCHAR* pConfigPath) {

	srand(timeGetTime());

	mConfigPath.assign(pConfigPath);
	mSplashPath.assign(L"../res/splash.png");
	mLoadTrackPath.assign(L"../res/load.ogg");

	SoftRef<DataChunk> renderD3D9SettingsChunk;

	if (!mAudioEngine.isValid())
		WE_MMemNew(mAudioEngine.ptrRef(), AudioEngineOpenAL());

	if (!mSplashSprite.isValid())
		WE_MMemNew(mSplashSprite.ptrRef(), RenderSprite(true, true));

	if (mConfigPath.isValid()) {

		SoftRef<DataSource> source = DataSourceCreator::create(mConfigPath.c_tstr(), NULL);
		BufferString tempStr;
		
		if (source.isValid()) {

			SoftRef<DataChunk> root = source->getRoot();
			SoftRef<DataChunk> perfChunk = root->getChild(L"performance");

			renderD3D9SettingsChunk = root->getChild(L"RendererD3D9");

			if (perfChunk.isValid()) {

				if (perfChunk->getAttribute(L"processPriority", tempStr)) {

					if (tempStr.equals(L"2") || tempStr.equals(L"highest")) {

						BOOL ok = SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);
						assrt(ok == TRUE);

					} else if (tempStr.equals(L"1") || tempStr.equals(L"higher")) {

						BOOL ok = SetPriorityClass(GetCurrentProcess(), ABOVE_NORMAL_PRIORITY_CLASS);
						assrt(ok == TRUE);

					} else {

						BOOL ok = SetPriorityClass(GetCurrentProcess(), NORMAL_PRIORITY_CLASS);
						assrt(ok == TRUE);
					}
				}
			}

			SoftRef<DataChunk> pathsChunk = root->getChild(L"paths");

			if (pathsChunk.isValid()) {

				pathsChunk->getAttribute(L"splash", mSplashPath);
				pathsChunk->getAttribute(L"loadTrack", mLoadTrackPath);
				pathsChunk->getAttribute(L"assets", mAssetsPath);
				pathsChunk->getAttribute(L"local", mLocalPath);
				pathsChunk->getAttribute(L"plugins", mPluginsPath);
			}

			SoftRef<DataChunk> tweakChunk = root->getChild(L"tweak");

			if (tweakChunk.isValid()) {

				tweakChunk->scanAttribute(tempStr, L"enableDebugKeys", mTweakEnableDebugKeys);
				tweakChunk->scanAttribute(tempStr, L"loadTrackVolume", L"%f", &mLoadTrackVolume);
		

				RenderDataLoadHelper::extract(tempStr, tweakChunk, mSplashSprite->skin().color(), true, L"splashColor", 1.0f);
				mSplashSprite->skin().color().alpha = 1.0f;
			}

		}
	}

	{
		BufferString temp;

		{
			temp.assign(mSplashPath.c_tstr());
			mSplashPath.assign(AppPath::getPath());

			PathResolver::appendPath(mSplashPath, temp.c_tstr(), true, true);
		}

		{
			temp.assign(mLoadTrackPath.c_tstr());
			mLoadTrackPath.assign(AppPath::getPath());

			PathResolver::appendPath(mLoadTrackPath, temp.c_tstr(), true, true);
		}		

		{
			temp.assign(mAssetsPath.c_tstr());
			mAssetsPath.assign(AppPath::getPath());

			PathResolver::appendPath(mAssetsPath, temp.c_tstr(), true, true);
		}

		{
			temp.assign(mLocalPath.c_tstr());
			mLocalPath.assign(AppPath::getPath());

			PathResolver::appendPath(mLocalPath, temp.c_tstr(), true, true);
		}

		{
			temp.assign(mPluginsPath.c_tstr());
			mPluginsPath.assign(AppPath::getPath());

			PathResolver::appendPath(mPluginsPath, temp.c_tstr(), true, true);
		}
	}

	mNativeAppInfo = &nativeAppInfo;
	mLog = &log;
	mConsole = &console;

	destroy();

	MMemNew(mRenderer.ptrRef(), RendererD3D9(renderD3D9SettingsChunk));
	resetDevice(pDevice);

	MMemNew(mScriptEngine.ptrRef(), ScriptEngine());

	if (!mScriptEngine->init(mLog, mConsole, NULL)) {

		log.log(LOG_ERROR, true, true, L"Failed to Create Scripting Engine");

		destroy();
		return false;
	}

	mScriptEngine->setGameObject(&scriptArg(this));

	{
		bool initSuccess = false;

		//OpenAL Init sometimes fails without reason and succeeds on the next call!
		for (int i = 0; i < 2 && !initSuccess; ++i) {

			initSuccess = mAudioEngine->init(mPluginsPath.c_tstr());

			if (!initSuccess)
				Sleep(50);
		}

		if (!initSuccess)
			mAudioEngine.destroy();
	}

	{
		LoadContextImpl loadContext;
		loadContext.render().setRenderer(mRenderer);

		BufferString tempStr;

		mSplashSprite->skin().loadTexture(mSplashPath.c_tstr(), loadContext.render());
		mSplashSprite->skin().mTexFilter = RTF_Anisotropic;
	}

	{
		bool isFolder;

		if (mAudioEngine.isValid() && Win::isValidFile(mLoadTrackPath.c_tstr(), isFolder) && !isFolder) {

			mAudioEngine->createTrack(mLoadTrackPath.c_tstr(), mLoadTrack.ptrRef());
		}
	}

	

	return true;
}


void Game::renderSplashScreen(bool enableLoadTrack) {

	if (mLoadTrack.isValid())
		mAudioEngine->updateTrackBuffers(mLoadTrack);

	if (mSplashSprite.isValid()) {

		RenderViewportInfo viewport;

		if (mRenderer->getViewportInfo(viewport)) {
		
			mRenderer->setViewTransform2D(NULL, true);

			mSplashSprite->setSize((float) viewport.width, (float) viewport.height, RenderQuad::Anchor_Start, RenderQuad::Anchor_Start);
			mSplashSprite->render2D(mRenderer);
		}
	}
}

bool Game::fillDeviceSelection(VideoDeviceSelectionContext& selection, int tryCount) {

	switch (tryCount) {
		case 0:
			{
				selection.allowRefDevice = false;
				selection.requireAlphaBlending = true;
				selection.requireVertexBlending = true;
				selection.requireDepthBias = true;

				selection.allowNoPaletteVertexBlending = false;
				selection.allowSoftwareVertexBlending = false;

				selection.minStencilBitDepth = 1;
				selection.vSync = false;
			}
			break;
		case 1:
			{
				selection.allowNoPaletteVertexBlending = true;
			}
			break;
		/*
		case 2:
			{
				selection.allowSoftwareVertexBlending = true;
			}
			break;
		*/
		default:
			return false;
	}

	return true;
}

void Game::onLostDevice() {

	mRenderer->onLostDevice();
}

void Game::resetDevice(IDirect3DDevice9* pDevice) {

	if (mRenderer.isValid()) {

		mRenderer->setDevice(pDevice);
	}
}


bool Game::isLoaded() {

	return (mMatch.isValid());
}

bool Game::load(bool enableLoadTrack) {

	mState = State_Loading;

	if (enableLoadTrack && mLoadTrack.isValid()) {

		mAudioEngine->setTrackGain(mLoadTrack, mLoadTrackVolume);
		mAudioEngine->setTrackLooping(mLoadTrack, true);
		mAudioEngine->start(mLoadTrack);
	}

	bool ret = createMatch();

	if (enableLoadTrack && mLoadTrack.isValid()) {

		for (int i = 0; i < 10; ++i) {

			mAudioEngine->setTrackGain(mLoadTrack, mLoadTrackVolume - (mLoadTrackVolume * ((float) i / 10.0f)));
			Sleep(100);
		}

		mAudioEngine->stop(mLoadTrack);
		mLoadTrack.destroy();
	}

	mState = State_Selecting;

	return ret;
}

bool Game::showFPS() {

	return mShowFPS;
}

bool Game::processWndMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {

	switch (uMsg) {
		case WM_KEYDOWN:
		{
			switch (wParam) {
			case VK_F1:
				mShowFPS = !mShowFPS;
			break;
			}
		}
	}

	if (mMatch.isValid()) {

		return mMatch->processWndMessage(uMsg, wParam, lParam);
	}

	return true;
}

void Game::frameMove(double fTime, float fElapsedTime, bool& isRenderFrame) {

	switch (mState) {

		case State_PlayingMatch: {

			if (mMatch.isValid()) {

				bool isActive 
					=	(mNativeAppInfo->mWnd == GetForegroundWindow())
						&&	
						(!IsIconic(mNativeAppInfo->mWnd))
					;

				mMatch->frameMove(fTime, fElapsedTime, isRenderFrame, isActive);
			}

		} break;

		case State_Selecting: {

			mMatch->getInputManager().update(fTime);
			isRenderFrame = true;
			Sleep(1);

		} break;
	}
}

void Game::render(ID3DXSprite* pSprite, ID3DXFont* pFont, ID3DXFont* pBigFont) {

	mRenderer->begin();
	
	switch (mState) {

		case State_PlayingMatch: {

			if (mMatch.isValid()) {

				mMatch->render();
				mMatch->renderText(pSprite, pFont, pBigFont);
			} 
		} break;
		
		case State_Selecting: {

			renderSplashScreen(false);

			RenderViewportInfo viewport;

			TEXTMETRIC textMetricsHolder;
			SoftPtr<TEXTMETRIC> textMetrics;
				
			if (pBigFont->GetTextMetrics(&textMetricsHolder)) {

				textMetrics = &textMetricsHolder;
			}

			if (mMatch.isValid() && mRenderer->getViewportInfo(viewport) && textMetrics.isValid()) {

				CDXUTTextHelper texter(pBigFont, pSprite, 20);
				texter.Begin();

				texter.SetForegroundColor( D3DXCOLOR( 1.0f, 1.0f, 1.0f, 1.0f ) );

				String selectText;

				float centerX = (float) (viewport.width / 2);
				float centerY = (float) (viewport.height / 2);

				float offsetX = 0.0f;
				float offsetY = 0.0f;

				offsetY = -centerY;

				{
					using namespace match;

					WE::Input::InputManager& inputManager = mMatch->getInputManager();

					for (WE::Input::InputManager::Index i = 0; i < inputManager.getControllerCount(); ++i) {

						SoftRef<WE::Input::Controller> controller = inputManager.getController(i);
						SoftPtr<Player> currPlayer;

						TeamEnum currTeam = Team_Invalid;

						if (controller->isVisible()) {

							for (int ti = 0; ti < 2; ++ti) {

								Team& team = mMatch->getTeam((TeamEnum) ti);

								for (PlayerIndex pi = 0; pi < team.getPlayerCount(); ++pi) {

									Player& player = team.getPlayer(pi);

									if (player.getAttachedController() == controller.ptr()) {

										currTeam = (TeamEnum) ti;
										currPlayer = &player;
										break;
									}
								}
							}

							bool hasNewTeam = false;
							TeamEnum newTeam = Team_Invalid;
							Time controllerAttachTime;

							{
								Input::SourceHolder source;

								if (controller->getSource(L"RightAxis", source, 0.0f)) { 

									SoftPtr<Input::Component_Generic1D> comp = Input::comp1D(source);

									if (comp.isValid()) {

										controllerAttachTime = source->getTime();

										if (!(currPlayer.isValid() && controllerAttachTime == currPlayer->getControllerAttachTime())) {

											if (comp->getValue() > 0.0f) {

												hasNewTeam = true;

												switch (currTeam) {

													case Team_A: newTeam = Team_Invalid; break;
													case Team_B: newTeam = Team_B; break;
													default: newTeam = Team_B; break;
												}
												
											} else if (comp->getValue() < 0.0f) {

												hasNewTeam = true;

												switch (currTeam) {

													case Team_A: newTeam = Team_A; break;
													case Team_B: newTeam = Team_Invalid; break;
													default: newTeam = Team_A; break;
												}
											}
										}
									}
								}
							}

							if (hasNewTeam) {

								if (currTeam != Team_Invalid && newTeam != currTeam) {

									mMatch->getTeam(currTeam).removePlayerWithController(controller);
								}

								if (newTeam != Team_Invalid && newTeam != currTeam) {

									mMatch->getTeam(newTeam).addPlayer(controller, controllerAttachTime);
								}

								currTeam = newTeam;
							}

							selectText.assign(controller->getSelectName());
							
							if (!selectText.isValid())
								selectText.assign(controller->getName());

							if (currTeam != Team_Invalid) {

								if (currTeam == Team_A)
									selectText.append(L" -> Team A");
								else
									selectText.append(L" -> Team B");
							}

							offsetX = -0.5f * ((float)(textMetrics->tmAveCharWidth * selectText.length()));
							texter.SetInsertionPos((int) (centerX + offsetX), (int) (centerY + offsetY));
							texter.DrawTextLine(selectText.c_tstr());


							offsetY += 22.0f;
						}
					}
				}

				offsetY += 22.0f;

				selectText.assign(L"Press Any Key ...");
				offsetX = -0.5f * ((float)(textMetrics->tmAveCharWidth * selectText.length()));
				texter.SetInsertionPos((int) (centerX + offsetX), (int) (centerY + offsetY));
				texter.DrawTextLine(selectText.c_tstr());

				texter.End();
			}
							
		} break;
	}
	
	mRenderer->end();
}


void Game::processConsoleCommand(const TCHAR* command) {

	if (mScriptEngine.isValid()) {

		mScriptEngine->run(command, ScriptEngine::Out_Console, ScriptEngine::Out_Console);
	}
}

bool Game::createMatch() {


	if (mMatch.isValid() || mRenderer.isNull()) {

		assrt(false);
		return false;
	}

	MMemNew(mMatch.ptrRef(), match::Match(dref(this), mNativeAppInfo.dref(), mRenderer.dref(), mLog.dref(), mConsole, mFramePerformance, mScriptEngine));

	mScriptEngine->redirectOutput(ScriptEngine::Out_Both);

	bool ret = true;

	if (mMatch->load()) {

		/*
		{
			ScriptBodyDynamics test;
			test.create(mScriptEngine, L"BodyDynamics_DynamicTackle");
			test.load(mScriptEngine, NULL, NULL);
		}
		*/

		mScriptEngine->redirectOutput(ScriptEngine::Out_Both);

	} else {

		ret = false;
	}

	if (!ret) {

		mMatch.destroy();
	}

	return (mMatch.isValid());
}

void Game::destroyMatch() {

	mMatch.destroy();
}


Time Game::getTime() {

	if (mMatch.isValid()) {

		return mMatch->getClock().mTime.t_discreet;
	}

	return 0;
}

}