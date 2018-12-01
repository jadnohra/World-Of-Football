#include "WOFMatchBall.h"

#include "EE/scene/EESceneNodeType.h"
using namespace EE;

#include "../controller/ball/WOFMatchBallController.h"
#include "../WOFMatch.h"

namespace WOF {

MatchBall::MatchBall() {
}

bool MatchBall::bindNode(Match& match) {

	SceneEntity* mNode = this;

	//if (mNode.isValid()) 
	{

		const FastUnitCoordSys& coordSys = match.getCoordSys();

		PhysRigidBody::init(false);

		mass = 0.425f;
		massInv = 1.0f / mass;


		float radius = coordSys.convUnit(0.18f);

		if (mNode->mEntVolumeLocal.type == V_AAB) {

			radius = mNode->mEntVolumeLocal.toAAB().xSize() * 0.5f;
		} 
		
		float inertiaCoeff = 1.0f;
		
		ITensor.type = IT_Simple;

		float radius2 = powf(radius, 2.0f);
		ITensor.tensorSimple.IBody = (2.0f * mass * (radius2) / 3.0f) * inertiaCoeff;
		ITensor.tensorSimple.IBodyInv = 1.0f / ITensor.tensorSimple.IBody;
		
		//orient.fromMatrix(mSceneLocalTransform.castToCtMatrix3x3Base());
		orient.identity();


		mCollSphere.radius = radius;
		mCollSphere.center = mNode->mTransformLocal.getPosition();
	}

	return true;
}

void MatchBall::bindController(MatchBallController* pController) {

	mController = pController;
}

}