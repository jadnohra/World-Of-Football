#ifndef _WOF_match_BallListener_h
#define _WOF_match_BallListener_h

#include "WE3/Object/WEObject.h"

namespace WOF { namespace match {

	class Ball;
	struct BallCommand;

	/*
	class BallSimulStateListener {
	public:

		virtual void onBallChangedController(Ball& ball) {}
		virtual void onBallExecutedCommand(Ball& ball, const BallCommand* pCommand) {}
		virtual void onBallChangedIsSimulable(Ball& ball, const bool& isSimulable) {}
	};
	*/

	class BallSimulSyncListener {
	public:

		virtual void onBallSyncCommand(Ball& ball, const BallCommand* pCommand) {}
		virtual void onBallSyncSimulableStateChange(Ball& ball) {}
		virtual void onBallSyncPostCollision(Ball& ball, Object& object) {}
		virtual void onBallSyncBallState(Ball& ball, const Time& time) {}
	};

	/*
	class BallListener {
	public:

		virtual void onBallCollision(Object& object) {}
		virtual void onBallCommand(Object* pObject) {}
		virtual void onBallAfterTouchOwner(Object* pObject) {}
	};
	*/

	class BallOwner {
	public:

		virtual void onBallCollision(Ball& ball, Object& object, const unsigned int& objectPartID) {}
		virtual void onBallCommand(BallCommand& command) {}

		virtual void onBallAcquiredAfterTouch(Ball& ball) {}
		virtual void onBallLostAfterTouch(Ball& ball) {}
	};

} }

#endif