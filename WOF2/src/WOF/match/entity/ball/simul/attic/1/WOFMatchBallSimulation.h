#ifndef _WOFMatchBallSimulation_h
#define _WOFMatchBallSimulation_h

#include "WOFMatchBallSimulationBase.h"
#include "BallSimulation2.h"

namespace WOF { namespace match {

	class Ball;
	class Match;

	struct SimulationProcessingRate {

		bool logPerformance;

		int frameCount;
		TickCount FPS;
		float durationPerFrame;
		int processingFrameCount;
		float maxDuration;


		SimulationProcessingRate();
		void load(Match& match, BufferString& tempStr, DataChunk& chunk, CoordSysConv* pConv);
	};

	class BallSimulation : public BallSimulation_Filtered_Trajectory, public BallTrajectory::RenderListener {
	public:

		BallSimulation();

		virtual void signalBallState(const Point& pos, const Vector3& v, const Vector3& w, const Time& time);
		virtual void onRenderedSample(Renderer& renderer, const BallTrajectory::Index& index, const BallTrajectory::Sample& sample, const float& radius, const RenderColor& color);

	public:

		typedef  BallTrajectory::Index Index;

		enum EventType {

			ET_None = 0, ET_Discontinuity
		};

		struct Event {

			EventType type;
			Index sampleIndex;
			Index relSampleIndex;

			inline void set(const EventType& _type, const Index& _sampleIndex) {

				type = _type;
				sampleIndex = _sampleIndex;
			}

			inline void set(const EventType& _type, const Index& _sampleIndex, const Index& _relSampleIndex) {

				type = _type;
				sampleIndex = _sampleIndex;
				relSampleIndex = _relSampleIndex;
			}
		};

	public:

		inline const bool& isValid() { return mIsValid; }
		inline const bool& isPartial() { return mIsPartial; }

		inline const SimulationID& getID() { return mID; }
		inline const SimulationID& getPartID() { return mPartID; }

		void enableEvent(EventType evt, bool enable);
		void setDiscontinuityEventAngle(float degAngle);

		inline const Index& getEventCount() const { return mEventCount; }
		const Event* getEvent(const Index& index) const { return &(mEvents.el[index]); }

		bool analyzeDiscontinuity(const Index& sampleIndex, const Index& relSampleIndex, Vector3& dir1, Vector3& dir2, bool& isDiscontinuity);

	public: 

		void reSyncToBall(Ball& ball, bool ballWasManipulated, const SimulationProcessingRate& rate);
		void render(Ball& ball, Renderer& renderer, const Index* pStartIndex = NULL, const Index* pEndIndex = NULL);
		
		const Index& getTimeValidSampleIndex() { return mTimeValidSampleIndex; }
		const Index& getTimeValidCollSampleIndex() { return mTimeValidCollSampleIndex; }
		bool searchForTime(const Time& absoluteTime, Index& sample1, Index& sample2); //if end resting sample is reached then sample1 = sample2 = last sample index

	protected:

		
		void reSimul(Ball& ball, bool newSimul, const SimulationProcessingRate& rate);
		void setValid(bool isValid) { mIsValid = isValid; }
		void setEmptySimul(Match& match, bool valid);
		void resetNew(const Time& time);
		void resetContinue();
		bool simulIsDesynced(Ball& ball);

	protected:

		Event* addEvent();
		Event* addEvent(const EventType& type, const Index& sampleIndex, const Index* pRelSampleIndex = NULL);

	protected:

		void render(Renderer& renderer, const float& ballRadiusStart, const float* pBallRadiusEnd = NULL, const Index* pStartIndex = NULL, const Index* pEndIndex = NULL);

	protected:
	
		bool mIsValid;
		SimulationID mID;
		SimulationID mPartID;

		bool mIsPartial;

		typedef WETL::SizeAllocT<Event, Index, false> Events;

		bool mEventDiscontinuityEnabled;
		float mDiscontinuityEventDeviation;

		Events::Index mEventCount;
		Events mEvents;

		Events::Index mRenderEvtIndex;

		Index mTimeValidSampleIndex;
		Index mTimeValidCollSampleIndex;

		BallSimulation2 mTestSimul;
	};

} }

#endif