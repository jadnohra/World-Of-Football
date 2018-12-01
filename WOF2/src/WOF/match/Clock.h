#ifndef h_WOF_match_Clock
#define h_WOF_match_Clock

#include "WE3/WETime.h"
using namespace WE;


namespace WOF { namespace match {


	class Clock {
	public:

		void init(const float& FPS, const unsigned int& physFPSMultiplier);
		void start();

		void add(const Time& fTime, const Time& fElapsedTime);
		void accept();
		void consume();

		inline const int& getPhysFPSMultiplier() const {

			return physFPSMultiplier;
		}
		
		inline const Time& getTime() const {

			return mTime.t_discreet;
		}

		inline const Time& getPhysTime() const {

			return mBallPhysTime.t_discreet;
		}

		const Time& getFrameMoveTime() const {

			return mTime.dt_discreet;
		}

		const Time& getPhysFrameMoveTime() const {

			return mBallPhysTime.dt_discreet;
		}

		const TickCount& getFramePhysTickCount() const {

			return mBallPhysTime.frameTicks;
		}

		const TickCount& getFrameMoveTickCount() const {

			return mTime.frameTicks;
		}

		const Time& getFramePhysTickLength() const {

			return mBallPhysTime.tickLength;
		}

		const Time& getFrameMoveTickLength() const {

			return mTime.tickLength;
		}
		
	public:

		int physFPSMultiplier;

		DiscreeteTime mTime;
		DiscreeteTime mBallPhysTime;

	};

} }

#endif