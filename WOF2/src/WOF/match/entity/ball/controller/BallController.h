#ifndef h_WOF_match_BallController
#define h_WOF_match_BallController

#include "WE3/WEPtr.h"
#include "WE3/WETime.h"
#include "WE3/math/WEVector.h"
#include "WE3/WEDataTypes.h"
using namespace WE;

#include "../../../Clock.h"

namespace WOF {  namespace match {

	class Match;
	class Ball;
	
	class BallController {
	protected:

		SoftPtr<Ball> mBall; 

	public:

		virtual ~BallController();

		virtual int getControllerType();

		bool isAttached();

		virtual void setPosition(const Point& pos);
		virtual void attach(Match& match, Ball* pBall);

		virtual bool handleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
		virtual void frameMove(Match& match, const Clock& time, Time& outEndTime) = 0;
		
		virtual void applyAfterTouch(const Vector3& acceleration, float rotSynRatio, float dt) {}

	protected:

		void handleBallPreDetach();
		void handleBallPostAttach();
	};
	
} }


#endif