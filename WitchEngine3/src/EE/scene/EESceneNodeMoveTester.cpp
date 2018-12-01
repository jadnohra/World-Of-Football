#include "EESceneNodeMoveTester.h"

#include "WE3/WEAssert.h"
using namespace WE;

namespace EE {


SceneNodeMoveTester::SceneNodeMoveTester() : usingCollTestData(false) {

	type = SE_MoveTest;
}

SceneNodeMoveTester::~SceneNodeMoveTester() {

	assrt(usingCollTestData == false);
}


void SceneNodeMoveTester::create(ICollDetector* pCollDetect) {

	if (usingCollTestData) {

		assrt(false);
		return;
	}

	if (pCollDetect) {

		pCollDetect->createTest(collTestData);
		usingCollTestData = true;
	}
}

void SceneNodeMoveTester::destroy(ICollDetector* pCollDetect) {

	if (usingCollTestData) {

		if (pCollDetect) {

			pCollDetect->destroyTest(collTestData);
			usingCollTestData = false;

		} else {

			assrt(false);
		}
	}
}

}