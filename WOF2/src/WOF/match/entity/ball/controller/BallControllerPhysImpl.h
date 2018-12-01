#ifndef h_WOF_match_BallControllerPhysImpl
#define h_WOF_match_BallControllerPhysImpl

#include "WE3/phys/WEPhysRigidBody.h"
#include "WE3/phys/WEPhysMaterial.h"
#include "WE3/phys/WEPhysBodyGroup.h"
#include "WE3/WETL/WETLArray.h"
using namespace WE;

#include "BallController.h"
#include "../../../collision/ContactGeometry.h"
#include "../../../inc/CollisionDef.h"
#include "../simul/BallSimulation.h"
#include "../BallCommand.h"

/*
	THE BIGGEST AND BEST IDEA HERE WOULD BE TO REPLACE THE 
	balanceBallPointVelocity by another method: none-instantaneous contact
	this would ballance the velocity automatically and 
	allow resorting to more srtandard physics!! (maybe)
*/

namespace WOF { namespace match {

	class Ball;

	class BallControllerPhysImpl : public BallController {
	public:

		BallControllerPhysImpl();
		virtual ~BallControllerPhysImpl();

		void init(Match& match);

		virtual int getControllerType();
		virtual void attach(Match& match, Ball* pBall);

		virtual void setPosition(const Point& pos, const BallCommand* pCommand); 
		void setVelocities(const Vector& v, const Vector& w, const BallCommand* pCommand); 
		void setV(const Vector& v, const BallCommand* pCommand); 
		void setW(const Vector& w, const BallCommand* pCommand); 
		void setV(const Vector& v, float blendCurrVRatio, const Vector& normal, float rollSyncRatio, bool blendCurrW, float blendRatio, const BallCommand* pCommand); 
		virtual void applyAfterTouch(const Vector3& acceleration, float rotSynRatio, float dt, const BallCommand* pCommand);

		virtual void frameMove(Match& match, const Clock& time, Time& outEndTime);

		virtual void debugReset();
		virtual void debugRender(Renderer& renderer, bool validOnly);

		bool executeCommand(const BallCommand& command);

	public:

		static void setPosition(Ball& ball, const Point& pos, const BallCommand* pCommand);
		static void setVelocities(Ball& ball, const Vector& v, const Vector& w, const BallCommand* pCommand); 

	public:

		void blend(const Ball& ball, const Vector& currV, const Vector* pCurrW, const Vector& v, float blendCurrVRatio, const Vector& normal, float rollSyncRatio, bool blendCurrW, float blendRatio, Vector& outV, Vector& outW);
		void simulate(Match& match, BallSimulation* pSimul2, Ball& ball, const Time& startTime, const TickCount& startTick, const Time& duration, const Time& tickLength, Time& outEndTime, TickCount& outEndTick);

	protected:

		void frameMove(Match& match, Ball& ball, const TickCount& tickCount, const Time& tickLength, const Time& startTime, const TickCount& startFrameIndex, BallSimulation* pSimul2, const bool& executeMaterials, Time& outEndTime, TickCount& outEndTick);

	protected:

		void wakeUpBall();

	protected:

		void determinIntersectionDynamics(SweptIntersection& intersection, Ball& ball, Vector& sweptVector, SweptContactDynamics& dynamics);

		void applyCollidingContact(Ball& ball, SweptIntersection& intersection, SweptContactDynamics& dynamics, const float& sweepTime, float& impulseMag);
		bool detectAndProcessContacts(Match& match, Ball& ball, const Time& tickLength, const Time& startTime, const TickCount& tickIndex, BallSimulation* pSimul2, const bool& executeMaterials, SimulationID& ballMoveTickID);

		void applyEnvEffects(Ball& ball, bool applyDrag, bool applyMagnus, const Time& dt);

		void balanceBallPointVelocity2(Ball& ball, SweptIntersection& intersection, SweptContactDynamics& dynamics, PhysMaterial& material, const float& dt);

		inline void checkAndSetResting(Ball& ball, const bool& allResting);

		void setupCurrContactBody(SweptIntersection& intersection);

	protected:

		PhysForceField mGravity;
		PhysRigidBody mStaticWorld;
		PhysRigidBody mDynamicBox;
		SoftPtr<PhysRigidBody> mCurrContactBody;

		float mAlmostRestingSpeed;

		ContactGeometry mContactGeometry;

		SweptIntersectionArray mSweptIntersections;
		SweptIntersectionPtrArray mSweptIntersectionPtrs;

		struct IntersectionState {

			bool wasProcessed;

			SweptFractionType fractionType;
			SweptContactDynamics dynamics;
			SweptContactDynamicType dynamicType;
			
		};

		typedef WETL::CountedArray<IntersectionState, false, ContactIndexType> IntersectionStates;
		IntersectionStates mIntersectionStates;

		SweptIntersectionArray mRestingIntersections;
	};

} } 


#endif