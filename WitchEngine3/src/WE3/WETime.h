#ifndef _WETime_h
#define _WETime_h

namespace WE {

	typedef float Time;
	typedef unsigned int TickCount;
	
	class DiscreeteTime {
	public:
		
		DiscreeteTime();
		
		void init(Time ticksPerSec);

		void start(Time t0);
		//void add(Time dt);

		
		void add(Time dt);
		void accept();
		void consume();

		/*
		void prepare() {
			simulStepSizeMillis = simulStepSizeSecs * 1000.0f;
		}
		*/
	public:

		Time ticksPerSec;
		Time tickLength;

		Time t0;
		
		Time t;
		Time t_discreet;

		Time dt;
		Time dt_discreet;
		TickCount frameTicks;

		Time added_dt;
		Time addded_dt_discreet;

		TickCount ticks;
		TickCount frames;


	protected:

		Time tentative_dt;
		Time tentative_dt_discreet;
		Time tentative_t;
		TickCount tentative_frameTicks;

	};

}

#endif