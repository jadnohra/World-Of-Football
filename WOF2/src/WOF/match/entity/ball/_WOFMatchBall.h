#ifndef _WOFMatchBall_h
#define _WOFMatchBall_h

#include "WE3/WEPtr.h"
#include "WE3/phys/WEPhysRigidBody.h"
#include "WE3/math/WESphere.h"
using namespace WE;

#include "EE/scene/entity/EESceneEntityMesh.h"
using namespace EE;

namespace WOF {

	class MatchBallController;
	class Match;

	class MatchBall : public SceneEntityMesh, public PhysRigidBody {
	public:

		MatchBall();

		bool bindNode(Match& match);
		void bindController(MatchBallController* pController);

	public:

		SoftPtr<MatchBallController> mController;

		Sphere mCollSphere;
	};

}

#endif