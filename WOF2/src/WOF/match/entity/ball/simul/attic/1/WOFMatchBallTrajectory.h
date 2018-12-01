#ifndef _WOFMatchBallTrajectory_h
#define _WOFMatchBallTrajectory_h

#include "WE3/math/WEVector.h"
#include "WE3/WETL/WETLSizeAlloc.h"
#include "WE3/render/WERenderer.h"
using namespace WE;

#include "../../../WOFMatchTime.h"
#include "../../../WOFMatchDataTypes.h"


namespace WOF { namespace match {

	
	class BallTrajectory {
	public:

		typedef unsigned int Index;

		struct Sample {

			Point pos;
			Vector3 vel;

			Time time; //relative to Trajectory start time

			inline void set(const Point& _pos, const Vector3& _vel, const Time& _time) {

				pos = _pos;
				time = _time;
			}
		};

		class RenderListener {
		public:

			virtual void onRenderedSample(Renderer& renderer, const Index& index, const Sample& sample, const float& radius, const RenderColor& color) {};
		};

	public:

		BallTrajectory();

		void clear();
		void reserve(Index sampleCount, bool addCurrSampleCount = false); 

		void setStartTime(const Time& time, bool clear);
		
		inline const Time& getStartTime() { return mStartTime;}
		bool getEndTime(Time& endTime);

		Sample* addSample();
		Sample* addSample(const Point& pos, const Vector3& vel, const Time& time);

		inline const Index& getSampleCount() const { return mSampleCount; }
		const Sample* getSample(const Index& index) const { return &(mSamples.el[index]); }

		void render(Renderer& renderer, const float& ballRadiusStart, RenderListener* pListener = NULL, const float* pBallRadiusEnd = NULL, const RenderColor* pLineStart = NULL, const RenderColor* pLineEnd = NULL, const RenderColor* pSphereStart = NULL, const RenderColor* pSphereEnd = NULL, const Index* pStartIndex = NULL, const Index* pEndIndex = NULL);

	public:

		enum SamplingMethod {

			SM_None = 0, SM_Interval, SM_Distance
		};

		class Sampler {
		public:

			void setSampleMethod(SamplingMethod method);
			void setSampleInterval(const Time& interval, bool setMethod = true);
			void setSampleDistance(const float& distance, bool setMethod = true);

			const float& getSampleDistance() { return mSampleDistance; }

		protected:
		friend class BallTrajectory;

		//will add 0 to n actual samples
		void addSample(BallTrajectory& trajectory, const Point& pos, const Vector3& vel, const Time& time);

		protected:

			SamplingMethod mSampleMethod;
			Time mSampleInterval;
			float mSampleDistance;

			Sample mLastSample;
			float mTraveledDistance;
		};

	public:

		//will add 0 to n actual samples
		inline void addSample(Sampler& sampler, const Point& pos, const Vector3& vel, const Time& time) {

			sampler.addSample(*this, pos, vel, time);
		}

	protected:

		typedef WETL::SizeAllocT<Sample, Index, false> Samples;

		Time mStartTime;
		
		Index mSampleCount;
		Samples mSamples;
	};


	

} }

#endif