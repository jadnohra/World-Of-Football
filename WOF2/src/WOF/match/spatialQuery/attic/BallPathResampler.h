#ifndef _BallPathTimeResampler_h
#define _BallPathTimeResampler_h

#include "../entity/ball/ballSimul/BallSimulation2.h"

namespace WOF { namespace match {

	class BallPathTimeResampler {
	/*

	maybe should be part subclass of BallSimulation becuase it might depend on 
	the way it samples

	public:

		typedef BallTrajectory::Index SampleIndex;

		struct Sample {

			bool isValid;

			Vector3 pos;
			Time time;
			bool isResting;

			float speed;
			
			SampleIndex sample1;
			SampleIndex sample2;
			float lerpFactor;

			bool setup(const Time& time, BallSimulation& simul);
		};

	public:

		bool reset(BallSimulation* pSimul, const Time& startTime, const Time& interval);

		const Sample* getNextSample();

	protected:
	
		static bool searchForTime(const Time& _time, SampleIndex& sample1, SampleIndex& sample2);

	protected:
		
		SoftPtr<BallSimulation> mSimul;
		SimulationID mSimulID;

		Time mStartTime;
		Time mTimeInterval;
		Time mCurrTime;

		Sample mSample;
	*/
	};

} }

#endif