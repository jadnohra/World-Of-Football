#include "Match.h"

//#include "WE3/math/WEContactNormalSolver.h"
//#include "WE3/math/WEResponseCurve.h"
//#include "WE3/inputDevice/include.h"
//#include "WE3/input/include.h"
//#include "WE3/math/WEQuad.h"
//#include "spatialQuery/PitchRegionShape.h"
//#include "entity/camera/SceneCamera.h"
//#include "script/ScriptTeamFootballerIterator.h"
//#include "entity/footballer/Footballer.h"

namespace WOF { namespace match {

//Input::InputClient testInput(NULL);

/*

SoftPtr<Match> kMatch;

Input::Button testButton;
Input::Button::State testButtonState = Input::Button::State_None;

class TestClass : public Input::Tracker {
public:

	virtual void onSourceChanged(Input::SourceImpl* pVarImpl) {

		String dbg;
		Input::ButtonImpl* pButton = (Input::ButtonImpl*) pVarImpl;

		Input::Button::State newState = pButton->getButtonState(kMatch->getClock().getTime());

		switch (newState) {

			case Input::Button::State_None: {

				dbg.assignEx(L"none\n");

				switch (pButton->getButtonMode()) {


					case Input::Button::Mode_Tap: {

						dbg.assignEx(L"taps %d\n", pButton->getTapCount(true));

					} break;

					case Input::Button::Mode_Hold: {

						dbg.assignEx(L"held %f\n", pButton->getHoldTime(kMatch->getClock().getTime(), true));

					} break;

				}

			} break;

			case Input::Button::State_Tapping: {

				//dbg.assignEx(L"tap %d\n", testButton->getTapCount(false));

			} break;

			case Input::Button::State_Holding: {

				//dbg.assignEx(L"hold %f\n", testButton->getHoldTime(getClock().getTime(), false));

			} break;

			case Input::Button::State_On: {

				dbg.assignEx(L"on\n");

			} break;

			case Input::Button::State_Off: {

				dbg.assignEx(L"off\n");

			} break;
		}

		kMatch->getConsole().show(true, true);
		kMatch->getConsole().print(dbg.c_tstr(), true);

	}

};

TestClass kTestClass;
*/

void Match::runPreLoadTests() {

	//kMatch = this;

	/*
	ContactNormalSolver solver;

	solver.reset();
	solver.add().normal.set(-0.5f, 0.0f, 0.5f);
	solver.add().normal.set(0.5f, 0.0f, 0.5f);
	solver.add().normal.set(0.2f, 0.0f, 0.7f);
	//solver.add().normal.set(0.0f, 0.0f, -1.0f);
	
	const Vector3* pSol = solver.solve();
	*/

	/*
	for (int i = 0; i < 30; ++i) {

		log(L"%f", normalDist(0.5f, 0.1f));
	}

	log(L"*******************");

	for (int i = 0; i < 30; ++i) {

		log(L"%f", normalDist(0.5f, 0.3f));
	}

	log(L"*******************");
	
	for (int i = 0; i < 30; ++i) {

		log(L"%f", normalDist(0.5f, 1.0f));
	}

	log(L"*******************");
	
	for (int i = 0; i < 30; ++i) {

		log(L"%f", normalDist(0.5f, 5.0f));
	}
	*/

	/*
	float v;

	{
		ResponseCurve curve(1.5f, 0.0f, 1.0f, 0.0f, 1.0f, false);

		v = curve.map(-1.2f);
		v = curve.map(-0.5f);
		v = curve.map(0.0f);
		v = curve.map(0.5f);
		v = curve.map(1.0f);
		v = curve.map(1.2f);
	}

	{
		ResponseCurve curve(1.5f, 0.0f, 1.0f, 0.0f, 1.0f, true);

		v = curve.map(-1.2f);
		v = curve.map(-0.5f);
		v = curve.map(0.0f);
		v = curve.map(0.5f);
		v = curve.map(1.0f);
		v = curve.map(1.2f);
	}
	*/

	/*
	{
		Quad quad(true);

		Vector2 point(1.0f, 0.2f);

		quad.contains(point);
	}

	{
		Quad quad(true);

		quad.coordFrame.setupRotation(RigidMatrix3x2::Axis_X, degToRad(45.0f));
		quad.coordFrame.setTranslation(Vector2(10.0f, 50.0f));

		Vector2 point(1.0f, 0.2f);

		quad.contains(point);
	}
	*/

	/*
	{
		Quad quad(true);

		quad.coordFrame.setupRotation(RigidMatrix3x2::Axis_X, degToRad(45.0f));
		quad.coordFrame.setTranslation(Vector2(10.0f, 50.0f));
		quad.extents.set(0.5f, 2.0f);

		Quad quadIn(true);

		quadIn.coordFrame.setupRotation(RigidMatrix3x2::Axis_X, degToRad(45.0f));
		quadIn.coordFrame.setTranslation(Vector2(10.0f, 50.0f));
		quadIn.extents.set(0.49f, 1.99f);

		Quad quadOut(true);

		quadOut.coordFrame.setupRotation(RigidMatrix3x2::Axis_X, degToRad(45.0f));
		quadOut.coordFrame.setTranslation(Vector2(10.0f, 50.0f));
		quadOut.extents.set(0.51f, 2.01f);

		bool ok;
		Vector2 point;

		for (int i = 0; i < 4; ++i) {

			quadIn.getPoint(i, point);
			ok = quad.contains(point);

			quadOut.getPoint(i, point);
			ok = quad.contains(point);
		}

	}
	*/

	bool ok  = true;
}

void Match::runOnDestroyTests() {

	/*
	testButton->removeTracker(&kTestClass);
	*/
}

/*
RenderSprite sprite(true, true, true);
DimMap_2_3 dimMap;
*/

/*
PitchRegionShape_AAQuad kTestQuad;
RenderSprite kRenderSprite;
*/

void Match::runOnLoadTests() {

	/*
	{
		BallShotEstimator& shotEstim = getBallShotEstimator();
		BallSimulation& simul = shotEstim.getTempSimulPitchSimulation();

		float maxApogeeHeight = -1.0f;
		Time maxApogeeTime = -1.0f;
		BallSimulation::CollSample maxPostBounceCollSample;

		{

			int stepCount = 7;
			float maxSpeed = getCoordSys().convUnit(90.0f);
			int totalTrajSampleCount = 0;

			Vector3 vel;
			vel.zero();
			vel.el[Scene_Up] = maxSpeed;
			vel.el[Scene_Forward] = maxSpeed;

			if (shotEstim.simulateSingleVerticalBounce(vel, simul, false)) {

				maxApogeeHeight = simul.getSimulData()->mApogeeHeight;
				maxApogeeTime = simul.getSimulData()->mApogeeTime;
				maxPostBounceCollSample = simul.getCollSample(0);

				Log loggerBallUp;
				loggerBallUp.init(LOG_MASK_LOGLEVEL | LOG_MASK_FILEPATH | LOG_MASK_OPTIONBITS, L"", L"ballUp.train", LOG_FORCE, LOG_ENABLED_BIT | LOG_LOG_TOFILE_BIT);

				loggerBallUp.log(L"%d %d %d", stepCount, 1, 2);

				Log loggerBallUpTraj;
				loggerBallUpTraj.init(LOG_MASK_LOGLEVEL | LOG_MASK_FILEPATH | LOG_MASK_OPTIONBITS, L"", L"ballUpTraj.train", LOG_FORCE, LOG_ENABLED_BIT | LOG_LOG_TOFILE_BIT);

				loggerBallUpTraj.log(L"%d %d %d", 0, 2, 2);

				//Log loggerBounce;
				//loggerBounce.init(LOG_MASK_LOGLEVEL | LOG_MASK_FILEPATH | LOG_MASK_OPTIONBITS, L"", L"ballBounce.train", LOG_FORCE, LOG_ENABLED_BIT | LOG_LOG_TOFILE_BIT);

				//loggerBounce.log(L"%d %d %d", stepCount, 1, 2);
				
				vel.zero();

				for (int i = 0; i < stepCount; ++i) {

					vel.el[Scene_Fwd] = maxSpeed * ((float) (i + 1)) / ((float) stepCount);

					if (shotEstim.simulateSingleVerticalBounce(vel, simul, true)) {

						loggerBallUp.log(L"%f", vel.el[Scene_Up] / maxSpeed);
						loggerBallUp.log(L"%f", simul.getSimulData()->mApogeeHeight / maxApogeeHeight);
						loggerBallUp.log(L"%f", simul.getSimulData()->mApogeeTime / maxApogeeTime);

						for (BallSimulation::Index si = 0; si < simul.getSampleCount(); ++si) {

							const BallSimulation::TrajSample& sample = simul.getSample(si);

							if (sample.time <= simul.getSimulData()->mApogeeTime) {

								++totalTrajSampleCount;

								loggerBallUpTraj.log(L"%f", vel.el[Scene_Up] / maxSpeed);
								loggerBallUpTraj.log(L"%f", sample.time / maxApogeeTime);
								loggerBallUpTraj.log(L"%f", sample.pos.el[Scene_Up] / maxApogeeHeight);
								loggerBallUpTraj.log(L"%f", sample.vel.el[Scene_Up] / maxSpeed);

							} else {

								break;
							}
						}

						//loggerBounce.log(L"%f", apogeeHeight / maxApogeeHeight);
						//loggerBounce.log(L"%f", collSample.postVel.el[Scene_Up] / maxPostBounceCollSample.postVel.el[Scene_Up]);
						//loggerBounce.log(L"%f", collSample.time / maxPostBounceCollSample.time);
					}
				}

				loggerBallUpTraj.log(L"//samples: %d", totalTrajSampleCount);
			}
		}
	}
	*/

	/*
	{
		BallShotEstimator& shotEstim = getBallShotEstimator();
		BallSimulation& simul = shotEstim.getTempSimulPitchSimulation();

		float maxApogeeHeight = -1.0f;
		Time maxApogeeTime = -1.0f;
		BallSimulation::CollSample maxPostBounceCollSample;

		{

			int stepCount = 7;
			float maxSpeed = getCoordSys().convUnit(90.0f);
			int totalTrajSampleCount = 0;

			Vector3 vel;
			vel.zero();
			vel.el[Scene_Up] = maxSpeed;

			if (shotEstim.simulateSingleVerticalBounce(vel, simul, false)) {

				maxApogeeHeight = simul.getSimulData()->mApogeeHeight;
				maxApogeeTime = simul.getSimulData()->mApogeeTime;
				maxPostBounceCollSample = simul.getCollSample(0);

				Log loggerBallUp;
				loggerBallUp.init(LOG_MASK_LOGLEVEL | LOG_MASK_FILEPATH | LOG_MASK_OPTIONBITS, L"", L"ballUp.train", LOG_FORCE, LOG_ENABLED_BIT | LOG_LOG_TOFILE_BIT);

				loggerBallUp.log(L"%d %d %d", stepCount, 1, 2);

				Log loggerBallUpTraj;
				loggerBallUpTraj.init(LOG_MASK_LOGLEVEL | LOG_MASK_FILEPATH | LOG_MASK_OPTIONBITS, L"", L"ballUpTraj.train", LOG_FORCE, LOG_ENABLED_BIT | LOG_LOG_TOFILE_BIT);

				loggerBallUpTraj.log(L"%d %d %d", 0, 2, 2);

				//Log loggerBounce;
				//loggerBounce.init(LOG_MASK_LOGLEVEL | LOG_MASK_FILEPATH | LOG_MASK_OPTIONBITS, L"", L"ballBounce.train", LOG_FORCE, LOG_ENABLED_BIT | LOG_LOG_TOFILE_BIT);

				//loggerBounce.log(L"%d %d %d", stepCount, 1, 2);
				
				for (int i = 0; i < stepCount; ++i) {

					vel.el[Scene_Up] = maxSpeed * ((float) (i + 1)) / ((float) stepCount);

					if (shotEstim.simulateSingleVerticalBounce(vel, simul, true)) {

						loggerBallUp.log(L"%f", vel.el[Scene_Up] / maxSpeed);
						loggerBallUp.log(L"%f", simul.getSimulData()->mApogeeHeight / maxApogeeHeight);
						loggerBallUp.log(L"%f", simul.getSimulData()->mApogeeTime / maxApogeeTime);

						for (BallSimulation::Index si = 0; si < simul.getSampleCount(); ++si) {

							const BallSimulation::TrajSample& sample = simul.getSample(si);

							if (sample.time <= simul.getSimulData()->mApogeeTime) {

								++totalTrajSampleCount;

								loggerBallUpTraj.log(L"%f", vel.el[Scene_Up] / maxSpeed);
								loggerBallUpTraj.log(L"%f", sample.time / maxApogeeTime);
								loggerBallUpTraj.log(L"%f", sample.pos.el[Scene_Up] / maxApogeeHeight);
								loggerBallUpTraj.log(L"%f", sample.vel.el[Scene_Up] / maxSpeed);

							} else {

								break;
							}
						}

						//loggerBounce.log(L"%f", apogeeHeight / maxApogeeHeight);
						//loggerBounce.log(L"%f", collSample.postVel.el[Scene_Up] / maxPostBounceCollSample.postVel.el[Scene_Up]);
						//loggerBounce.log(L"%f", collSample.time / maxPostBounceCollSample.time);
					}
				}

				loggerBallUpTraj.log(L"//samples: %d", totalTrajSampleCount);
			}
		}
	}
	*/
	

	/*
	{
		ScriptTeamFootballerIterator iter;

		iter.setTeam(*this, Team_A);
		iter.setMaxConsecutiveNextCount(4);

		SoftPtr<ScriptedFootballer> footballer;

		while (true) {

			footballer = iter.next();
		}
	}
	*/

	/*
	{
		kRenderSprite.enableSkin(true);
		kRenderSprite.skin().enableColor(true);

		const PitchInfo& pitchInfo = getPitchInfo();

		kTestQuad.shape().min.x = getCoordSys().invConvUnit(pitchInfo.width) * -0.5f;
		kTestQuad.shape().max.x = getCoordSys().invConvUnit(pitchInfo.width) * 0.5f;

		kTestQuad.shape().min.y = getCoordSys().invConvUnit(pitchInfo.length) * -0.5f;
		kTestQuad.shape().max.y = getCoordSys().invConvUnit(pitchInfo.length) * 0.5f;
	}
	*/

	/*
	dimMap.mapX = Scene_Right;
	dimMap.mapY = Scene_Forward;
	dimMap.mapNone = Scene_Up;

	sprite.setDimMap(dimMap);

	sprite.setSize(100.0f, 200.0f);
	sprite.color().set(1.0f, 0.0f, 1.0f, 0.5f);
	sprite.transform3D().position().el[Scene_Up] = 100.0f;
	*/

	/*
	Matrix33 coordSysTransf;

	CoordSys source2DCoordSys;
	
	source2DCoordSys.setValid(true);

	source2DCoordSys.el0 = CSD_Right;
	source2DCoordSys.el1 = CSD_Forward;
	source2DCoordSys.el2 = CSD_Unused;
	source2DCoordSys.setFloatUnitsPerMeter(0.0f);

	setupCoordSysConvTransorm(source2DCoordSys, getBaseCoordSys(), coordSysTransf);

	sprite.transform().castToMatrix3x3Base() = coordSysTransf;
	*/
	

	/*
	{

		BallShotEstimator& estimator = getBallShotEstimator();

		bool ok;
		float outSpeed;

		
		{
			const TCHAR* groupName = L"Low";

			Vector3 dir = getCoordAxis(Scene_Forward);
			Vector3 from(0.0f);
			Vector3 to(0.0f);
			to.el[Scene_Forward] = getCoordSys().convUnit(10.0f);
			float t = 1.5f;
			float epsilon = getCoordSys().convUnit(0.2f);
			
			ok = estimator.estimateNeededShotSpeed(groupName, dir, from, to, t, epsilon, outSpeed);
		}
	
		{
			const TCHAR* groupName = L"Low";

			Vector3 dir = getCoordAxis(Scene_Right);
			Vector3 from(0.0f);
			Vector3 to(0.0f);
			to.el[Scene_Right] = getCoordSys().convUnit(10.0f);
			float t = 1.5f;
			float epsilon = getCoordSys().convUnit(0.2f);
			
			ok = estimator.estimateNeededShotSpeed(groupName, dir, from, to, t, epsilon, outSpeed);
		}
		
		{
			const TCHAR* groupName = L"Low";

			Vector3 dir = getCoordAxis(Scene_Right);
			Vector3 from(0.0f);
			Vector3 to(0.0f);
			to.el[Scene_Right] = 0.5f;
			to.el[Scene_Forward] = -0.5f;
			to.normalize();
			to.mul(getCoordSys().convUnit(10.0f));
			to.normalize(dir);
			float t = 1.5f;
			float epsilon = getCoordSys().convUnit(0.2f);
			
			ok = estimator.estimateNeededShotSpeed(groupName, dir, from, to, t, epsilon, outSpeed);
		}
		
	}
	*/

	/*
	{

		Input::InputManager& manager = getInputManager();

		SoftRef<Input::Controller> controller = manager.findController(L"Keyboard", true);

		if (controller.isValid()) {

			bool ok = controller->createButton(L"k", L"testk", 0.0f, testButton);
			testButton->setTapInterval(0.2f);
			testButton->setButtonMode(Input::Button::Mode_Tap);
			testButton->addTracker(&kTestClass);

			ok = ok;
		}
	}
	*/

	/*
	{

		Input::InputManager& manager = getInputManager();

		testInput.setManager(&manager);

		SoftRef<Input::Controller> controller = manager.findController(L"Keyboard", true);

		if (controller.isValid()) {

			Input::SourceHolder testButton;

			bool ok = controller->createButton(L"k", L"testk", 0.0f, testButton);

			if (ok) {

				SoftPtr<Input::Component_Button> butt = (Input::Component_Button*) testButton->getComponent(Input::ComponentID_Button);

				if (butt.isValid()) {

					butt->setTapInterval(0.2f);
					butt->setButtonMode(Input::Component_Button::Mode_Tap);
				}

				ok = testInput.addSource(testButton, true, true, 0);
				ok = testInput.removeSource(testButton);

				ok = testInput.addSource(testButton, true, true, 0);
				ok = testInput.removeSource(testInput.getSourceByID(0));

				ok = testInput.addSource(testButton, true, true, 0);
				testInput.destroy();

				ok = testInput.addSource(testButton, true, true, 0);
			}

			ok = ok;
		}
	}
	*/
}

void Match::runOnFrameMoveTests() {

	//k still buggy ... fix...

	/*
	Input::Button::State newState = testButton->getButtonState(getClock().getTime());

	if (newState != testButtonState) {

		String dbg;

		
	

		testButtonState = newState;
	}
	*/

	/*
	{

		String dbg;
		SoftPtr<Input::Source> evtVar = testInput.startIterateEvents();

		while (evtVar.isValid()) {

			Input::InputClient::IDType ID = -1;
			
			testInput.getSourceID(evtVar, ID);

			SoftPtr<Input::Component_Button> butt = (Input::Component_Button*) evtVar->getComponent(Input::ComponentID_Button);
			
			if (butt.isValid()) {

				if (butt->getButtonState(getClock().getTime()) == Input::Component_Button::State_None) {

					dbg.assignEx(L"taps %d\n", butt->getTapCount(true));

					getConsole().show(true, true);
					getConsole().print(dbg.c_tstr(), true);
				}
			}

			evtVar = testInput.nextEvent(); 
		}
	}
	*/
}

void Match::runOnRenderTests() {

	/*
	{
		mRenderer->setViewTransform2D(NULL);
		
		kTestQuad.render(mRenderer, kRenderSprite);
	}
	*/

	//sprite.render3D(mRenderer);
}

} }